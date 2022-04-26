/*
 *  platformdependent.c
 *  Brogue
 *
 *  Created by Brian Walker on 4/13/10.
 *  Copyright 2010. All rights reserved.
 *
 *  This file is part of Brogue.
 *
 *  Brogue is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Brogue is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Brogue.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "platform.h"

typedef struct brogueScoreEntry {
    long int score;
    long int dateNumber; // in seconds
    char dateText[COLS]; // in the form mm/dd/yy
    char description[COLS];
} brogueScoreEntry;

brogueScoreEntry scoreBuffer[HIGH_SCORES_COUNT];

unsigned int glyphToUnicode(enum displayGlyph glyph) {
    if (glyph < 128) return glyph;

    // environment glyphs should show up in TILES or HYBRID, not TEXT
    // monster glyphs should only show up in TILES, not TEXT or HYBRID
    // items glyphs show up only in TILES, per isEnvironment()
    // maybeTiled() is #defined in platform.h

    
    switch (glyph) {
        case G_UP_ARROW: return U_UP_ARROW;
        case G_DOWN_ARROW: return U_DOWN_ARROW;
        case G_POTION: return maybeTiled(G_POTION , '!');
        case G_GRASS: return maybeTiled(G_GRASS , '"');
        case G_WALL: return maybeTiled(G_WALL , '#');
        case G_DEMON: return maybeTiled(G_DEMON , '&');
        case G_OPEN_DOOR:  return maybeTiled(G_OPEN_DOOR , '\'');
        case G_GOLD:  return maybeTiled(G_GOLD , '*');
        case G_CLOSED_DOOR: return maybeTiled(G_CLOSED_DOOR , '+');
        case G_RUBBLE: return maybeTiled(G_RUBBLE , ',');
        case G_KEY: return maybeTiled(G_KEY , '-');
        case G_BOG: return maybeTiled(G_BOG , '~');
        case G_CHAIN_TOP_LEFT: return maybeTiled(G_CHAIN_TOP_LEFT , '\\');
        case G_CHAIN_BOTTOM_RIGHT: return maybeTiled(G_CHAIN_BOTTOM_RIGHT , '\\');
        case G_CHAIN_TOP_RIGHT: return maybeTiled(G_CHAIN_TOP_RIGHT , '/');
        case G_CHAIN_BOTTOM_LEFT: return maybeTiled(G_CHAIN_BOTTOM_LEFT , '/');
        case G_CHAIN_TOP: return maybeTiled(G_CHAIN_TOP , '|');
        case G_CHAIN_BOTTOM: return maybeTiled(G_CHAIN_BOTTOM , '|');
        case G_CHAIN_LEFT: return maybeTiled(G_CHAIN_LEFT , '-');
        case G_CHAIN_RIGHT: return maybeTiled(G_CHAIN_RIGHT , '-');
        case G_FOOD:  return maybeTiled(G_FOOD , ';');
        case G_UP_STAIRS: return maybeTiled(G_UP_STAIRS , '<');
        case G_VENT: return maybeTiled(G_VENT , '=');
        case G_DOWN_STAIRS:  return maybeTiled(G_DOWN_STAIRS , '>');
        case G_PLAYER: return maybeTiled(G_PLAYER , '@');
        case G_BOG_MONSTER: return  maybeTiled(G_BOG_MONSTER , 'B');
        case G_CENTAUR: return maybeTiled(G_CENTAUR,  'C');
        case G_DRAGON: return maybeTiled(G_DRAGON,  'D');
        case G_FLAMEDANCER: return maybeTiled(G_FLAMEDANCER,  'F');
        case G_GOLEM: return maybeTiled(G_GOLEM,  'G');
        case G_TENTACLE_HORROR: return maybeTiled(G_TENTACLE_HORROR,  'H');
        case G_IFRIT: return maybeTiled(G_IFRIT,  'I');
        case G_JELLY: return maybeTiled(G_JELLY,  'J');
        case G_KRAKEN: return maybeTiled(G_KRAKEN,  'K');
        case G_LICH: return maybeTiled(G_LICH,  'L');
        case G_NAGA: return maybeTiled(G_NAGA,  'N');
        case G_OGRE: return maybeTiled(G_OGRE,  'O');
        case G_PHANTOM: return maybeTiled(G_PHANTOM,  'P');
        case G_REVENANT: return maybeTiled(G_REVENANT,  'R');
        case G_SALAMANDER: return maybeTiled(G_SALAMANDER,  'S');
        case G_TROLL: return maybeTiled(G_TROLL,  'T');
        case G_UNDERWORM: return maybeTiled(G_UNDERWORM,  'U');
        case G_VAMPIRE: return maybeTiled(G_VAMPIRE,  'V');
        case G_WRAITH: return maybeTiled(G_WRAITH,  'W');
        case G_ZOMBIE: return maybeTiled(G_ZOMBIE,  'Z');
        case G_ARMOR:  return maybeTiled(G_ARMOR , '[');
        case G_STAFF:  return maybeTiled(G_STAFF , '/');
        case G_WEB:  return maybeTiled(G_WEB , ':');
        case G_MOUND: return maybeTiled(G_MOUND,  'a');
        case G_BLOAT: return maybeTiled(G_BLOAT,  'b');
        case G_CENTIPEDE: return maybeTiled(G_CENTIPEDE,  'c');
        case G_DAR_BLADEMASTER: return maybeTiled(G_DAR_BLADEMASTER,  'd');
        case G_EEL: return maybeTiled(G_EEL,  'e');
        case G_FURY: return maybeTiled(G_FURY,  'f');
        case G_GOBLIN: return maybeTiled(G_GOBLIN,  'g');
        case G_IMP: return maybeTiled(G_IMP,  'i');
        case G_JACKAL: return maybeTiled(G_JACKAL,  'j');
        case G_KOBOLD: return maybeTiled(G_KOBOLD,  'k');
        case G_MONKEY: return maybeTiled(G_MONKEY,  'm');
        case G_PIXIE: return maybeTiled(G_PIXIE,  'p');
        case G_RAT:  return  maybeTiled(G_RAT,  'r');
        case G_SPIDER: return maybeTiled(G_SPIDER,  's');
        case G_TOAD: return maybeTiled(G_TOAD,  't');
        case G_BAT: return maybeTiled(G_BAT,  'v');
        case G_WISP: return maybeTiled(G_WISP,  'w');
        case G_PHOENIX: return maybeTiled(G_PHOENIX,  'P');
        case G_ALTAR: return maybeTiled(G_ALTAR , '|');
        case G_LIQUID: return maybeTiled(G_LIQUID , '~');
        case G_FLOOR:  return maybeTiled(G_FLOOR, U_MIDDLE_DOT);
        case G_CHASM:  return maybeTiled(G_CHASM, U_FOUR_DOTS);
        case G_TRAP:  return maybeTiled(G_TRAP, U_DIAMOND);
        case G_FIRE:  return maybeTiled(G_FIRE, U_FLIPPED_V);
        case G_FOLIAGE:  return maybeTiled(G_FOLIAGE, U_ARIES);
        case G_AMULET:  return maybeTiled(G_AMULET, U_ANKH);
        case G_SCROLL:  return maybeTiled(G_SCROLL, U_MUSIC_NOTE);
        case G_RING:  return maybeTiled(G_RING, U_CIRCLE);
        case G_WEAPON:  return maybeTiled(G_WEAPON, U_UP_ARROW);
        case G_GEM:  return maybeTiled(G_GEM, U_FILLED_CIRCLE);
        case G_TOTEM:  return maybeTiled(G_TOTEM, U_NEUTER);
        case G_GOOD_MAGIC:  return maybeTiled(G_GOOD_MAGIC, U_CIRCLE_BARS);
        case G_BAD_MAGIC:  return maybeTiled(G_BAD_MAGIC, U_FILLED_CIRCLE_BARS);
        case G_DOORWAY:  return maybeTiled(G_DOORWAY, U_OMEGA);
        case G_CHARM:  return maybeTiled(G_CHARM, U_LIGHTNING_BOLT);
        case G_WALL_TOP:  return maybeTiled(G_WALL_TOP, '#');
        case G_DAR_PRIESTESS: return maybeTiled(G_DAR_PRIESTESS,  'd');
        case G_DAR_BATTLEMAGE: return maybeTiled(G_DAR_BATTLEMAGE,  'd');
        case G_GOBLIN_MAGIC: return maybeTiled(G_GOBLIN_MAGIC,  'g');
        case G_GOBLIN_CHIEFTAN: return maybeTiled(G_GOBLIN_CHIEFTAN,  'g');
        case G_OGRE_MAGIC: return maybeTiled(G_OGRE_MAGIC,  'O');
        case G_GUARDIAN: return maybeTiled(G_GUARDIAN, U_ESZETT);
        case G_WINGED_GUARDIAN: return maybeTiled(G_WINGED_GUARDIAN, U_ESZETT);
        case G_EGG: return maybeTiled(G_EGG, U_FILLED_CIRCLE);
        case G_WARDEN: return maybeTiled(G_WARDEN,  'Y');
        case G_DEWAR:  return maybeTiled(G_DEWAR , '&');
        case G_ANCIENT_SPIRIT: return maybeTiled(G_ANCIENT_SPIRIT,  'M');
        case G_LEVER:  return maybeTiled(G_LEVER , '/');
        case G_LEVER_PULLED:  return maybeTiled(G_LEVER_PULLED , '\\');
        case G_BLOODWORT_STALK:  return maybeTiled(G_BLOODWORT_STALK, U_ARIES);
        case G_FLOOR_ALT:  return maybeTiled(G_FLOOR_ALT, U_MIDDLE_DOT);
        case G_UNICORN: return maybeTiled(G_UNICORN, U_U_ACUTE);
        case G_TURRET:  return maybeTiled(G_TURRET, U_FILLED_CIRCLE);
        case G_WAND:  return maybeTiled(G_WAND , '~');
        case G_GRANITE:  return maybeTiled(G_GRANITE,  '#');
        case G_CARPET:  return maybeTiled(G_CARPET, U_MIDDLE_DOT);
        case G_CLOSED_IRON_DOOR: return maybeTiled(G_CLOSED_IRON_DOOR , '+');
        case G_OPEN_IRON_DOOR: return maybeTiled(G_OPEN_IRON_DOOR , '\'');
        case G_TORCH:  return maybeTiled(G_TORCH , '#');
        case G_CRYSTAL:  return maybeTiled(G_CRYSTAL , '#');
        case G_PORTCULLIS:  return maybeTiled(G_PORTCULLIS , '#');
        case G_BARRICADE:  return maybeTiled(G_BARRICADE , '#');
        case G_STATUE:  return maybeTiled(G_STATUE, U_ESZETT);
        case G_CRACKED_STATUE:  return maybeTiled(G_CRACKED_STATUE,  U_ESZETT);
        case G_CLOSED_CAGE:  return maybeTiled(G_CLOSED_CAGE , '#');
        case G_OPEN_CAGE:  return maybeTiled(G_OPEN_CAGE , '|');
        case G_PEDESTAL:  return maybeTiled(G_PEDESTAL , '|');
        case G_CLOSED_COFFIN:  return maybeTiled(G_CLOSED_COFFIN , '-');
        case G_OPEN_COFFIN:  return maybeTiled(G_OPEN_COFFIN , '-');
        case G_MAGIC_GLYPH:  return maybeTiled(G_MAGIC_GLYPH,  U_FOUR_DOTS);
        case G_BRIDGE:  return maybeTiled(G_BRIDGE , '=');
        case G_BONES:  return maybeTiled(G_BONES , ',');
        case G_ELECTRIC_CRYSTAL:  return maybeTiled(G_ELECTRIC_CRYSTAL, U_CURRENCY);
        case G_ASHES:  return maybeTiled(G_ASHES , '\'');
        case G_BEDROLL:  return maybeTiled(G_BEDROLL , '=');
        case G_BLOODWORT_POD:  return maybeTiled(G_BLOODWORT_POD,  '*');
        case G_VINE:  return maybeTiled(G_VINE , ':');
        case G_NET:  return maybeTiled(G_NET , ':');
        case G_LICHEN:  return maybeTiled(G_LICHEN , '"');
        case G_PIPES:  return maybeTiled(G_PIPES , '+');
        case G_SAC_ALTAR:  return maybeTiled(G_SAC_ALTAR , '|');
        case G_ORB_ALTAR:  return maybeTiled(G_ORB_ALTAR , '|');
                                                 
        default:
            brogueAssert(false);
            return '?';
    }
}

/*
Tells if a glyph represents part of the environment (true) or an item or creature (false).
*/
boolean isEnvironmentGlyph(enum displayGlyph glyph) {
    switch (glyph) {
        // items
        case G_AMULET: case G_ARMOR: case G_BEDROLL: case G_CHARM:
        case G_DEWAR: case G_EGG: case G_FOOD: case G_GEM: case G_BLOODWORT_POD:
        case G_GOLD: case G_KEY: case G_POTION: case G_RING:
        case G_SCROLL: case G_STAFF: case G_WAND: case G_WEAPON:
            return false;

        // creatures
        case G_ANCIENT_SPIRIT: case G_BAT: case G_BLOAT: case G_BOG_MONSTER:
        case G_CENTAUR: case G_CENTIPEDE: case G_DAR_BATTLEMAGE: case G_DAR_BLADEMASTER:
        case G_DAR_PRIESTESS: case G_DEMON: case G_DRAGON: case G_EEL:
        case G_FLAMEDANCER: case G_FURY: case G_GOBLIN: case G_GOBLIN_CHIEFTAN:
        case G_GOBLIN_MAGIC: case G_GOLEM: case G_GUARDIAN: case G_IFRIT:
        case G_IMP: case G_JACKAL: case G_JELLY: case G_KOBOLD:
        case G_KRAKEN: case G_LICH: case G_MONKEY: case G_MOUND:
        case G_NAGA: case G_OGRE: case G_OGRE_MAGIC: case G_PHANTOM:
        case G_PHOENIX: case G_PIXIE: case G_PLAYER: case G_RAT:
        case G_REVENANT: case G_SALAMANDER: case G_SPIDER: case G_TENTACLE_HORROR:
        case G_TOAD: case G_TROLL: case G_UNDERWORM: case G_UNICORN:
        case G_VAMPIRE: case G_WARDEN: case G_WINGED_GUARDIAN: case G_WISP:
        case G_WRAITH: case G_ZOMBIE:
            return false;

        // everything else is considered part of the environment
        default:
            return true;
    }
}

