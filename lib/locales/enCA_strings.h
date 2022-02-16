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

extern const char* LSTRING_HUNGER;          //Hunger, as in the gameplay mechanic.
extern const char* LSTRING_FUN;             //Fun, as in the gameplay mechanic
extern const char* LSTRING_DISCIPLINE;      //Discipline, as in the gameplay mechanic
extern const char* LSTRING_WEIGHT;          //Weight, as in the invisible state based on the health byte
extern const char* LSTRING_AGE;             //Age, as in the age in days of the current living pet.
extern const char* LSTRING_SPECIAL;         //Localization of the "Special" Status Phrase
extern const char* LSTRING_STATUS_HEADER;   //Title for the Status Menu screne
extern const char* LSTRING_DEBUG_HEADER;    //Title for the Debug Header
extern const char* LSTRING_CALMENU_HEADER;  //"Please enter the time:" or localized.
extern const char* LSTRING_FEEDING_HEADER;
extern const char* LSTRING_FOOD_HEADER;
extern const char* LSTRING_SNACKS_HEADER;
extern const char* LSTRING_SOUND_CHECK;
extern const char* LSTRING_STAGE_SELECT;

extern const char* LARRAY_DEBUG_OPTS[];     //Option Names for the Debug Menu

extern const char* LARRAY_DAYS_OF_WEEK[];   //Three-letter abbreviations of the names of the days of the week

extern const char* LARRAY_EVO_STAGE_NAMES[]; // Names of each stage of PETI evolution
extern const char* LARRAY_FOOD_FOOD_NAMES[]; // Names of all the foods in the food menu
extern const char* LARRAY_FOOD_SNACK_NAMES[]; // Name of all the snacks in the snack menu.
extern const char* LARRAY_FOOD_TOP_MENU[];
extern const char* LARRAY_SOUNDCHECK_OPTS[];


#endif /* LIB_LOCALES_ENCA_STRINGS_H_ */
