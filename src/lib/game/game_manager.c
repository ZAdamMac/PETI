//***************************************************************************************
//  PETI FOSS Virtual Pet - Game Management Code
//
//  Principal reachable code for the management of the game state. In chief, exposes two
//  functions; GAME_initStateStruct(); and GAME_evaluateTimedEvents();
//
//  These functions are expanded in this file but expanded subfunctions should live in
//  their own files.
//
//  Zac Adam-MacEwen (Kensho Security Labs)
//  May 2021
//***************************************************************************************


#include <msp430.h>
#include "driverlib.h"
#include "main.h"
#include "game_manager.h"
#include "lib/scenes/scenes_manager.h"
#include "lib/game/evo_data.h"
#include "lib/display/display.h"
#include "lib/alerts/alerts.h"

unsigned int egg_delay_set;
unsigned int egg_delay = 0x01; // The length of the egg state in minutes. Gameplay default is 5, but can be tweaked for testing.
unsigned int needs_evaluation = 0x01; // A flag used to prevent double-dipping on minute checks

GameState StateMachine = {0, 3, 0, 0, 0, 0x00, 0, 0}; 

// A nice basic init function to set up the global state machine.
// In future work we might add some functionality to look for an
// existing state object and pull that instead for resumability.
void GAME_initStateStruct(void){
    unsigned int debug_disabled; // FUTURE: Replace with a call seperate to this one.
    debug_disabled = GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN6);
    StateMachine.AGE = 0x00;
    StateMachine.ACT = 0x03; // Special activity level for eggs. We're "awake" in a sense but behaviour is notedly different.
    StateMachine.HUNGER_FUN = 0x00;
    StateMachine.DISCIPLINE = 0x00;
    StateMachine.NAUGHTY = 0x00; // Reasonable starting value, may need to be tweaked during testing.
    if (debug_disabled){
        StateMachine.STAGE_ID = 0x00; // Reserved Species ID for Eggs
    }
    else {
        StateMachine.STAGE_ID = 0x01; // Skip hatching an egg for testing
    }
    StateMachine.HEALTH_BYTE = 0x00;

    NEXT_STAGE_TRANSITION_AGE = 0x00;
    NEXT_STAGE_TRANSITION_HOURS = 0x00;
    NEXT_STAGE_TRANSITION_MINUTES = 0x00;
    calendar_initial_setup_completed = false;  // we need this flag and the next one so that we can have the state incrementer do special egg handling AFTER the RTC scene is finished
    egg_delay_set = false;                    // Normally we can do with an evolution function but that dog doesn't hunt with eggs.
    StateMachine.INIT = true;
}

void GAME_NEEDS_evaluateHungerFun(unsigned int hf_minutes){
    unsigned int active_hfrate = EVO_metaStruct[StateMachine.STAGE_ID].rateHF;
    unsigned int rate_hunger = active_hfrate >> 4 & 0xF;
    unsigned int rate_fun = active_hfrate & 0xF;
    unsigned int current_hunger = StateMachine.HUNGER_FUN >> 4 & 0xF;
    unsigned int current_fun = StateMachine.HUNGER_FUN & 0xF;

    hf_minutes = hf_minutes % 16;

    if (rate_hunger >= hf_minutes){
        current_hunger--;
    }
    if (rate_fun >= hf_minutes){
        current_fun--;
    }

    // These two if statements handle the case where the value exceeds the floor at 0
    if (current_hunger > 15){
        current_hunger = 0;
    }
    if (current_fun > 15){
        current_fun = 0;
    }

    if ((current_hunger == 0) || (current_fun == 0)){
        ALERTS_hunger_fun_alert();
    }

    StateMachine.HUNGER_FUN = (current_hunger << 4) + current_fun;

}

