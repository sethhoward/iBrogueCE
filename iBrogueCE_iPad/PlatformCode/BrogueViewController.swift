 //
//  BrogueViewController.swift
//  iBrogue_iPad
//
//  This file is part of Brogue.
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as
//  published by the Free Software Foundation, either version 3 of the
//  License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

//import UIKit
import SpriteKit



fileprivate let kESCKey = "\u{1B}"    // 27
fileprivate let kDelKey = "\u{B1}"    // 177
fileprivate let kEnterKey = "\n"

private func synchronized<T>(_ lock: Any, _ body: () throws -> T) rethrows -> T {
    objc_sync_enter(lock)
    defer { objc_sync_exit(lock) }
    return try body()
}

fileprivate let COLS = 100
fileprivate let ROWS = 34
 
 // Because of a main thread issue we need to set this.
 fileprivate var kOffset: CGFloat = 0

extension UIScreen {
    @objc static var safeBounds: CGRect {
        var offset: CGFloat =  0.0
        if #available(iOS 11.0, *) {
            if Thread.isMainThread {
                offset = UIApplication.shared.windows.first(where: \.isKeyWindow)?.safeAreaInsets.bottom ?? 0
                kOffset = offset
            } else {
                offset = kOffset
            }
        }
        
        return CGRect(x: 0, y: 0, width: UIScreen.main.bounds.width, height: UIScreen.main.bounds.height - offset)
    }
}

fileprivate func getCellCoords(at point: CGPoint) -> CGPoint {
    let cellx = Int(CGFloat(COLS) * point.x / UIScreen.main.bounds.size.width)
    let celly = Int(CGFloat(ROWS) * point.y / (UIScreen.safeBounds.size.height))
    
    return CGPoint(x: cellx, y: celly)
}

// TODO: switch to Character
extension String {
    var ascii: UInt8 {
        return (unicodeScalars.map { UInt8(min(255,$0.value)) }).first!
    }
}

// MARK: - UIBrogueTouchEvent

@objc class UIBrogueTouchEvent: NSObject, NSCopying {
    @objc let phase: UITouch.Phase
    @objc let location: CGPoint
    
    required init(phase: UITouch.Phase, location: CGPoint) {
        self.phase = phase
        self.location = location
    }
    
    required init(touchEvent: UIBrogueTouchEvent) {
        phase = touchEvent.phase
        location = touchEvent.location
    }
    
    func copy(with zone: NSZone? = nil) -> Any {
        return type(of:self).init(touchEvent: self)
    }
}

// MARK: - BrogueGameEvent

extension BrogueGameEvent {
    var canShowMagnifyingGlass: Bool {
        switch self {
        case .startNewGame, .inventoryItemAction, .confirmationComplete, .actionMenuClose, .closedInventory, .openGame:
            return true
        default:
            return false
        }
    }
}

// MARK: - BrogueViewController

final class BrogueViewController: UIViewController {
    fileprivate var keyboardDetectedKeyEvent : Bool  = false
    fileprivate var touchEvents = [UIBrogueTouchEvent]()
    fileprivate var lastTouchLocation = CGPoint()
    @objc fileprivate var directionsViewController: DirectionControlsViewController?
    fileprivate var keyEvents = [UInt8]()
    fileprivate var magnifierTimer: Timer?
    fileprivate var inputRequestString: String?
    fileprivate var shiftModifierPressed: Bool = false
    fileprivate var controlModifierPressed: Bool = false
    
    @IBOutlet var skViewPort: SKViewPort!
    @IBOutlet fileprivate weak var magView: SKMagView!
    @IBOutlet fileprivate weak var escButton: UIButton! {
        didSet {
            escButton.isHidden = true
        }
    }
    @IBOutlet fileprivate weak var inputTextField: UITextField!
  //  @IBOutlet fileprivate weak var leaderBoardButton: UIButton!
 //   @IBOutlet fileprivate weak var seedButton: UIButton!
   
