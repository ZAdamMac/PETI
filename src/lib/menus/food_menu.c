/*
 * food_menu.c
 *
 *  Created on: Dec 6, 2021
 *      Author: patches
 */

#include "lib/scenes/scenes_manager.h"
#include "lib/scenes/menu_generator.h"


void MENU_FOOD_goto_Rice(void){
    SCENE_ACT = SCENEADDR_proof_text;
}


const int MENU_FOOD_count_options = 1;

voidFuncPointer MENU_FOOD_functions[1] = {
                                          MENU_FOOD_goto_Rice
};
