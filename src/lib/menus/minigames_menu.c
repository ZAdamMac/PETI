/*
 * feeding_menu.c
 *
 *  Created on: Dec 6, 2021
 *      Author: patches
 */

#include "lib/scenes/scenes_manager.h"
#include "lib/scenes/menu_generator.h"


void MINIGAME_goto_RPS(void){
    SCENE_ACT = SCENEADDR_minigame_rockpaperscissors;
};


const int MINIGAME_count_options = 1;

voidFuncPointer MINIGAME_array_games[1] = {
                                           MINIGAME_goto_RPS,
};