    @IBOutlet weak var dContainerView: UIView!
//    @objc var seedKeyDown = false
    @objc var lastBrogueGameEvent: BrogueGameEvent = .showTitle {
        didSet {
            DispatchQueue.main.async {
                //default visibility for ESC button
                self.escButton.isHidden = true
//
                switch self.lastBrogueGameEvent {
                case .keyBoardInputRequired:
                    if ( !self.keyboardDetectedKeyEvent) {
                        // only display a screen keyboard if we haven't seen any key presses
                        self.inputTextField.becomeFirstResponder()
                    }
                case .showTitle, .openGameFinished:
                    self.inputTextField.resignFirstResponder()
                    self.escButton.isHidden = true
                case .startNewGame, .openGame, .beginOpenGame:
                    self.escButton.isHidden = true
                default: self.escButton.isHidden = true
                    
                }

                // Hide/Show the directions.
                switch self.lastBrogueGameEvent {
                case .waitingForConfirmation, .actionMenuOpen, .openedInventory, .showTitle, .openGameFinished, .playRecording, .showHighScores, .playBackPanic, .messagePlayerHasDied, .playerHasDiedMessageAcknowledged, .keyBoardInputRequired, .beginOpenGame:
                    self.dContainerView.isHidden = true
                    self.dContainerView.isUserInteractionEnabled = false
                default:
                    self.dContainerView.isHidden = false
                    self.dContainerView.isUserInteractionEnabled = true
                }
            }
        }
    }
    
    override var prefersHomeIndicatorAutoHidden: Bool {
        return true
    }
    
