//***************************************************************************************
//  PETI FOSS Virtual Pet - Eating Animation Scene
//
//  This code geneartes the animation files for eating.
//
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  December 6 2021
//***************************************************************************************

#include "lib/game/food_data.h"
#include "food_selector.h"
#include <msp430.h>
#include <string.h>
#include "lib/display/display.h"
#include "lib/game/game_manager.h"
#include "lib/game/evo_data.h"
#include "driverlib.h"
#include "eating_animation.h"
#include "scenes_manager.h"
#include "main.h"
#include "lib/hwinit/human_input.h"


void EAT_handleInputs(void){
    if (HID_input_events_queue[0] != BUTTON_NO_PRESS){ // If any button was pressed at all, skip to the end.
        SCENE_FRAME = 10;
        HID_input_events_queue[0] = BUTTON_NO_PRESS;
    }
}

char * EAT_computeLine(const char active_animation[], int size, unsigned int current_frame, int active_line, char food_animation[]){
    int iterator;

    //Blank the line before starting.
    for (iterator = 0;iterator < PIXELS_X/FONT_SIZE_FLOOR_X; iterator++){
        WORK_STRING[iterator] = ' '; // Let's always start blank. <- WHY!?
    }

    //Animate the food first
    if (current_frame > 0){ // We are ready for the food!
        if (current_frame - active_line == 1){ // This works generally, all the way down to frame 7
            WORK_STRING[EATING_falling_column] = food_animation[0];
        }
        if (current_frame > 6 && current_frame < 10){ // actively eating!
            if (active_line == 5){
                WORK_STRING[EATING_falling_column] = food_animation[current_frame - 7];
            }
        }
        if (current_frame == 10){ // We have eaten everything
            if (active_line == 5){
                WORK_STRING[EATING_falling_column] = ' ';
            }
        }
    }



    // Now, draw our friend.
    if (active_line > 1){
        switch (size){
        case 1:
            if (active_line == 5){
                WORK_STRING[2] = active_animation[0];
            }
            break;
        case 4:
            if (active_line == 4){
                WORK_STRING[1] = active_animation[0];
                WORK_STRING[2] = active_animation[1];
            }
            if (active_line == 5){
                WORK_STRING[1] = active_animation[2];
                WORK_STRING[2] = active_animation[3];
            }
            break;
        case 9:
            if  (active_line == 3){
                WORK_STRING[0] = active_animation[0];
                WORK_STRING[1] = active_animation[1];
                WORK_STRING[2] = active_animation[2];
            }
            if (active_line == 4){
                WORK_STRING[0] = active_animation[3];
                WORK_STRING[1] = active_animation[4];
                WORK_STRING[2] = active_animation[5];
            }
            if (active_line == 5){
                WORK_STRING[0] = active_animation[6];
                WORK_STRING[1] = active_animation[7];
                WORK_STRING[2] = active_animation[8];
            }
            break;
        }
    }

    return WORK_STRING;
}

void EAT_computeNextFrame(void){
    int active_line;
    int row, col;
    Stage active_species = EVO_metaStruct[StateMachine.STAGE_ID];
    // We needs these lines to be blank every time.
    strcpy(DISPLAY_FRAME.frame[0].line, "\x0"); // Under the display logic, this terminates printing a line.
    strcpy(DISPLAY_FRAME.frame[7].line, "\x0");
    strcpy(DISPLAY_FRAME.frame[8].line, "\x0");

    for (row = 0; row<PIXELS_Y/FONT_SIZE_FLOOR_Y; row++){
        for (col=0; col<PIXELS_X/FONT_SIZE_FLOOR_X; col++){
            DISPLAY_FRAME.frame[row].directives[col] = active_species.font + DIRECTIVE_NORMAL;
        }
        // FUTURE: If implementing font support for foods, that change goes here.
        DISPLAY_FRAME.frame[row].directives[EATING_falling_column] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
    }

    if (SCENE_FRAME > 7){ // By now the food is on the ground
        for (active_line = 0;active_line<6; active_line++){
            strncpy(DISPLAY_FRAME.frame[active_line+1].line, EAT_computeLine(active_species.animationEating[SCENE_FRAME % 2], active_species.size, SCENE_FRAME, active_line, FOODSEL_active_food.animation), (PIXELS_X/FONT_SIZE_FLOOR_X)); // Compute the animated text for this active_line.
        }
    }
    else { // Unless it's not
        for (active_line = 0;active_line<6; active_line++){
            strncpy(DISPLAY_FRAME.frame[active_line+1].line, EAT_computeLine(active_species.animation[SCENE_FRAME % 2], active_species.size, SCENE_FRAME, active_line, FOODSEL_active_food.animation), (PIXELS_X/FONT_SIZE_FLOOR_X)); // Compute the animated text for this active_line.
        }
    }
}

void EAT_eatFood(void){
    GAME_applyHungerFun(FOODSEL_active_food.hunger_value, FOODSEL_active_food.fun_value);
    //FUTURE: Implement the weight mod stuff; this is currently unused

}

// A main function to be called by the scene_manger to handle the inputs and then compute the next frame of animation, before sending all the updates to
// DISPLAY. Handling the inputs itself can set the exit conditions for this scene so there is no need for special exit logic.
void SCENE_eating(void){
    EAT_handleInputs();
    EAT_computeNextFrame();
    DISPLAY_updatesOnly_enhanced(&DISPLAY_FRAME, MODE_GAME);
    if (SCENE_FRAME > 9) {
        EAT_eatFood();
        SCENE_ACT = SCENEADDR_main_game;
        SCENE_FRAME = 0;
    }
    else {
        SCENE_FRAME++;
    }

}
