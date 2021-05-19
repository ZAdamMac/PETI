/*
 * game_manager.h
 *
 *  Created on: May 12, 2021
 *      Author: patches
 */

#ifndef LIB_GAME_GAME_MANAGER_H_
#define LIB_GAME_GAME_MANAGER_H_

typedef struct GameState {      // A metastructure to hold the current game state as one structure. Contains most of the relevant data about the active PET.
    unsigned int AGE;           // Integer number indicating the time in days-past-midnight of the PET having been hatched, starting from 0
    unsigned int ACT;           // Activity state of the PET. 0 for sleeping, 1 for awake and normal, 2 for sick, and 3 for the egg life state.
    unsigned int HUNGER_FUN;    // A byte where the upper nibble is the current hunger score and the lower nibble is the current fun score.
    unsigned int DISCIPLINE;    // Influenced by various in-game factors and factor in controlling the evolution path.
    unsigned int NAUGHTY;       // bounded to 100, variable intended to influence the likelihood the pet alerts the user for no reason.
    unsigned int STAGE_ID;      // Corresponds to the index of EVO_metaStruct at which the currently-active "stage" or species of PET is stored.
    unsigned int HEALTH_BYTE;   // Holds various bits or nibbles as indicators of various other health elements.
} GameState;

// These three globals are not technically part of the original state spec but needed to determine the next time at which evolution mechanics need to be calculated.
// It might be advisable to refactor such that they are part of the state object, especially if we are going to make the game state savable or expressible.

unsigned int NEXT_STAGE_TRANSITION_AGE;     // Shared global integer indicating the age/day value at which point the pet should trigger evolution
unsigned int NEXT_STAGE_TRANSITION_HOURS;   // As above, for the hours.
unsigned int NEXT_STAGE_TRANSITION_MINUTES; // As further above,f or the minutes.
GameState StateMachine;

void GAME_initStateStruct(void);
void GAME_evaluateTimedEvents(void);


#endif /* LIB_GAME_GAME_MANAGER_H_ */
