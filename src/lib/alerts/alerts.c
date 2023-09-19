//***************************************************************************************
//  PETI FOSS Virtual Pet - Alerts API
//
//  Principal Code for PETI to call for alert events
//
//  Check the hardware_design folder for VCS-controlled hardware schematics
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  Added January 2022
//***************************************************************************************

#include "driverlib.h"
#include <msp430.h>
#include "blinkenlights.h"
#include "audio.h"
#include "lib/display/display.h"


void ALERTS_hunger_fun_alert(void){
    if (!BLINKENLIGHTS_ALERT_LED_HOT){
        BLINKENLIGHTS_blinkAlertLED(BLINKENLIGHTS_SLOW_PULSES);
    }
    AUDIO_pulse(AUDIO_LONG_PULSE);
    DISPLAY_wakeLCD();
}
