/*
 * battery.h
 *
 *  Created on: September 19, 2023
 *      Author: patches
 */

#ifndef LIB_HWINIT_BATT_H_
#define LIB_HWINIT_BATT_H_

// Sets 0 if the battery is good, or 1 if the battery LBO signal was detected
// from the power management IC.
extern unsigned int BATTERY_LOW;

// This function once called checks the status of the LBO flag and sets BATTERY_LOW.
// If the battery is found to be low it also calls ALERT_blinkBatteryLED();
void BATTERY_checkForSigLBO(void);

#endif /* LIB_HWINIT_BATT_H_ */
