/*
 * debug_menu.c
 *
 *  Created on: Nov. 8, 2021
 *      Author: patches
 */

#include "lib/scenes/scenes_manager.h"
#include "lib/scenes/menu_generator.h"


void MENU_DEBUG_goto_DemoMode(void){
    SCENE_ACT = SCENEADDR_demo_mode;
}

void MENU_DEBUG_goto_Proofer(void){
    SCENE_ACT = SCENEADDR_proof_text;
}

void MENU_DEBUG_goto_SoundTest(void){
    SCENE_ACT = SCENEADDR_sound_test;
}

#pragma PERSISTENT(MENU_DEBUG_count_options);
const int MENU_DEBUG_count_options = 3;

#pragma PERSISTENT(MENU_DEBUG_count_options);
voidFuncPointer MENU_DEBUG_functions[3] = {
                                      MENU_DEBUG_goto_DemoMode,
                                      MENU_DEBUG_goto_Proofer,
                                      MENU_DEBUG_goto_SoundTest
};
