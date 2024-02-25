/*
 * debug_menu.c
 *
 *  Created on: Nov. 8, 2021
 *      Author: patches
 */

#include "lib/scenes/scenes_manager.h"
#include "lib/scenes/menu_generator.h"
#include "lib/game/game_manager.h"
#include "lib/display/display.h"


void MENU_DEBUG_goto_Proofer(void){
    SCENE_ACT = SCENEADDR_proof_text;
}

void MENU_DEBUG_goto_SoundTest(void){
    SCENE_ACT = SCENEADDR_sound_test;
}

void MENU_DEBUG_fill_hunger(void){
    unsigned int current_hunger = StateMachine.HUNGER_FUN >> 4 & 0xF; // Take the upper nibble
    unsigned int needed_points = (15 - current_hunger) << 4; // Diff
    StateMachine.HUNGER_FUN = StateMachine.HUNGER_FUN + needed_points; // Put that back where we found it.
}

void MENU_DEBUG_fill_fun(void){
    unsigned int current_fun = StateMachine.HUNGER_FUN & 0xF;
    unsigned int needed_points = (15 - current_fun);
    StateMachine.HUNGER_FUN = StateMachine.HUNGER_FUN + needed_points;
}

void MENU_DEBUG_deplete_hunger(void){
    unsigned int current_hunger = StateMachine.HUNGER_FUN & 0xF0; // Take the upper nibble
    StateMachine.HUNGER_FUN = StateMachine.HUNGER_FUN - current_hunger; // Put that back where we found it.
}

void MENU_DEBUG_deplete_fun(void){
    unsigned int current_fun = StateMachine.HUNGER_FUN & 0xF;
    StateMachine.HUNGER_FUN = StateMachine.HUNGER_FUN - current_fun;
}

void MENU_DEBUG_goto_TimeMachine(void){
    SCENE_ACT = SCENEADDR_stage_select;
}

void MENU_DEBUG_goto_RNGDebug(void){
    SCENE_ACT = SCENEADDR_rng_debug;
}

void MENU_DEBUG_sleepDisplay(void){
    DISPLAY_sleepLCD();
}

void MENU_debug_incrementAge(void){
    StateMachine.AGE += 1;
}

void MENU_debug_forceEvolution(void){
    GAME_EVO_incrementForEvolution();
}

#pragma PERSISTENT(MENU_DEBUG_count_options);
const int MENU_DEBUG_count_options = 11;

#pragma PERSISTENT(MENU_DEBUG_count_options);
voidFuncPointer MENU_DEBUG_functions[11] = {
                                      MENU_DEBUG_goto_Proofer,
                                      MENU_DEBUG_goto_SoundTest,
                                      MENU_DEBUG_fill_hunger,
                                      MENU_DEBUG_deplete_hunger,
                                      MENU_DEBUG_fill_fun,
                                      MENU_DEBUG_deplete_fun,
                                      MENU_DEBUG_goto_TimeMachine,
                                      MENU_DEBUG_goto_RNGDebug,
                                      MENU_DEBUG_sleepDisplay,
                                      MENU_debug_incrementAge,
                                      MENU_debug_forceEvolution
};