void pausingTimerStartsNow() {

}

void notifyEvent(short eventId, int data1, int data2, const char *str1, const char *str2) {
    if (currentConsole.notifyEvent) {
        currentConsole.notifyEvent(eventId, data1, data2, str1, str2);
    }
}

boolean takeScreenshot() {
    if (currentConsole.takeScreenshot) {
        return currentConsole.takeScreenshot();
    } else {
        return false;
    }
}

enum graphicsModes setGraphicsMode(enum graphicsModes mode) {
    if (currentConsole.setGraphicsMode) {
        return currentConsole.setGraphicsMode(mode);
    } else {
        return TEXT_GRAPHICS;
    }
}

// creates an empty high scores file
void initScores() {
    short i;
    FILE *scoresFile;

    scoresFile = fopen("BrogueHighScores.txt", "w");
    for (i=0; i<HIGH_SCORES_COUNT; i++) {
        fprintf(scoresFile, "%li\t%li\t%s", (long) 0, (long) 0, "(empty entry)\n");
    }
    fclose(scoresFile);
}

// sorts the entries of the scoreBuffer global variable by score in descending order;
// returns the sorted line number of the most recent entry
short sortScoreBuffer() {
    short i, j, highestUnsortedLine, mostRecentSortedLine = 0;
    long highestUnsortedScore, mostRecentDate;
    brogueScoreEntry sortedScoreBuffer[HIGH_SCORES_COUNT];
    boolean lineSorted[HIGH_SCORES_COUNT];

    mostRecentDate = 0;

    for (i=0; i<HIGH_SCORES_COUNT; i++) {
        lineSorted[i] = false;
    }

    for (i=0; i<HIGH_SCORES_COUNT; i++) {
        highestUnsortedLine = 0;
        highestUnsortedScore = 0;
        for (j=0; j<HIGH_SCORES_COUNT; j++) {
            if (!lineSorted[j] && scoreBuffer[j].score >= highestUnsortedScore) {
                highestUnsortedLine = j;
                highestUnsortedScore = scoreBuffer[j].score;
            }
        }
        sortedScoreBuffer[i] = scoreBuffer[highestUnsortedLine];
        lineSorted[highestUnsortedLine] = true;
    }

    // copy the sorted list back into scoreBuffer, remember the most recent entry
    for (i=0; i<HIGH_SCORES_COUNT; i++) {
        scoreBuffer[i] = sortedScoreBuffer[i];
        if (scoreBuffer[i].dateNumber > mostRecentDate) {
            mostRecentDate = scoreBuffer[i].dateNumber;
            mostRecentSortedLine = i;
        }
    }
    return mostRecentSortedLine;
}

