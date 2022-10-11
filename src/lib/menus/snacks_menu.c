/*
 * snacks_menu.c
 *
 *  Created on: Dec 6, 2021
 *      Author: patches
 */

#include "lib/scenes/scenes_manager.h"
#include "lib/scenes/menu_generator.h"


void MENU_SNACKS_goto_Lollypop(void){
    SCENE_ACT = SCENEADDR_proof_text;
}


const int MENU_SNACKS_count_options = 1;

voidFuncPointer MENU_SNACKS_functions[1] = {
                                          MENU_SNACKS_goto_Lollypop
};
