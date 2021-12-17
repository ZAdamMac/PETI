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

unsigned int egg_delay_set;
unsigned int egg_delay = 0x01; // The length of the egg state in minutes. Gameplay default is 5, but can be tweaked for testing.

// A nice basic init function to set up the global state machine.
// In future work we might add some functionality to look for an
// existing state object and pull that instead for resumability.
void GAME_initStateStruct(void){
    StateMachine.AGE = 0x00;
    StateMachine.ACT = 0x03; // Special activity level for eggs. We're "awake" in a sense but behaviour is notedly different.
    StateMachine.HUNGER_FUN = 0x00;
    StateMachine.DISCIPLINE = 0x00;
    StateMachine.NAUGHTY = 0x00; // Reasonable starting value, may need to be tweaked during testing.
    StateMachine.STAGE_ID = 0x01; // Reserved Species ID for Eggs
    StateMachine.HEALTH_BYTE = 0x00;

    NEXT_STAGE_TRANSITION_AGE = 0x00;
    NEXT_STAGE_TRANSITION_HOURS = 0x00;
    NEXT_STAGE_TRANSITION_MINUTES = 0x00;
    calendar_initial_setup_completed = false;  // we need this flag and the next one so that we can have the state incrementer do special egg handling AFTER the RTC scene is finished
    egg_delay_set = false;                    // Normally we can do with an evolution function but that dog doesn't hunt with eggs.

    SCENE_ACT = 0x00; // We can now play the boot screen
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
        //TODO should change activity levels once those are implemented.
    }
    if (current_seconds == 0){ // At the round minute, we need to check for these several functions, none of which actually exist yet.
        GAME_NEEDS_evaluateHungerFun(current_minutes);
        //GAME_NEEDS_evaluatePooped();
        //GAME_NEEDS_evaluateIllness();
        //GAME_NEEDS_evaluateDeath();
    }
}
