//***************************************************************************************
//  PETI FOSS Virtual Pet - Save Data Reset Screen
//
//  Detects if a player has existing save data, and prompts them to confirm resetting it
//  if they do.
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  September 2023
//***************************************************************************************

#include <msp430.h>
#include <string.h>
#include "driverlib.h"
#include "scenes_manager.h"
#include "boot_splash.h"
#include "lib/display/display.h"
#include "main.h"
#include "lib/game/game_manager.h"
#include "lib/locales/enCA_strings.h"
#include "lib/hwinit/human_input.h"

#define RESET_cursor_position_affirm 0x01   //SCENE_CURSOR_POS value that corresponds to "yes"

int NEXT_SCENE = SCENEADDR_reset_save;

// We need our own scene-specific input handling, which will probably almost always be the case for scenes.
// In this case, we can really only go up, down, or exit
void RESET_handleInputs(void){
    int i;
    unsigned int this_event;
    for (i =0; i <= HID_input_events_queue_depth; ++i) // For every event in the event queue;
    {
        this_event = HID_input_events_queue[i]; //fetch the event.
        switch (this_event){
            case(BUTTON_A_PRESS): // Scroll up ONE PAGE
		    SCENE_CURSOR_POS -= 1;
		if (SCENE_CURSOR_POS < 0){
		    SCENE_CURSOR_POS = 1;
		}
            	break;
            case(BUTTON_B_PRESS): // scroll down the list by one per keypress
		    SCENE_CURSOR_POS += 1;
		if (SCENE_CURSOR_POS > 1){
		    SCENE_CURSOR_POS = 0;
		}
                break;
            case(BUTTON_C_PRESS): //Exit
		        if (SCENE_CURSOR_POS == RESET_cursor_position_affirm){ // If we're in the affirmative
                    SCENE_EXIT_FLAG = true;
                    GAME_initStateStruct();
                    NEXT_SCENE = SCENEADDR_calendar_menu;
                }
                else {
                    SCENE_EXIT_FLAG = true;
                    NEXT_SCENE = SCENEADDR_calendar_menu;
                }
                break;
            case(BUTTON_D_PRESS): // exit 
	            SCENE_EXIT_FLAG = true;
                NEXT_SCENE = SCENEADDR_calendar_menu;
                break;
        }
        HID_input_events_queue[i] = BUTTON_NO_PRESS;
    };
    HID_input_events_queue_depth = 0;
}


void RESET_prompt(void){
    // For Sanity's Sake, blank the directives out.
    int row; int col;
    for (row = 0; row<PIXELS_Y/FONT_SIZE_FLOOR_Y; row++){
        for (col=0; col<PIXELS_X/FONT_SIZE_FLOOR_X; col++){
            DISPLAY_FRAME.frame[row].directives[col] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
        }
    }

    strcpy(DISPLAY_FRAME.frame[0].line, LSTRING_WARNING);
    strcpy(DISPLAY_FRAME.frame[2].line, LARRAY_RESET_WARNING[0]);  // FUTURE: likely pain point for localization dev.
    strcpy(DISPLAY_FRAME.frame[3].line, LARRAY_RESET_WARNING[1]);
    strcpy(DISPLAY_FRAME.frame[5].line, LSTRING_NO);
    strcpy(DISPLAY_FRAME.frame[6].line, LSTRING_YES); // Putting this on the lower line forces a 2-button reset.
    DISPLAY_FRAME.frame[6].line[0] = 20;
    DISPLAY_FRAME.frame[5+SCENE_CURSOR_POS].line[1] = 22;

    //highlight the line that was actually selected
    for (col=0; col<PIXELS_X/FONT_SIZE_FLOOR_X; col++){
        DISPLAY_FRAME.frame[5+SCENE_CURSOR_POS].directives[col] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
    }
}


void SCENE_reset_menu(void){
    if (StateMachine.INIT != 0){
        SCENE_EXIT_FLAG = false;
        RESET_handleInputs();
        RESET_prompt();
    }
    else {
        SCENE_EXIT_FLAG = true;
        NEXT_SCENE = SCENEADDR_calendar_menu;
        GAME_initStateStruct();
    }
    DISPLAY_updatesOnly_enhanced(&DISPLAY_FRAME, MODE_MENU); // Updating the LCD is slow, please update just the parts that matter, and use the MENU layout.
    if (SCENE_EXIT_FLAG){ // The user has asked to leave.
        SCENE_ACT = NEXT_SCENE; // We need to go back to wherever this leads, usually to the main game screne.
        SCENE_EXIT_FLAG = false; // The player can come back to this menu, so we need to reset this.
        SCENE_CURSOR_POS = 0;
    }
}