// loads the BrogueHighScores.txt file into the scoreBuffer global variable
// score file format is: score, tab, date in seconds, tab, description, newline.
short loadScoreBuffer() {
    short i;
    FILE *scoresFile;
    time_t rawtime;
    struct tm * timeinfo;

    scoresFile = fopen("BrogueHighScores.txt", "r");

    if (scoresFile == NULL) {
        initScores();
        scoresFile = fopen("BrogueHighScores.txt", "r");
    }

    for (i=0; i<HIGH_SCORES_COUNT; i++) {
        // load score and also the date in seconds
        fscanf(scoresFile, "%li\t%li\t", &(scoreBuffer[i].score), &(scoreBuffer[i].dateNumber));

        // load description
        fgets(scoreBuffer[i].description, COLS, scoresFile);
        // strip the newline off the end
        scoreBuffer[i].description[strlen(scoreBuffer[i].description) - 1] = '\0';

        // convert date to DATE_FORMAT
        rawtime = (time_t) scoreBuffer[i].dateNumber;
        timeinfo = localtime(&rawtime);
        strftime(scoreBuffer[i].dateText, DCOLS, DATE_FORMAT, timeinfo);
    }
    fclose(scoresFile);
    return sortScoreBuffer();
}

void loadKeymap() {
    int i;
    FILE *f;
    char buffer[512];

    f = fopen("keymap.txt", "r");

    if (f != NULL) {
        while (fgets(buffer, 512, f) != NULL) {
            // split it in two (destructively)
            int mode = 1;
            char *input_name = NULL, *output_name = NULL;
            for (i = 0; buffer[i]; i++) {
                if (isspace(buffer[i])) {
                    buffer[i] = '\0';
                    mode = 1;
                } else {
                    if (mode) {
                        if (input_name == NULL) input_name = buffer + i;
                        else if (output_name == NULL) output_name = buffer + i;
                    }
                    mode = 0;
                }
            }
            if (input_name != NULL && output_name != NULL) {
                if (input_name[0] == '#') continue; // must be a comment

                currentConsole.remap(input_name, output_name);
            }
        }
        fclose(f);
    }
}


