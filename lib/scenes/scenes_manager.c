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
#include "calendar_menu.h"
#include "main.h"
#include "button_proofer.h"
#include "status_menu.h"
#include "lib/display/display.h"
#include "menu_generator.h"
#include "lib/menus/debug_menu.h"
#include "lib/menus/snacks_menu.h"
#include "lib/menus/food_menu.h"
#include "lib/menus/feeding_menu.h"
#include "lib/menus/sound_test.h"
#include "lib/locales/enCA_strings.h"
#include "food_selector.h"
#include "lib/game/food_data.h"
#include "eating_animation.h"
#include "stage_selector.h"

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
        case SCENEADDR_boot_splash :    //Triggers the initial boot splash animation, defined in boot_splash.h/c
            SCENE_boot_splash();
            break;
        case SCENEADDR_main_game :      //Yooooo!
            SCENE_main_game();
            break;
        case SCENEADDR_calendar_menu:   // This is the calendar menu screen
            SCENE_CalendarMenu();
            break;
        case SCENEADDR_proof_text:      // A simplistic scene used for debugging purposes as a placeholder scene.
            SCENE_button_proofer();     // Displays a message briefly and returns to SCENEADDR_main_game
            break;
        case SCENEADDR_status_menu:     // The pet's health and safety status information display.
            SCENE_status_menu();
            break;
        case SCENEADDR_debug_menu:      // Debug menu
            SCENE_TextMenu(LSTRING_DEBUG_HEADER, LARRAY_DEBUG_OPTS, MENU_DEBUG_functions, MENU_DEBUG_count_options); // Calls the menu generator as needed.
            break;
        case SCENEADDR_feed_menu:      // Top-Level Food Menu
            SCENE_TextMenu(LSTRING_FEEDING_HEADER, LARRAY_FOOD_TOP_MENU, MENU_FEED_functions, MENU_FEED_count_options); // Calls the menu generator as needed.
            break;
        case SCENEADDR_food_menu:      // Food menu
            SCENE_FoodMenu(LSTRING_FOOD_HEADER, LARRAY_FOOD_FOOD_NAMES, FOOD_array_foods, FOOD_count_foods); // Calls the menu generator as needed.
            break;
        case SCENEADDR_snacks_menu:      // SNACKS menu
            SCENE_FoodMenu(LSTRING_SNACKS_HEADER, LARRAY_FOOD_SNACK_NAMES, FOOD_array_snacks, FOOD_count_snacks); // Calls the menu generator as needed.
            break;
        case SCENEADDR_eating: // Monch Monch
            SCENE_eating();
            break;
        case SCENEADDR_sound_test:
            SCENE_TextMenu(LSTRING_SOUND_CHECK, LARRAY_SOUNDCHECK_OPTS, MENU_SOUNDCHECK_functions, MENU_SOUNDCHECK_count_options); // Calls the audio check text menu
            break;
        case SCENEADDR_stage_select:
            SCENE_StageSelect(LSTRING_STAGE_SELECT, LARRAY_EVO_STAGE_NAMES, EVO_metaStruct, EVO_count_stages);
    }
}

// we can set a delay of up to 60 minutes using this function.
// then evaluate if the condition is met using SCENE_checkTransitionTimeCondition();
void SCENE_setTransitionTimeCondition(unsigned int delay_seconds){
    volatile unsigned int delta_seconds, delta_minutes, current_seconds, current_minutes;
    delta_seconds = delay_seconds % 60;
    delta_minutes = delay_seconds / 60;

    volatile Calendar currentTime = RTC_C_getCalendarTime(RTC_C_BASE);
    current_seconds = RTC_C_convertBCDToBinary(RTC_C_BASE, currentTime.Seconds);
    current_minutes = RTC_C_convertBCDToBinary(RTC_C_BASE, currentTime.Minutes); // Adjust for BCD Time
    SCENE_TRANS_SECONDS = (current_seconds + delta_seconds) % 60;
    SCENE_TRANS_MINUTES = (current_minutes + delta_minutes) % 60;
}

// Interrogate the RTC and determine if the delay set by SCENE_setTransitionTimeCondition has passed.
// Principle use of this workflow would be to evaluate if it is time to leave the current scene.
// Other mechanisms exist to determine if (e.g.) the screen is due to be rested.
int SCENE_checkTransitionTimeCondition(){
    volatile unsigned int current_seconds, current_minutes;
    volatile Calendar currentTime = RTC_C_getCalendarTime(RTC_C_BASE);
    current_seconds = RTC_C_convertBCDToBinary(RTC_C_BASE, currentTime.Seconds);
    current_minutes = RTC_C_convertBCDToBinary(RTC_C_BASE, currentTime.Minutes); // Adjust for BCD Time

    if (current_seconds >= SCENE_TRANS_SECONDS && current_minutes >= SCENE_TRANS_MINUTES){
        return true;
    }
    else{
        return false;
    }
}