    override var preferredScreenEdgesDeferringSystemGestures: UIRectEdge {
        return .all
    }

    
    override func viewDidLoad() {
        super.viewDidLoad()
        // TODO: clean this up
        RogueDriver.sharedInstance(with: skViewPort, viewController: self)

        let thread = Thread(target: self, selector: #selector(BrogueViewController.playBrogue), object: nil)
        thread.stackSize = 400 * 8192
        thread.start()
        
        magView.viewToMagnify = skViewPort
        magView.hideMagnifier()
        inputTextField.delegate = self
        
        let panGesture = UIPanGestureRecognizer(target: self, action: #selector(draggedView(_:)))
        panGesture.minimumNumberOfTouches = 2
        dContainerView.addGestureRecognizer(panGesture)
        
        if #available(iOS 11.0, *) {
            setNeedsUpdateOfHomeIndicatorAutoHidden()
        }
    }
    
    @objc func handleDirectionTouch(_ sender: UIPanGestureRecognizer) {
        directionsViewController?.cancel()
    }
    
    @objc func draggedView(_ sender: UIPanGestureRecognizer) {
        directionsViewController?.cancel()
        let translation = sender.translation(in: view)
        dContainerView.center = CGPoint(x: dContainerView.center.x + translation.x, y: dContainerView.center.y + translation.y)
        sender.setTranslation(CGPoint.zero, in: view)
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        if segue.destination is DirectionControlsViewController {
            directionsViewController = segue.destination as? DirectionControlsViewController
            addObserver(self, forKeyPath: #keyPath(directionsViewController.directionalButton), options: [.new], context: nil)
        }
    }
    
    @objc private func playBrogue() {
        rogueMain()
    }
}
 
extension BrogueViewController {
    @IBAction func escButtonPressed(_ sender: Any) {
        addKeyEvent(event: kESCKey.ascii)
        inputTextField.resignFirstResponder()
    }
    
//    @IBAction func seedButtonPressed(_ sender: Any) {
//        seedKeyDown = !seedKeyDown
//
//        if seedKeyDown {
//            let image = UIImage(named: "brogue_sproutedseed.png")
//            seedButton.setImage(image, for: .normal)
//        } else {
//            let image = UIImage(named: "brogue_seed.png")
//            seedButton.setImage(image, for: .normal)
//        }
//    }
}

extension BrogueViewController {
    override func motionBegan(_ motion: UIEvent.EventSubtype, with event: UIEvent?) {
        addKeyEvent(event: kESCKey.ascii)
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        super.touchesBegan(touches, with: event)
        
        guard dContainerView.hitTest(touches.first!.location(in: dContainerView), with: event) == nil else { return }
        
        for touch in touches {
            let location = touch.location(in: skViewPort)
            // handle double tap on began.
            if touch.tapCount >= 2 && pointIsInPlayArea(point: location) {
                // double tap in the play area
                addTouchEvent(event: UIBrogueTouchEvent(phase: .stationary, location: lastTouchLocation))
                addTouchEvent(event: UIBrogueTouchEvent(phase: .moved, location: lastTouchLocation))
                addTouchEvent(event: UIBrogueTouchEvent(phase: .ended, location: lastTouchLocation))
            }
            else {
                let brogueEvent = UIBrogueTouchEvent(phase: touch.phase, location: location)
                addTouchEvent(event: brogueEvent)
                showMagnifier(at: location)
            }
        }
    }
    
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        super.touchesMoved(touches, with: event)
        
        guard dContainerView.hitTest(touches.first!.location(in: dContainerView), with: event) == nil else { return }
        
        if let touch = touches.first {
            let location = touch.location(in: skViewPort)
            let brogueEvent = UIBrogueTouchEvent(phase: touch.phase, location: location)

            addTouchEvent(event: brogueEvent)
            showMagnifier(at: location)
        }
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        super.touchesEnded(touches, with: event)
        
        guard dContainerView.hitTest(touches.first!.location(in: dContainerView), with: event) == nil else { return }
        
        if let touch = touches.first {
            let location = touch.location(in: skViewPort)
            
            if pointIsInSideBar(point: location) && lastBrogueGameEvent != .openedInventory {
                // side bar
                if touch.tapCount >= 2 {
                    addTouchEvent(event: UIBrogueTouchEvent(phase: .ended, location: lastTouchLocation))
                } else {
                    addTouchEvent(event: UIBrogueTouchEvent(phase: .moved, location: location))
                }
            } else {
                // other touch
                addTouchEvent(event: UIBrogueTouchEvent(phase: .stationary, location: lastTouchLocation))
                addTouchEvent(event: UIBrogueTouchEvent(phase: .ended, location: lastTouchLocation))
                
                if pointIsInPlayArea(point: location) && lastBrogueGameEvent != .openedInventory && lastBrogueGameEvent != .inventoryItemAction && lastBrogueGameEvent != .showTitle && lastBrogueGameEvent != .waitingForConfirmation && lastBrogueGameEvent != .actionMenuOpen {
                    addTouchEvent(event: UIBrogueTouchEvent(phase: .ended, location: lastTouchLocation))
                }
            }
        }
        
        hideMagnifier()
    }
    
    fileprivate func pointIsInPlayArea(point: CGPoint) -> Bool {
        let cellCoord = getCellCoords(at: point)
        if cellCoord.x > 20 && cellCoord.y < 32 && cellCoord.y > 3 {
            return true
        }
        
        return false
    }
    
    private func pointIsInSideBar(point: CGPoint) -> Bool {
        let cellCoord = getCellCoords(at: point)
        if cellCoord.x <= 20 {
            return true
        }
        
        return false
    }
    
    private func addTouchEvent(event: UIBrogueTouchEvent) {
        lastTouchLocation = event.location
        synchronized(touchEvents) {
            // only want the last moved event, no point caching them all
            if let lastEvent = touchEvents.last, lastEvent.phase == .moved && hasTouchEvent() {
                _ = touchEvents.removeLast()
            }
            
            touchEvents.append(event)
        }
    }
    
    private func clearTouchEvents() {
        synchronized(touchEvents) {
            touchEvents.removeAll()
        }
    }
    
    @objc func dequeTouchEvent() -> UIBrogueTouchEvent? {
        var event: UIBrogueTouchEvent?
        
        synchronized(touchEvents) {
            if !touchEvents.isEmpty {
                event = touchEvents.removeFirst()
                event = event?.copy() as? UIBrogueTouchEvent
            }
        }
        
        return event
    }
    
    @objc func hasTouchEvent() -> Bool {
        return !touchEvents.isEmpty
    }
    
    @objc func keyboardDetected() -> Bool {
        return self.keyboardDetectedKeyEvent
    }
    
    @objc func shiftKeyDown() -> Bool {
        return self.shiftModifierPressed
    }
    
    @objc func controlKeyDown() -> Bool {
        return self.controlModifierPressed
    }
}

extension BrogueViewController {
    @objc private func handleMagnifierTimer() {
        if canShowMagnifier(at: lastTouchLocation) {
            magView.showMagnifier(at: lastTouchLocation)
        }
    }
    
