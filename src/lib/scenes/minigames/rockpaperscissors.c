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

enum options {rock, paper, scissors, cancel, not_made};
enum options choice;

// We need our own scene-specific input handling, which will probably almost always be the case for scenes.
void MINIGAME_handleInputs(void){
    int i;
    unsigned int this_event;
    for (i =0; i <= HID_input_events_queue_depth; ++i) // For every event in the event queue;
    {
        this_event = HID_input_events_queue[i]; //fetch the event.
        switch (this_event){
            case(BUTTON_A_PRESS): // Select for Rock
		        choice = rock;
            	break;
            case(BUTTON_B_PRESS): // Select for Paper
		        choice = paper;
                break;
            case(BUTTON_C_PRESS): //Select for Scissors
		        choice = scissors;
                break;
            case(BUTTON_D_PRESS): // Exit
	            SCENE_ACT = SCENEADDR_main_game;
                break;
        }
        HID_input_events_queue[i] = BUTTON_NO_PRESS;
    };
    HID_input_events_queue_depth = 0;
}

//
void MINIGAME_computeNextFrame(void){
    int active_line;
    int row, col;
    Stage active_species = EVO_metaStruct[StateMachine.STAGE_ID];
    strcpy(DISPLAY_FRAME.frame[0].line, "\xF7      \xF5");
    strcpy(DISPLAY_FRAME.frame[7].line, "\xF6      \xF0");

    //placeholder logic
    //TODO: Replace with the real stuff
    for (row = 0; row<PIXELS_Y/FONT_SIZE_FLOOR_Y; row++){
        for (col=0; col<PIXELS_X/FONT_SIZE_FLOOR_X; col++){
            DISPLAY_FRAME.frame[row].directives[col] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
        }
    }

    strcpy(DISPLAY_FRAME.frame[1].line, "   \xF2\xF1\xF2   ");
    strcpy(DISPLAY_FRAME.frame[2].line, "   \xEF\xF7\xEF   ");
    strcpy(DISPLAY_FRAME.frame[3].line, "   \xF3\xF4\xF3   ");
    DISPLAY_FRAME.frame[1].directives[5] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
    DISPLAY_FRAME.frame[2].directives[5] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
    DISPLAY_FRAME.frame[3].directives[5] = FONT_ADDR_0 + DIRECTIVE_REVERSED;

}

// This is the public function of the minigame. Since it's purely icon-based there are no localization strings to load,
// meaning this can be made more simply
void SCENE_RockPaperScissors(void){
    choice = not_made; // By Default
    MINIGAME_handleInputs(); // Will override a choice if chosen
    MINIGAME_computeNextFrame();
    DISPLAY_updatesOnly_enhanced(&DISPLAY_FRAME, MODE_MINIGAME); // Updating the LCD is slow, please update just the parts that matter, and use the MENU layout.
}
