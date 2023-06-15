//***************************************************************************************
//  PETI FOSS Virtual Pet - Rock Paper Scissors Minigame
//
//  The Pet can play Rock Paper Scissors against he player to potentially increase
//  the happiness score and to help regulate physical health states. The game is
//  played best-of-five.
//
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  April 2023
//***************************************************************************************

#include "lib/display/display.h"
#include "lib/hwinit/hwinit.h"
#include "lib/hwinit/human_input.h"
#include "driverlib.h"
#include <msp430.h>
#include <string.h>
#include "main.h"
#include "lib/locales/enCA_strings.h"
#include "lib/scenes/scenes_manager.h"
#include "lib/scenes/menu_generator.h"
#include "lib/game/evo_data.h"
#include "lib/game/game_manager.h"
#include "lib/game/entropy.h"


// By defining the choice values in this way, we map them directly to the
// charater positions in FONT_ADDRESS_0 that represents their symbol.
// This makes things cleaner when it comes to other functions later on.
#define MINIGAME_RPS_choiceNotMade 0
#define MINIGAME_RPS_choiceRock '\xF7'
#define MINIGAME_RPS_choicePaper '\xF6'
#define MINIGAME_RPS_choiceScissors '\xF5'

char MINIGAME_RPS_choice;
char MINIGAME_RPS_petChoice;

signed int MINIGAME_RPS_score;

// This scene will set SCENE_current_page to one of the following depending on 
// what step of the animation we're in.
#define MINIGAME_RPS_attract 0      // Waiting for user input.
#define MINIGAME_RPS_eval 1         // Computing and displaying the result of a move
#define MINIGAME_RPS_final_page 2   // Computing and displaying the result of the gameplay.

// This scene absuses SCENE_PAGE_COUNT as a general purpose store to keep track
// of how many rounds the user has played. If it's equal to the value defined
// it changes state to scoring the game.
#define MINIGAME_RPS_best_of 5

/// For balance reasons, we may want to change how much happiness is granted for each successful play.
#define MINIGAME_RPS_prize 5

// We need our own scene-specific input handling, which will probably almost always be the case for scenes.
void MINIGAME_RPS_handleInputs(void){
    int i;
    unsigned int this_event;
    for (i =0; i <= HID_input_events_queue_depth; ++i) // For every event in the event queue;
    {
        this_event = HID_input_events_queue[i]; //fetch the event.
        switch (this_event){
            case(BUTTON_A_PRESS): // Select for Rock
		        MINIGAME_RPS_choice = MINIGAME_RPS_choiceRock;
                SCENE_CURRENT_PAGE = MINIGAME_RPS_eval;
                SCENE_FRAME = 0;
            	break;
            case(BUTTON_B_PRESS): // Select for Paper
		        MINIGAME_RPS_choice = MINIGAME_RPS_choicePaper;
                SCENE_CURRENT_PAGE = MINIGAME_RPS_eval;
                SCENE_FRAME = 0;
                break;
            case(BUTTON_C_PRESS): //Select for Scissors
		        MINIGAME_RPS_choice = MINIGAME_RPS_choiceScissors;
                SCENE_CURRENT_PAGE = MINIGAME_RPS_eval;
                SCENE_FRAME = 0;
                break;
            case(BUTTON_D_PRESS): // Exit
	            SCENE_ACT = SCENEADDR_main_game;
                SCENE_CURRENT_PAGE = MINIGAME_RPS_eval;
                SCENE_FRAME = 0;
                break;
        }
        HID_input_events_queue[i] = BUTTON_NO_PRESS;
    };
    HID_input_events_queue_depth = 0;
}

//
void MINIGAME_RPS_attractFrame(void){
    int active_line;
    int row, col;
    Stage active_species = EVO_metaStruct[StateMachine.STAGE_ID];
    strcpy(DISPLAY_FRAME.frame[0].line, "Make a choice!");
    strcpy(DISPLAY_FRAME.frame[1].line, "\xF7      \xF5"); //TODO Define these characters and below on symbols.
    strcpy(DISPLAY_FRAME.frame[6].line, "\xF6      \xF0");

    //placeholder logic
    //TODO: Replace with the real stuff
    for (row = 0; row<PIXELS_Y/FONT_SIZE_FLOOR_Y; row++){
        for (col=0; col<PIXELS_X/FONT_SIZE_FLOOR_X; col++){
            DISPLAY_FRAME.frame[row].directives[col] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
        }
    }

    strcpy(DISPLAY_FRAME.frame[2].line, "  \xF2\xF1\xF2   "); // TODO it'd be nice to have this as a function itself.
    strcpy(DISPLAY_FRAME.frame[3].line, "  \xEF\xF7\xEF   ");
    strcpy(DISPLAY_FRAME.frame[4].line, "  \xF3\xF4\xF3   ");
    DISPLAY_FRAME.frame[4].directives[4] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
    DISPLAY_FRAME.frame[2].directives[4] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
    DISPLAY_FRAME.frame[3].directives[4] = FONT_ADDR_0 + DIRECTIVE_REVERSED;

}

void MINIGAME_RPS_drawChoice(float random){
        if (random > 0.6666){
            MINIGAME_RPS_petChoice = MINIGAME_RPS_choiceRock;
        }
        else if (random > 0.3333){
            MINIGAME_RPS_petChoice = MINIGAME_RPS_choicePaper;
        }
        else {
            MINIGAME_RPS_petChoice = MINIGAME_RPS_choiceScissors;
        }
}

