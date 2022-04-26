#include "Rogue.h"

#define U_MIDDLE_DOT  0x00b7
#define U_FOUR_DOTS  0x2237
#define U_DIAMOND  0x25c7
#define U_FLIPPED_V  0x22CF
#define U_ARIES  0x2648
#define U_ESZETT  0x00df
#define U_ANKH  0x2640
#define U_MUSIC_NOTE  0x266A
#define U_CIRCLE  0x25CB
#define U_LIGHTNING_BOLT  0x03DF
#define U_FILLED_CIRCLE  0x25cf
#define U_NEUTER  0x26b2
#define U_U_ACUTE  0x00da
#define U_CURRENCY 0x00A4
#define U_UP_ARROW  0x2191
#define U_DOWN_ARROW  0x2193
#define U_LEFT_ARROW  0x2190
#define U_RIGHT_ARROW  0x2192
#define U_OMEGA  0x03A9
#define U_CIRCLE_BARS  0x29F2
#define U_FILLED_CIRCLE_BARS  0x29F3

// define characters in the Brogue CE font
// note that these are the same as (glyph - 130)+0x4000
#define U_TILES_POTION 0x4000
#define U_TILES_GRASS 0x4001
#define U_TILES_WALL 0x4002
#define U_TILES_DEMON 0x4003
#define U_TILES_OPEN_DOOR 0x4004
#define U_TILES_GOLD 0x4005
#define U_TILES_CLOSED_DOOR 0x4006
#define U_TILES_RUBBLE 0x4007
#define U_TILES_KEY 0x4008
#define U_TILES_BOG 0x4009
#define U_TILES_CHAIN_TOP_LEFT 0x400A
#define U_TILES_CHAIN_BOTTOM_RIGHT 0x400B
#define U_TILES_CHAIN_TOP_RIGHT 0x400C
#define U_TILES_CHAIN_BOTTOM_LEFT 0x400D
#define U_TILES_CHAIN_TOP 0x400E
#define U_TILES_CHAIN_BOTTOM 0x400F
#define U_TILES_CHAIN_LEFT 0x4010
#define U_TILES_CHAIN_RIGHT 0x4011
#define U_TILES_FOOD 0x4012
#define U_TILES_UP_STAIRS 0x4013
#define U_TILES_VENT 0x4014
#define U_TILES_DOWN_STAIRS 0x4015
#define U_TILES_PLAYER 0x4016
#define U_TILES_BOG_MONSTER 0x4017
#define U_TILES_CENTAUR 0x4018
#define U_TILES_DRAGON 0x4019
#define U_TILES_FLAMEDANCER 0x401A
#define U_TILES_GOLEM 0x401B
#define U_TILES_TENTACLE_HORROR 0x401C
#define U_TILES_IFRIT 0x401D
#define U_TILES_JELLY 0x401E
#define U_TILES_KRAKEN 0x401F
#define U_TILES_LICH 0x4020
#define U_TILES_NAGA 0x4021
#define U_TILES_OGRE 0x4022
#define U_TILES_PHANTOM 0x4023
#define U_TILES_REVENANT 0x4024
#define U_TILES_SALAMANDER 0x4025
#define U_TILES_TROLL 0x4046
#define U_TILES_UNDERWORM 0x4027
#define U_TILES_VAMPIRE 0x4028
#define U_TILES_WRAITH 0x4029
#define U_TILES_ZOMBIE 0x402A
#define U_TILES_ARMOR 0x402B
#define U_TILES_STAFF 0x402C
#define U_TILES_WEB 0x402D
#define U_TILES_MOUND 0x402E
#define U_TILES_BLOAT 0x402F
#define U_TILES_CENTIPEDE 0x4030
#define U_TILES_DAR_BLADEMASTER 0x4031
#define U_TILES_EEL 0x4032
#define U_TILES_FURY 0x4033
#define U_TILES_GOBLIN 0x4034
#define U_TILES_IMP 0x4035
#define U_TILES_JACKAL 0x4036
#define U_TILES_KOBOLD 0x4037
#define U_TILES_MONKEY 0x4038
#define U_TILES_PIXIE 0x4039
#define U_TILES_RAT 0x403A
#define U_TILES_SPIDER 0x403B
#define U_TILES_TOAD 0x403C
#define U_TILES_BAT 0x403D
#define U_TILES_WISP 0x403E
#define U_TILES_PHOENIX 0x403F
#define U_TILES_ALTAR 0x4040
#define U_TILES_LIQUID 0x4041
#define U_TILES_FLOOR 0x4042
#define U_TILES_CHASM 0x4043
#define U_TILES_TRAP 0x4044
#define U_TILES_FIRE 0x4045
#define U_TILES_FOLIAGE 0x4046
#define U_TILES_AMULET 0x4047
#define U_TILES_SCROLL 0x4048
#define U_TILES_RING 0x4049
#define U_TILES_WEAPON 0x404A
#define U_TILES_GEM 0x404B
#define U_TILES_TOTEM 0x404C
#define U_TILES_GOOD_MAGIC 0x404D
#define U_TILES_BAD_MAGIC 0x404E
#define U_TILES_DOORWAY 0x404F
#define U_TILES_CHARM 0x4050
#define U_TILES_WALL_TOP 0x4051
#define U_TILES_DAR_PRIESTESS 0x4052
#define U_TILES_DAR_BATTLEMAGE 0x4053
#define U_TILES_GOBLIN_MAGIC 0x4054
#define U_TILES_GOBLIN_CHIEFTAN 0x4055
#define U_TILES_OGRE_MAGIC 0x4056
#define U_TILES_GUARDIAN 0x4057
#define U_TILES_WINGED_GUARDIAN 0x4058
#define U_TILES_EGG 0x4059
#define U_TILES_WARDEN 0x405A
#define U_TILES_DEWAR 0x405B
#define U_TILES_ANCIENT_SPIRIT 0x405C
#define U_TILES_LEVER 0x405D
#define U_TILES_LEVER_PULLED 0x405E
#define U_TILES_BLOODWORT_STALK 0x405F
#define U_TILES_FLOOR_ALT 0x4060
#define U_TILES_UNICORN 0x4061
#define U_TILES_TURRET 0x4062
#define U_TILES_WAND 0x4063
#define U_TILES_GRANITE 0x4064
#define U_TILES_CARPET 0x4065
#define U_TILES_CLOSED_IRON_DOOR 0x4066
#define U_TILES_OPEN_IRON_DOOR 0x4067
#define U_TILES_TORCH 0x4068
#define U_TILES_CRYSTAL 0x4069
#define U_TILES_PORTCULLIS 0x406A
#define U_TILES_BARRICADE 0x406B
#define U_TILES_STATUE 0x406C
#define U_TILES_CRACKED_STATUE 0x406D
#define U_TILES_CLOSED_CAGE 0x406E
#define U_TILES_OPEN_CAGE 0x406F
#define U_TILES_PEDESTAL 0x4070
#define U_TILES_CLOSED_COFFIN 0x4071
#define U_TILES_OPEN_COFFIN 0x4072
#define U_TILES_MAGIC_GLYPH 0x4073
#define U_TILES_BRIDGE 0x4074
#define U_TILES_BONES 0x4075
#define U_TILES_ELECTRIC_CRYSTAL 0x4076
#define U_TILES_ASHES 0x4077
#define U_TILES_BEDROLL 0x4078
#define U_TILES_BLOODWORT_POD 0x4079
#define U_TILES_VINE 0x407A
#define U_TILES_NET 0x407B
#define U_TILES_LICHEN 0x407C
#define U_TILES_PIPES 0x407D
#define U_TILES_SAC_ALTAR 0x407E
#define U_TILES_ORB_ALTAR 0x407F