    private func canShowMagnifier(at point: CGPoint) -> Bool {
        return lastBrogueGameEvent.canShowMagnifyingGlass && pointIsInPlayArea(point: point)
    }
    
    fileprivate func showMagnifier(at point: CGPoint) {
        guard canShowMagnifier(at: point) else {
            magView.hideMagnifier()
            return
        }
        
        if magView.isHidden {
            magnifierTimer?.invalidate()
            magnifierTimer = nil
            magnifierTimer = Timer.scheduledTimer(timeInterval: 0.2, target: self, selector: #selector(BrogueViewController.handleMagnifierTimer), userInfo: nil, repeats: false)
            // Need to go iOS 10
            //            magnifierTimer = Timer.scheduledTimer(withTimeInterval: 1.0, repeats: false) { _ in
            //                self.magView.showMagnifier(at: self.lastTouchLocation)
            //            }
        } else {
            magView.updateMagnifier(at: point)
        }
    }
    
    fileprivate func hideMagnifier() {
        magnifierTimer?.invalidate()
        magnifierTimer = nil
        DispatchQueue.main.async {
            self.magView.hideMagnifier()
        }
    }
}

extension BrogueViewController {
    
    override func observeValue(forKeyPath keyPath: String?, of object: Any?, change: [NSKeyValueChangeKey : Any]?, context: UnsafeMutableRawPointer?) {
        guard keyPath == #keyPath(directionsViewController.directionalButton) else { return }

        if let tag = directionsViewController?.directionalButton?.tag, let direction = ControlDirection(rawValue: tag) {
            var key: String
            switch direction {
            case .up:
                key = kUP_Key
            case .right:
                key = kRIGHT_key
            case .down:
                key = kDOWN_key
            case .left:
                key = kLEFT_key
            case .upLeft:
                key = kUPLEFT_key
            case .upRight:
                key = kUPRight_key
            case .downRight:
                key = kDOWNRIGHT_key
            case .downLeft:
                key = kDOWNLEFT_key
            case .catchAll:
                return
            }
            
            addKeyEvent(event: key.ascii)
        }
    }
    
    fileprivate func addKeyEvent(event: UInt8) {
        synchronized(touchEvents) {
            keyEvents.append(event)
        }
    }
    
    // cannot be optional for backward compat
    @objc func dequeKeyEvent() -> UInt8 {
        var event: UInt8!
        
        synchronized(keyEvents) {
            if !keyEvents.isEmpty {
                event = keyEvents.removeFirst()
            } else {
                fatalError("Deque Key, queue is empty")
            }
        }
        
        return event
    }
    
    @objc func hasKeyEvent() -> Bool {
        return !keyEvents.isEmpty
    }
}

// MARK: - Handle Key input from screen or keyboard
extension BrogueViewController: UITextFieldDelegate {
    @objc func requestTextInput(for string: String) {
        inputRequestString = string
        
        DispatchQueue.main.async {
            self.escButton.isHidden = false
            if ( !self.keyboardDetectedKeyEvent ) {
                self.inputTextField.becomeFirstResponder()
            }
        }
    }
    
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        inputTextField.resignFirstResponder()
        addKeyEvent(event: "\n".ascii)
        self.escButton.isHidden = true
        return true
    }
    
    func textFieldDidBeginEditing(_ textField: UITextField) {
        inputTextField.text = inputRequestString ?? ""
        escButton.isHidden = false
    }
    