void MINIGAME_RPS_evaluateForScoring(void){
    SCENE_PAGE_COUNT++;
    if (MINIGAME_RPS_choice == MINIGAME_RPS_petChoice){ // This is the general case of a draw.
        MINIGAME_RPS_score += 0;
        return;
    }
    else { // Handling actual disparities is a little more branchy
        switch(MINIGAME_RPS_choice){
            case (MINIGAME_RPS_choiceRock):
                if (MINIGAME_RPS_petChoice == MINIGAME_RPS_choiceScissors) {
                    MINIGAME_RPS_score += 1; // You won the hand
                }
                else {
                    MINIGAME_RPS_score += -1; // You have necessarily lost the hand
                }
                break;
            case (MINIGAME_RPS_choicePaper):
                if (MINIGAME_RPS_petChoice == MINIGAME_RPS_choiceRock) {
                    MINIGAME_RPS_score += 1; // You won the hand
                }
                else {
                    MINIGAME_RPS_score += -1; // You have necessarily lost the hand
                }
                break;
            case (MINIGAME_RPS_choiceScissors):
                if (MINIGAME_RPS_petChoice == MINIGAME_RPS_choicePaper) {
                    MINIGAME_RPS_score += 1; // You won the hand
                }
                else {
                    MINIGAME_RPS_score += -1; // You have necessarily lost the hand
                }
                break;
        }
    }
}

//
void MINIGAME_RPS_evaluateFrame(void){
    int active_line;
    int row, col;
    if (SCENE_FRAME == 0) { // In such an event, we need to "blank" the frame directives and lines
            for (row = 0; row<PIXELS_Y/FONT_SIZE_FLOOR_Y; row++){
                for (col=0; col<PIXELS_X/FONT_SIZE_FLOOR_X; col++){
                    DISPLAY_FRAME.frame[row].directives[col] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                    DISPLAY_FRAME.frame[row].line[col]=' ';
                }
        }
        MINIGAME_RPS_drawChoice(RNG_drawFloat());
        MINIGAME_RPS_evaluateForScoring();
    }

    //TODO eventually this will be animated, but I'm trying to get the logo for now.
    DISPLAY_FRAME.frame[3].line[1] = MINIGAME_RPS_choice;
    DISPLAY_FRAME.frame[3].line[6] = MINIGAME_RPS_petChoice;

    SCENE_FRAME++;
    if (SCENE_FRAME >= 4){
        SCENE_FRAME = 0;
        if (SCENE_PAGE_COUNT >= MINIGAME_RPS_best_of){
            SCENE_CURRENT_PAGE = MINIGAME_RPS_final_page;
        }
        else {
            SCENE_CURRENT_PAGE = MINIGAME_RPS_attract;
        }
        
    }
}

// TODO docstring
void MINIGAME_RPS_finalScoreFrame(void){
    int row, col;
    if (SCENE_FRAME == 0) { // In such an event, we need to "blank" the frame directives and lines
            for (row = 0; row<PIXELS_Y/FONT_SIZE_FLOOR_Y; row++){
                for (col=0; col<PIXELS_X/FONT_SIZE_FLOOR_X; col++){
                    DISPLAY_FRAME.frame[row].directives[col] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                    DISPLAY_FRAME.frame[row].line[col]=' ';
                }
            }
        if (MINIGAME_RPS_score >= 0) {
            GAME_applyHungerFun(0, MINIGAME_RPS_prize);
            strcpy(DISPLAY_FRAME.frame[0].line, "You Win!      ");  //TODO placeholder only
        }
        else {
            strcpy(DISPLAY_FRAME.frame[0].line, "You lost...   "); // TODO placeholder only
        }
    }

    SCENE_FRAME++;
    if (SCENE_FRAME >= 6) {
        SCENE_EXIT_FLAG = 1;
    }
}

// This is the public function of the minigame. Since it's purely icon-based there are no localization strings to load,
// meaning this can be made more simply
void SCENE_RockPaperScissors(void){
    if (SCENE_PAGE_COUNT == 0){
        MINIGAME_RPS_score = 0;
    }
    if (SCENE_FRAME == 0) {
        MINIGAME_RPS_choice = MINIGAME_RPS_choiceNotMade; // By Default
        MINIGAME_RPS_handleInputs(); // Will override a MINIGAME_RPS_choice if chosen
    }
    switch (SCENE_CURRENT_PAGE){
        case MINIGAME_RPS_attract :
            MINIGAME_RPS_attractFrame();
            break;
        case MINIGAME_RPS_eval :
            MINIGAME_RPS_evaluateFrame();
            break;
        case MINIGAME_RPS_final_page : // #TODO resume!
            MINIGAME_RPS_finalScoreFrame(); // TODO implement correctly
            break;
    }
    DISPLAY_updatesOnly_enhanced(&DISPLAY_FRAME, MODE_GAME); // Updating the LCD is slow, please update just the parts that matter, and use the MENU layout.
    if (SCENE_EXIT_FLAG) {
        SCENE_ACT = SCENEADDR_minigames_menu;
        SCENE_EXIT_FLAG = 0;
        SCENE_FRAME = 0;
    }
}