#define maybeTiled(glyph, plain) ((graphicsMode == TILES_GRAPHICS) ||                                       \
                                ((graphicsMode == HYBRID_GRAPHICS) && isEnvironmentGlyph(glyph))            \
                                    ? ((glyph-130)+0x4000) : plain)

// #define U_UP_TRIANGLE  0x2206
// #define U_DOWN_TRIANGLE  0x2207
// #define U_THETA  0x03B8
// #define U_LAMDA  0x03BB
// #define U_KOPPA  0x03DE
// #define U_LOZENGE  0x29EB
// #define U_CROSS_PRODUCT  0x2A2F

struct brogueConsole {
    /*
    The platform entrypoint, called by the main function. Should initialize
    and then call rogueMain.
    */
    void (*gameLoop)();

    /*
    Pause the game, returning a boolean specifying whether an input event
    is available for receiving with nextKeyOrMouseEvent.
    */
    boolean (*pauseForMilliseconds)(short milliseconds);

    /*
    Block until an event is available and then update returnEvent with
    its details. textInput is true iff a text-entry box is active. See
    sdl2-platform.c for the boilerplate for colorsDance.
    */
    void (*nextKeyOrMouseEvent)(rogueEvent *returnEvent, boolean textInput, boolean colorsDance);

    /*
    Draw a character at a location with a specific color.
    */
    void (*plotChar)(
        enum displayGlyph inputChar,
        short x, short y,
        short foreRed, short foreGreen, short foreBlue,
        short backRed, short backGreen, short backBlue
    );

    void (*remap)(const char *, const char *);

    /*
    Returns whether a keyboard modifier is active -- 0 for Shift, 1 for Ctrl.
    */
    boolean (*modifierHeld)(int modifier);

    /*
    Optional. Notifies the platform code of an event during the game - e.g. victory
    */
    void (*notifyEvent)(short eventId, int data1, int data2, const char *str1, const char *str2);

    /*
    Optional. Take a screenshot in current working directory
    */
    boolean (*takeScreenshot)();

    /*
    Optional. Enables or disables graphical tiles, returning the new state. This
    is called when the user changes the option in-game. It is also called at the
    very start of the program, even before .gameLoop, to set the initial value.
    */
    enum graphicsModes (*setGraphicsMode)(enum graphicsModes mode);
};

// defined in platform
void loadKeymap();
void dumpScores();
unsigned int glyphToUnicode(enum displayGlyph glyph);
boolean isEnvironmentGlyph(enum displayGlyph glyph);

#ifdef BROGUE_SDL
extern struct brogueConsole sdlConsole;
extern int windowWidth;
extern int windowHeight;
extern boolean fullScreen;
extern boolean softwareRendering;
#endif

#ifdef BROGUE_CURSES
extern struct brogueConsole cursesConsole;
#endif

#ifdef BROGUE_WEB
extern struct brogueConsole webConsole;
#endif

extern struct brogueConsole currentConsole;
extern boolean noMenu;
extern char dataDirectory[];

// defined in brogue
extern playerCharacter rogue;

