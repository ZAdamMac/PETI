/*
 * enCA_strings.h
 *
 *  Created on: May 11, 2021
 *      Author: patches
 */

// These locale files are intended for use anywhere static strings will be used, to provide for
// more expedient localization, perhaps by replacing the values in such a file or adding a switch
// header file (similar to what TI have done with the main MSP430.h file) to control which
// locale gets compiled in. Localization is not a major focus of the project, but it is an
// important accessibility and openness feature and should be supported as efficiently as is practical.

// If this document gets of any real size, since all of these values are /static/, they should have the
// #PRAGMA PERSISTENT declaration to move them into FRAM.

// At the time of this note (Jul 2021) there is not a padding function available so these strings are
// prepadded for fixed widths as needed.

#ifndef LIB_LOCALES_ENCA_STRINGS_H_
#define LIB_LOCALES_ENCA_STRINGS_H_

extern const char* LSTRING_HUNGER = "Hunger";
extern const char* LSTRING_FUN = "Fun";
extern const char* LSTRING_DISCIPLINE = "Discipline";
extern const char* LSTRING_WEIGHT = "Weight";
extern const char* LSTRING_AGE = "Age";
extern const char* LSTRING_SPECIAL = "Special";
extern const char* LSTRING_STATUS_HEADER = "   STATS PAGE   ";
extern const char* LSTRING_DEBUG_HEADER = "   DEBUG MENU   ";
extern const char* LSTRING_SOUND_CHECK = "   SOUND CHECK";
extern const char* LSTRING_CALMENU_HEADER = "Enter the time: ";
extern const char* LSTRING_FEEDING_HEADER = "  FEEDING MENU  ";
extern const char* LSTRING_FOOD_HEADER = "   FOOD  MENU   ";
extern const char* LSTRING_SNACKS_HEADER = "  SNACKS  MENU  ";
extern const char* LSTRING_MINIGAME_HEADER = "     GAMES      ";
extern const char* LSTRING_STAGE_SELECT = "  STAGE SELECT  ";
extern const char* LSTRING_ENTROPY_INFO = "   RNG STATUS   ";
extern const char* LSTRING_INITIAL_SEED = "  INITIAL SEED  ";
extern const char* LSTRING_CURRENT_RNG = "CURRENT RNG CARD";
extern const char* LSTRING_MINIGAME_PICK = " Make A Choice! ";
extern const char* LSTRING_YES = "    YES ";
extern const char* LSTRING_NO = "    NO ";
extern const char* LSTRING_WARNING = "    WARNING    ";


extern const char* LARRAY_DEBUG_OPTS[] = {
                                        "Input Test      ",
                                        "Audio Check     ",
                                        "Fill Hunger Bar ",
                                        "Drain Hunger Bar",
                                        "Fill Fun Bar    ",
                                        "Drain Fun Bar   ",
                                        "Stage Select    ",
                                        "Entropy Check   ",
                                        "Sleep the LCD   ",
                                        "Age +1          ",
                                        "Force Evolution "
};

extern const char* LARRAY_DAYS_OF_WEEK[] = {
                      "SUN",
                      "MON",
                      "TUE",
                      "WED",
                      "THU",
                      "FRI",
                      "SAT"
};

extern const char* LARRAY_EVO_STAGE_NAMES[] = {
                    "      Egg       ",
                    "      Baby      ",
                    "   Hungryboi    ",
                    "   SicklySlim   ",
                    "   Jellyfloat   ",
                    "    Bughound    ",
                    "   Zazenkuchi   ",
                    "   Pointyboi    ",
                    "   Skullcrab    ",
                    "    Darumite    ",
                    "     Squid      ",
                    "   Cthulhorse   ",
                    "  Ham-Monster   ",
                    "    Nautilus    ",
                    "  Axe-a-lot'l   ",
                    "    Geo-Baby    ",
                    "      Spike     ",
                    "      Cubie     ",
                    "    Cluster     ",
                    "   Shine-Brite  "
};

extern const char* LARRAY_FOOD_TOP_MENU[] = {
                              "Food            ",
                              "Snacks          "
};

extern const char* LARRAY_FOOD_FOOD_NAMES[] = {
                              "Rice Bowl       "
};

extern const char* LARRAY_FOOD_SNACK_NAMES[] = {
                              "Lollypop        "
};

extern const char* LARRAY_SOUNDCHECK_OPTS[] = {
                              "SHORT BEEP      ",
                              "LONG BEEP       "
};

extern const char* LARRAY_MINIGAME_TITLES[] = {
    "Rock Paper Game"
};

extern const char* LARRAY_RESET_WARNING[] = {
    "Really clear the",
    "saved game data?"
};

#endif /* LIB_LOCALES_ENCA_STRINGS_H_ */
