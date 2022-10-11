/*
 * feeding_menu.c
 *
 *  Created on: Dec 6, 2021
 *      Author: patches
 */

#include "lib/scenes/scenes_manager.h"
#include "lib/scenes/menu_generator.h"


void MENU_FEED_goto_Food(void){
    SCENE_ACT = SCENEADDR_food_menu;
}

void MENU_FEED_goto_Snacks(void){
    SCENE_ACT = SCENEADDR_snacks_menu;
}


const int MENU_FEED_count_options = 2;

voidFuncPointer MENU_FEED_functions[2] = {
                                           MENU_FEED_goto_Food,
                                           MENU_FEED_goto_Snacks
};