// saves the scoreBuffer global variable into the BrogueHighScores.txt file,
// thus overwriting whatever is already there.
// The numerical version of the date is what gets saved; the "mm/dd/yy" version is ignored.
// Does NOT do any sorting.
void saveScoreBuffer() {
    short i;
    FILE *scoresFile;

    scoresFile = fopen("BrogueHighScores.txt", "w");

    for (i=0; i<HIGH_SCORES_COUNT; i++) {
        // save the entry
        fprintf(scoresFile, "%li\t%li\t%s\n", scoreBuffer[i].score, scoreBuffer[i].dateNumber, scoreBuffer[i].description);
    }

    fclose(scoresFile);
}

void dumpScores() {
    int i;

    rogueHighScoresEntry list[HIGH_SCORES_COUNT];
    getHighScoresList(list);

    for (i = 0; i < HIGH_SCORES_COUNT; i++) {
        if (list[i].score > 0) {
            printf("%d\t%s\t%s\n", (int) list[i].score, list[i].date, list[i].description);
        }
    }
}

// start of file listing

struct filelist {
    fileEntry *files;
    char *names;

    int nfiles, maxfiles;
    int nextname, maxname;
};

struct filelist *newFilelist() {
    struct filelist *list = malloc(sizeof(*list));

    list->nfiles = 0;
    list->nextname = 0;
    list->maxfiles = 64;
    list->maxname = list->maxfiles * 64;

