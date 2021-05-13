/*
 * game_manager.h
 *
 *  Created on: May 12, 2021
 *      Author: patches
 */

#ifndef LIB_GAME_GAME_MANAGER_H_
#define LIB_GAME_GAME_MANAGER_H_

typedef struct GameState {
    unsigned int AGE;
    unsigned int ACT;
    unsigned int HUNGER_FUN;
    unsigned int DISCIPLINE;
    unsigned int NAUGHTY;
    unsigned int STAGE_ID;
    unsigned int HEALTH_BYTE;
} GameState;

unsigned int NEXT_STAGE_TRANSITION_AGE;
unsigned int NEXT_STAGE_TRANSITION_HOURS;
unsigned int NEXT_STAGE_TRANSITION_MINUTES;
GameState StateMachine;

void GAME_initStateStruct(void);
void GAME_evaluateTimedEvents(void);


#endif /* LIB_GAME_GAME_MANAGER_H_ */
