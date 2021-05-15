//***************************************************************************************
//  PETI FOSS Virtual Pet - Scene Management Code
//
//  Beating heart of the Scenes Management System - see the wiki.
//  Used by PETI to manage which scene is active and code its relevant call.
//
//
//
//  Zac Adam-MacEwen (Kensho Security Labs)
//  May 2021
//***************************************************************************************

#include <msp430.h>
#include "driverlib.h"
#include "scenes_manager.h"
#include "boot_splash.h"
#include "demo_mode.h"
#include "calendar_menu.h"
#include "main.h"
#include "lib/display/display.h"

volatile unsigned int SCENE_FRAME = 0x00;   // The current translational frame, used for scenes that just play out a defined animation.
volatile unsigned int SCENE_TRANS_SECONDS;  // The time at which a time-based trigger should activate in seconds
volatile unsigned int SCENE_TRANS_MINUTES;  // the time at which a time-based transition trigger should activate in minutes
volatile unsigned int PREVIOUS_SCENE = 0x00;

// Basically, all of scene management is one giant switch statement to select the scene and determine what to print to screen.
// When adding new scenes make sure that the SCENEADDR is added to the header file and that a case is added to this function to handle it.
void SCENE_updateDisplay(void){
    if (SCENE_ACT != PREVIOUS_SCENE){
        LCDClearDisplay();
        FORCE_REFRESH = true;
    }
    PREVIOUS_SCENE = SCENE_ACT;
    switch(SCENE_ACT){
        case SCENEADDR_boot_splash :  //Triggers the initial boot splash animation, defined in boot_splash.h/c
            SCENE_boot_splash();
            break;
        case SCENEADDR_demo_mode :  //Triggers the demonstorator/helloworld mode defined in demo.h/c
            SCENE_demo_mode();
            break;
        case SCENEADDR_main_game :  //Yooooo!
            SCENE_main_game();
            break;
        case SCENEADDR_calendar_menu: // This is the calendar menu screen
            SCENE_CalendarMenu();
            break;
    }
}

// we can set a delay of up to 60 minutes using this function.
// then evaluate if the condition is met using SCENE_checkTransitionTimeCondition();
void SCENE_setTransitionTimeCondition(unsigned int delay_seconds){
    volatile unsigned int delta_seconds, delta_minutes, current_seconds, current_minutes;
    delta_seconds = delay_seconds % 60;
    delta_minutes = delay_seconds / 60;

    volatile Calendar currentTime = RTC_C_getCalendarTime(RTC_C_BASE);
    current_seconds = bcd_to_dec(currentTime.Seconds);
    current_minutes = bcd_to_dec(currentTime.Minutes); // Adjust for BCD Time
    SCENE_TRANS_SECONDS = (current_seconds + delta_seconds) % 60;
    SCENE_TRANS_MINUTES = (current_minutes + delta_minutes) % 60;
}

// Interrogate the RTC and determine if the delay set by SCENE_setTransitionTimeCondition has passed.
// Principle use of this workflow would be to evaluate if it is time to leave the current scene.
// Other mechanisms exist to determine if (e.g.) the screen is due to be rested.
bool SCENE_checkTransitionTimeCondition(){
    volatile unsigned int current_seconds, current_minutes;
    volatile Calendar currentTime = RTC_C_getCalendarTime(RTC_C_BASE);
    current_seconds = bcd_to_dec(currentTime.Seconds);
    current_minutes = bcd_to_dec(currentTime.Minutes); // Adjust for BCD Time

    if (current_seconds >= SCENE_TRANS_SECONDS && current_minutes >= SCENE_TRANS_MINUTES){
        return true;
    }
    else{
        return false;
    }
}
