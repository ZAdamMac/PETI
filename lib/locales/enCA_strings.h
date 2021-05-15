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

#ifndef LIB_LOCALES_ENCA_STRINGS_H_
#define LIB_LOCALES_ENCA_STRINGS_H_

const char* daysOfWeek[] = {
                      "SUN",
                      "MON",
                      "TUE",
                      "WED",
                      "THU",
                      "FRI",
                      "SAT"
};


#endif /* LIB_LOCALES_ENCA_STRINGS_H_ */