    list->files = malloc(sizeof(fileEntry) * list->maxfiles);
    list->names = malloc(list->maxname);

    return list;
}

void freeFilelist(struct filelist *list) {
    //if (list->names != NULL) free(list->names);
    //if (list->files != NULL) free(list->files);
    free(list);
}

fileEntry *commitFilelist(struct filelist *list, char **namebuffer) {
    int i;
    /*fileEntry *files = malloc(list->nfiles * sizeof(fileEntry) + list->nextname); // enough space for all the names and all the files

    if (files != NULL) {
        char *names = (char *) (files + list->nfiles);

        for (i=0; i < list->nfiles; i++) {
            files[i] = list->files[i];
            files[i].path = names + (files[i].path - (char *) NULL);
        }

        memcpy(names, list->names, list->nextname);
    }
    */
    for (i=0; i < list->nfiles; i++) {
        list->files[i].path = list->names + (list->files[i].path - (char *) NULL);
    }
    *namebuffer = list->names;

    return list->files;
}

boolean tryParseUint64(char *str, uint64_t *num) {
    unsigned long long n;
    char buf[100];
    if (strlen(str)                 // we need some input
        && sscanf(str, "%llu", &n)  // try to convert to number
        && sprintf(buf, "%llu", n)  // convert back to string
        && !strcmp(buf, str)) {     // compare (we need them equal)
        *num = (uint64_t)n;
        return true; // success
    } else {
        return false; // input was too large or not a decimal number
    }
}

// end of file listing

void initializeLaunchArguments(enum NGCommands *command, char *path, uint64_t *seed) {
    // we've actually already done this at this point, except for the seed.
}

boolean isApplicationActive(void) {
    // FIXME: finish
    return true;
}