//provided with signed integers for the change in hunger and change in fun
//desired, this function extracts those values from StateMachine.HUNGER_FUN,
//modifies them within their boundary sizes, then puts them back together on
//the state machine. Provided for convenience when writing scenes that modify
//this value or game state functions that do the same.
void GAME_applyHungerFun(int change_hunger, int change_fun){
    unsigned int current_hunger = StateMachine.HUNGER_FUN >> 4 & 0xF;
    unsigned int current_fun = StateMachine.HUNGER_FUN & 0xF;

    current_hunger = current_hunger + change_hunger;
    current_fun = current_fun + change_fun;

    // These two statements handle bounding in a way that avoids weird wrapping;
    // which is why don't use modular arithmetic.
    if (change_hunger > 0){         // The risk is exceeding 15, the upper bound value.
        if (current_hunger > 15){
            current_hunger = 15;
        }
    }
    else {
        if (current_hunger > 15){    // The risk is wrapping around to a large value
            current_hunger = 0;
        }
    }

    if (change_fun > 0){
        if (current_fun > 15){
            current_fun = 15;
        }
    }
    else {
        if (current_fun > 15){
            current_fun = 0;
        }
    }
    //Collapse bitwise back into the magic state thing.
    StateMachine.HUNGER_FUN = (current_hunger << 4) + current_fun;
}

// this function is the ultimate control function for all timed events, and needs to be updated
// if new time-based effects are added to the game, such as hunger degradation and so-forth.
// Several of the obviously-required-in-future functions are included as comments below.

void GAME_evaluateTimedEvents(void){
    unsigned int current_minutes, current_hours, current_seconds;
    Calendar TempTime = RTC_C_getCalendarTime(RTC_C_BASE);
    current_minutes = RTC_C_convertBCDToBinary(RTC_C_BASE, TempTime.Minutes);
    current_hours = RTC_C_convertBCDToBinary(RTC_C_BASE, TempTime.Hours);
    current_seconds = RTC_C_convertBCDToBinary(RTC_C_BASE, TempTime.Seconds);
    if (calendar_initial_setup_completed && !egg_delay_set){
        NEXT_STAGE_TRANSITION_MINUTES = current_minutes + egg_delay; // Eggs are special, they live five minutes, which is why we need logic TODO fix
        NEXT_STAGE_TRANSITION_HOURS = current_hours;
        if (NEXT_STAGE_TRANSITION_MINUTES > 59){    // We have entered an unreachable time.
            NEXT_STAGE_TRANSITION_MINUTES = NEXT_STAGE_TRANSITION_MINUTES % 60;
            NEXT_STAGE_TRANSITION_HOURS++;
        }
        egg_delay_set = true;
    }
    if ((NEXT_STAGE_TRANSITION_AGE <= StateMachine.AGE) && (NEXT_STAGE_TRANSITION_HOURS <= current_hours) && (NEXT_STAGE_TRANSITION_MINUTES <= current_minutes) && egg_delay_set){
       // GAME_EVO_incrementForEvolution(); Normally, we would call this evolution function, but I don't want to write all that before I test it. Instead:
        StateMachine.STAGE_ID = 0x01; // We always hatch to the first baby.
        FORCE_REFRESH = true; // Needed to redraw the menu on the first frame. Should be included in that evolution function.
        NEXT_STAGE_TRANSITION_AGE = 0xFF; // needed to avoid looping in this, which causes animation problems. Normally set in the evolution function.
        //FUTURE: should change activity levels once those are implemented.
    }
    if ((current_seconds == 0) && egg_delay_set && (StateMachine.STAGE_ID > 0x00) && needs_evaluation){ // At the round minute, we need to check for these several functions, only if not an egg.
        GAME_NEEDS_evaluateHungerFun(current_minutes);
        //GAME_NEEDS_evaluatePooped();
        //GAME_NEEDS_evaluateIllness();
        //GAME_NEEDS_evaluateDeath();
        needs_evaluation = false;
    }
    if (current_seconds == 1){
        needs_evaluation = true; // This has been done to prevent "double dipping" on effects like hunger and so on.
    }
}
