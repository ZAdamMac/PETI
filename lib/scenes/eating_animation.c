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

unsigned int eating_frame = 0;


void EAT_handleInputs(void){
    if (buttons_state & button_a_toggle){  //A scrolls up by 1
        eating_frame = 10;
        buttons_state ^= button_a_toggle;
    }
    if (buttons_state & button_b_toggle){  //B scrolls down by 1
        eating_frame = 10;
        buttons_state ^= button_b_toggle;
    }
    if (buttons_state & button_c_toggle){  //C exits the menu outright
        eating_frame = 10;
        buttons_state ^= button_c_toggle;
    }
    if (buttons_state & button_d_toggle){  //D also exits the menu outright.
        eating_frame = 10;
        buttons_state ^= button_d_toggle;
        }
}

char * EAT_computeLine(const char active_animation[], int size, unsigned int current_frame, int active_line, char food_animation[]){
    int iterator;

    //Blank the line before starting.
    for (iterator = 0;iterator < PIXELS_X/FONT_SIZE_FLOOR_X; iterator++){
        WORK_STRING[iterator] = ' '; // Let's always start blank.
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
    Stage active_species = EVO_metaStruct[StateMachine.STAGE_ID];
    // We needs these lines to be blank every time.
    strcpy(DISPLAY_FRAME.frame[0].line, " ");
    strcpy(DISPLAY_FRAME.frame[7].line, " ");
    strcpy(DISPLAY_FRAME.frame[8].line, " ");

    if (eating_frame > 7){ // By now the food is on the ground
        for (active_line = 0;active_line<6; active_line++){
            strcpy(DISPLAY_FRAME.frame[active_line+1].line, EAT_computeLine(active_species.animationEating[eating_frame % 2], active_species.size, eating_frame, active_line, FOODSEL_active_food.animation)); // Compute the animated text for this row.
            strcpy(DISPLAY_FRAME.frame[active_line+1].directives, "0000000000000000"); // Compute the directive information.
        }
    }
    else { // Unless it's not
        for (active_line = 0;active_line<6; active_line++){
            strcpy(DISPLAY_FRAME.frame[active_line+1].line, EAT_computeLine(active_species.animation[eating_frame % 2], active_species.size, eating_frame, active_line, FOODSEL_active_food.animation)); // Compute the animated text for this row.
            strcpy(DISPLAY_FRAME.frame[active_line+1].directives, "0000000000000000"); // Compute the directive information.
        }
    }
}

void EAT_eatFood(void){
    unsigned int current_hunger = StateMachine.HUNGER_FUN >> 4 & 0xF;
    unsigned int current_fun = StateMachine.HUNGER_FUN & 0xF;

    current_hunger = current_hunger + FOODSEL_active_food.hunger_value;
    current_fun = current_fun + FOODSEL_active_food.fun_value;

    // These two if statements handle the case where the value exceeds the maximum allowable of 15.
    if (current_hunger > 15){
        current_hunger = 15;
    }
    if (current_fun > 15){
        current_fun = 15;
    }

    StateMachine.HUNGER_FUN = (current_hunger << 4) + current_fun;

    //TODO: Implement the weight mod stuff; this is currently unused

}

// A main function to be called by the scene_manger to handle the inputs and then compute the next frame of animation, before sending all the updates to
// DISPLAY. Handling the inputs itself can set the exit conditions for this scene so there is no need for special exit logic.
void SCENE_eating(void){
    EAT_handleInputs();
    EAT_computeNextFrame();
    DISPLAY_updatesOnly_enhanced(&DISPLAY_FRAME, MODE_GAME);
    if (eating_frame > 9) {
        EAT_eatFood();
        SCENE_ACT = SCENEADDR_main_game;
        eating_frame = 0;
    }
    else {
        eating_frame++;
    }

}
