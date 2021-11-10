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

extern const char* LSTRING_HUNGER;
extern const char* LSTRING_FUN;
extern const char* LSTRING_DISCIPLINE;
extern const char* LSTRING_WEIGHT;
extern const char* LSTRING_AGE;
extern const char* LSTRING_SPECIAL;
extern const char* LSTRING_STATUS_HEADER;
extern const char* LSTRING_DEBUG_HEADER;

extern const char* LARRAY_DEBUG_OPTS[];

extern const char* daysOfWeek[];

extern const char* stageNames[];

#endif /* LIB_LOCALES_ENCA_STRINGS_H_ */
