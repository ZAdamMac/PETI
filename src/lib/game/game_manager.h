/*
 * game_manager.h
 *
 *  Created on: May 12, 2021
 *      Author: patches
 */

#ifndef LIB_GAME_GAME_MANAGER_H_
#define LIB_GAME_GAME_MANAGER_H_

#define GAME_NEEDS_teen_bedtime 20      
#define GAME_NEEDS_adult_bedtime 21
#define GAME_NEEDS_senior_bedtime 21
#define GAME_NEEDS_final_bedtime 21
#define GAME_NEEDS_teen_wakeup 8      
#define GAME_NEEDS_adult_wakeup 8
#define GAME_NEEDS_senior_wakeup 9
#define GAME_NEEDS_final_wakeup 9

//These floats modify the rate at which the pet poops; lower is more frequent.
#define GAME_NEEDS_baby_poop_rate 0.3
#define GAME_NEEDS_teen_poop_rate 0.5
#define GAME_NEEDS_adult_poop_rate 0.2
#define GAME_NEEDS_senior_poop_rate 0.2
#define GAME_NEEDS_sick_poop_rate_modifier 3

#define GM_ACTIVITY_SLEEPING 0x00
#define GM_ACTIVITY_IDLE 0x01
#define GM_ACTIVITY_SICK 0x02
#define GM_ACTIVITY_ISEGG 0x03

#define GM_MAX_POOPS 0x04  // Determines the maximum number of poops to be allowed in the game state.

typedef struct GameState {      // A metastructure to hold the current game state as one structure. Contains most of the relevant data about the active PET.
    unsigned int AGE;           // Integer number indicating the time in days-past-midnight of the PET having been hatched, starting from 0
    unsigned int ACT;           // Activity state of the PET. 0 for sleeping, 1 for awake and normal, 2 for sick, and 3 for the egg life state.
    unsigned int HUNGER_FUN;    // A byte where the upper nibble is the current hunger score and the lower nibble is the current fun score.
    unsigned int DISCIPLINE;    // Influenced by various in-game factors and factor in controlling the evolution path.
    unsigned int NAUGHTY;       // bounded to 100, variable intended to influence the likelihood the pet alerts the user for no reason.
    unsigned int STAGE_ID;      // Corresponds to the index of EVO_metaStruct at which the currently-active "stage" or species of PET is stored.
    unsigned int HEALTH_BYTE;   // Holds various bits or nibbles as indicators of various other health elements.
    unsigned int POOP_COUNT;    // Simple int usually bounded 0-4 which indicates how many times pet has pooped since last bath.
    unsigned int INIT;          // non-zero value indicates structure initialized
    unsigned int OLD_STAGE_ID;  // Corresponds to the index of EVO_metastruct which was active previous to the current STAGE_ID
} GameState;

// These three globals are not technically part of the original state spec but needed to determine the next time at which evolution mechanics need to be calculated.
// It might be advisable to refactor such that they are part of the state object, especially if we are going to make the game state savable or expressible.

unsigned int NEXT_STAGE_TRANSITION_AGE;     // Shared global integer indicating the age/day value at which point the pet should trigger evolution
unsigned int NEXT_STAGE_TRANSITION_HOURS;   // As above, for the hours.
unsigned int NEXT_STAGE_TRANSITION_MINUTES; // As further above,f or the minutes.

#pragma PERSISTENT(StateMachine)
extern GameState StateMachine;                     // Any file that includes game_manager.h can interrogate this for the current game state.

void GAME_initStateStruct(void);
void GAME_evaluateTimedEvents(void);
void GAME_evaluateWakeUpEvent(void);
void GAME_applyHungerFun(int change_hunger, int change_fun);


#endif /* LIB_GAME_GAME_MANAGER_H_ */