    func textFieldDidEndEditing(_ textField: UITextField) {
        escButton.isHidden = true
//        seedButton.isHidden = true
    }
    
    func textField(_ textField: UITextField, shouldChangeCharactersIn range: NSRange, replacementString string: String) -> Bool {
        if string.isEmpty {
            addKeyEvent(event: kDelKey.ascii)
        } else {
            // string may be multiple characters, if a paste operation occured
            for char in string {
                addKeyEvent(event: char.asciiValue ?? " ".ascii)
            }
        }
        return true
    }
    
    override func pressesBegan(_ presses: Set<UIPress>, with event: UIPressesEvent?) {
        // TODO: set a timer or otherwise start a key repitition simulation
        
            guard let key = presses.first?.key else { return }
            shiftModifierPressed = key.modifierFlags.contains([.shift,.alphaShift])
            controlModifierPressed = key.modifierFlags.contains(.control)
      }
    
    
    override func pressesEnded(_ presses: Set<UIPress>, with event: UIPressesEvent?) {
        
        keyboardDetectedKeyEvent = true // once it's true, it stays true, even if the keyboard is no longer used.
        
        shiftModifierPressed = false
        controlModifierPressed = false
        
         for press in presses {
            guard let key = press.key else { continue }
             
             // handle special case of Command-v to allow pasting seeds from contests
             // right now, this could happen anywhere, so careful if you paste while the game
             // is running.
             if (key.keyCode == .keyboardV &&
                 (key.modifierFlags.contains(.command) &&
                  !key.modifierFlags.contains([.shift,.alternate,.control]))) {
                 // they entered Command-V, without other modifiers
                 if (UIPasteboard.general.hasStrings) {
                     let seedData = UIPasteboard.general.string!
                     for ch in seedData {
                         if ch.isASCII && ch.isNumber {
                             addKeyEvent(event: ch.asciiValue!)
                         }
                     }
                 }
             }
            
            var sendKey: String = ""
            // this is a physical key, independent of modifiers such as CTRL, OPTION, SHIFT
            switch key.keyCode {
                
            // handle cardinal arrow keys, and keypad equivalents
            case .keyboardUpArrow, .keypad8 :
                sendKey = kUP_Key
            case .keyboardDownArrow, .keypad2 :
                sendKey = kDOWN_key
            case .keyboardLeftArrow, .keypad4 :
                sendKey = kLEFT_key
            case .keyboardRightArrow, .keypad6 :
                sendKey = kRIGHT_key
                
            // handle remaining keypad arrow equivalents
            case .keypad7 :
                sendKey = kUPLEFT_key
            case .keypad9 :
                sendKey = kUPRight_key
            case .keypad1 :
                sendKey = kDOWNLEFT_key
            case .keypad3 :
                sendKey = kDOWNRIGHT_key
                
            // handle special keys, ESC, Enter, DEL, Backspace
            case .keyboardEscape :
                escButton.isHidden = true
                sendKey = kESCKey
            case .keyboardReturnOrEnter :
                inputTextField.resignFirstResponder()   // whenever Enter, make the onscreen keyboard go away
                escButton.isHidden = true
                sendKey = kEnterKey
            case .keyboardDeleteOrBackspace, .keyboardDeleteForward :
                sendKey = kDelKey
            default :
                sendKey = key.characters        // DON'T ignore modifiers, to allow shifted letters
            }
            if !sendKey.isEmpty {
                addKeyEvent(event: sendKey.ascii)
            }
        }
    }
}



// MARK: - SKMagView

final class SKMagView: SKView {
    var viewToMagnify: SKViewPort?
    // TODO: magic numbers
    private var size = CGSize(width: 110, height: 110)
    private var offset = CGSize(width: 60, height: -27)
    private let parentNode: SKNode
    private var cells: [Cell]? {
        willSet {
            parentNode.removeAllChildren()
        }
        didSet {
            for cell in cells! {
                parentNode.addChild(cell.background)
                parentNode.addChild(cell.foreground)
            }
        }
    }
    
