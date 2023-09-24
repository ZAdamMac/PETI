//******************************************************************************
//  PETI FOSS Virtual Pet - Battery Status Management Handler
//
//  Code to handle functions related to the hardware-reported battery state
//  and apply various changes when a low signal is found.
//
//  Zac Adam-MacEwen (Arcana Labs)
//  Sept 2023
//******************************************************************************

#include "driverlib.h"
#include <msp430.h>
#include "lib/alerts/blinkenlights.h"
#include "lib/hwinit/hwinit.h"

unsigned int BATTERY_LOW = 0;

// This function once called checks the status of the LBO flag and sets BATTERY_LOW.
// If the battery is found to be low it also calls ALERT_blinkBatteryLED();
void BATTERY_checkForSigLBO (void) {
    unsigned int status;
    status = GPIO_getInputPinValue(SENSOR_BATTERY_PORT, SENSOR_BATTERY_PIN);
    switch (status){
        case GPIO_INPUT_PIN_HIGH:  // According to the TPS60204 design manual, high means the battery is good.
            BATTERY_LOW = 0;
            BLINKENLIGHTS_stopBatteryLED();
            break;
        case GPIO_INPUT_PIN_LOW:
            BLINKENLIGHTS_blinkBatteryLED(BLINKENLIGHTS_SLOW_PULSES);
            BATTERY_LOW = 1;
            break;
    }
}