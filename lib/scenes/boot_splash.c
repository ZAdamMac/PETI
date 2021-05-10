//***************************************************************************************
//  PETI FOSS Virtual Pet - Boot Splash Scene
//
//  Sends the boot splash screen to the display and waits until it is time to exit.
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
#include "lib/display/display.h"
#include "main.h"

#define NEXT_SCENE SCENEADDR_demo_mode  // This is where we goin' when we leave here.

volatile bool SPLASH_SENT = false;

void SCENE_boot_splash(void){
    bool exiting;
    if (SPLASH_SENT){       // We only want to send the splash once or the refresh will look gross
        exiting = SCENE_checkTransitionTimeCondition();
        if (exiting){
            SCENE_ACT = NEXT_SCENE;
        }
    }
    else {
        DisplaySplash();
        printTextSmall(VERSION, 118);
        SPLASH_SENT = true; // We sent it! Let's get out of here.
        SCENE_setTransitionTimeCondition(10); // Wait here for 10 seconds.
    }
}





