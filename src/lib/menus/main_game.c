/*
 * main_game.c
 *
 *  Created on: Oct. 5, 2023
 *      Author: patches
 */

#include "lib/scenes/scenes_manager.h"
#include "lib/scenes/menu_generator.h"
#include "lib/game/game_manager.h"
#include "lib/display/display.h"
#include "lib/scenes/main_game.h"


void MENU_MG_gotoStatusMenu(void){
    SCENE_ACT = SCENEADDR_status_menu;
}

void MENU_MG_gotoSettingsMenu(void){
    SCENE_ACT = SCENEADDR_proof_text;//Currently no settings menu, do this instead;
}

void MENU_MG_gotoFeedingMenu(void){
    if (StateMachine.ACT != 0){ // Refuse to do this if the pet is not awake
        SCENE_ACT = SCENEADDR_feed_menu;
    }
}

void MENU_MG_gotoMinigamesMenu(void){
    if (StateMachine.ACT != 0){ // Pet must be awake to play games.
        SCENE_ACT = SCENEADDR_minigames_menu;
    }
}

void MENU_MG_gotoClockMenu(void){
    SCENE_ACT = SCENEADDR_calendar_menu; // FUTURE: replace with explicit call to a dedicated calendar display
}

void MENU_MG_gotoDebugMenu(void){
    SCENE_ACT = SCENEADDR_debug_menu;
}

void MENU_MG_toggleLightsMenu(void){
    if (MG_lights_on == 1){ // Flippy Bit!
        MG_lights_on = 0; 
    }
    else {
        MG_lights_on = 1;
    }
}

void MENU_MG_washPet(void){
    SCENE_ACT = SCENEADDR_proof_text;//Currently no settings menu, do this instead;
}


#pragma PERSISTENT(MG_menu_options_array);
voidFuncPointer MG_menu_options_array[8] = {
    MENU_MG_gotoStatusMenu,
    MENU_MG_gotoFeedingMenu,
    MENU_MG_gotoMinigamesMenu,
    MENU_MG_toggleLightsMenu,
    MENU_MG_gotoClockMenu,
    MENU_MG_gotoSettingsMenu,
    MENU_MG_washPet,
    MENU_MG_gotoDebugMenu
};