    required init?(coder aDecoder: NSCoder) {
        parentNode = SKSpriteNode(color: .cyan, size: size)
        super.init(coder: aDecoder)
        
        let styleWindow: () -> Void = {
            self.frame = CGRect(x: 0, y: 0, width: self.size.width, height: self.size.height)
            self.layer.borderColor = UIColor(red: 1, green: 1, blue: 1, alpha: 0.4).cgColor
            self.layer.borderWidth = 3
            self.layer.cornerRadius = self.frame.size.width / 2
            self.layer.masksToBounds = true
            self.backgroundColor = .black
        }
        
        var scene: SKScene {
            let scene = SKScene(size: self.size)
            scene.scaleMode = .aspectFit
            scene.addChild(self.parentNode)
            return scene
        }
        
        styleWindow()
        presentScene(scene)
    }
    
    func showMagnifier(at point: CGPoint) {
        cells = cellsAtTouch(point: point)
        center = CGPoint(x: point.x + size.width / 2 - offset.width, y: point.y - size.height / 2 + offset.height)
        isHidden = false
    }
    
    func updateMagnifier(at point: CGPoint) {
        showMagnifier(at: point)
    }
    
    func hideMagnifier() {
        isHidden = true
        parentNode.removeAllChildren()
    }
    
    private func cellsAtTouch(point: CGPoint) -> [Cell] {
        guard let viewToMagnify = viewToMagnify else { return [Cell]() }
       
        let magnification: CGFloat = 1.0
        let currentCellXY = getCellCoords(at: point)
        let rows = 3 // opposite/flipped
        let cols = 2
        
        let cells: [[Cell]] = {
            var cells = [[Cell]]()
            
            for x in -(rows)...rows {
                var row = [Cell]()
                for y in -(cols)...cols {
                    let indexX = Int(currentCellXY.x) + x
                    let indexY = Int(currentCellXY.y) + y
                    // don't try to draw anything out of bounds
                    if indexX < COLS && indexY < ROWS && indexX >= 0 && indexY >= 0 {
                        let newCell = MagCell(cell: viewToMagnify.rogueScene.cells[indexX][indexY], magnify: magnification)
                        row.append(newCell)
                    } else {
                        let cell = Cell(x: 0, y: 0, size: viewToMagnify.rogueScene.cells[0][0].size)
                        cell.bgcolor = .black
                        row.append(cell)
                    }
                }
                cells.append(row);
            }
            
            return cells
        }()
        
        let cellSize = cells[0][0].size
        
        // layout cells
        for x in 0...rows * 2 {
            for y in 0...cols * 2 {
                cells[x][y].position = CGPoint(x: (CGFloat(x) * cellSize.width), y: CGFloat(rows - y - 1) * cellSize.height)
            }
        }
        
        var position: CGPoint {
            let screenScale = UIScreen.main.scale
            let magnificationOffset = magnification + 1
            
            // take the touch point and figure out how far off from 0,0 inside the node we are. Magical fudge of magoffset ensure we move smoothly from one cell to the next.
            let xMouseOffset = (point.x - (currentCellXY.x * (viewToMagnify.rogueScene.cells[0][0].size.width / screenScale))) * magnificationOffset
            let yMouseOffset = (point.y - (currentCellXY.y * (viewToMagnify.rogueScene.cells[0][0].size.height / screenScale))) * magnificationOffset
            
            // center cell is 3,2 and should be in the middle of the magnifying glass view. As touches move so does the view need to move to follow.
            let xFinalOffset = ((CGFloat(rows) * cellSize.width - self.size.width/2) + cellSize.width/2) + xMouseOffset
            let yFinalOffset = ((CGFloat(rows - cols - 1) * cellSize.height - self.size.height / 2) + cellSize.height / 2) - yMouseOffset
            
            return CGPoint(x: -xFinalOffset + cellSize.width / 2, y: -yFinalOffset - cellSize.height / 2)
        }
        
        // offset needs to be offset by the appropriate cellsize.
        parentNode.position = position
        
        return cells.flatMap { $0 }
    }
}
