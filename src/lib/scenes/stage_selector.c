//***************************************************************************************
//  PETI FOSS Virtual Pet - Stage Select Override
//
//  Generalized stage selection Menu
//
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  February 2022
//***************************************************************************************

#include "lib/display/display.h"
#include "lib/hwinit/hwinit.h"
#include "lib/hwinit/human_input.h"
#include "lib/game/game_manager.h"
#include "lib/game/evo_data.h"
#include "driverlib/MSP430FR5xx_6xx/driverlib.h"
#include <msp430.h>
#include <string.h>
#include "main.h"
#include "lib/locales/enCA_strings.h"
#include "scenes_manager.h"
#include "menu_generator.h"
#include "stage_selector.h"

#define STAGESEL_active_lines 7                     // Defines the number of lines usable to display menu items on each page.
#define STAGESEL_width_buffered ((PIXELS_X/8)+1)    // Mathmatically ties the buffered menu widht to display.h's PIXELS_X

// Computes the total number of pages needed based on the value of STAGESEL_active_lines and the argued-in count of menu options
int STAGESEL_computePagination(int count_opts){
    int pages_needed;
    if (count_opts <= STAGESEL_active_lines){ // The division operation would round to 0, which is impossible to render!
        pages_needed = 1;
    }
    else if (count_opts % STAGESEL_active_lines == 0){ // The division is perfect and there are no remainders to render on an extra page
        pages_needed = count_opts/STAGESEL_active_lines;
    }
    else{
        pages_needed = count_opts/STAGESEL_active_lines + 1; // The division would have a remainder, and we need an extra page to account for it.
    }
    return pages_needed;
}

// We need our own scene-specific input handling, which will probably almost always be the case for scenes.
// In this case, we can really only go up, down, or exit
void STAGESEL_handleInputs(Stage *target_SARRAY, int target_count_opts){
    int index; unsigned int selection;
    int i;
    unsigned int this_event;
    for (i =0; i <= HID_input_events_queue_depth; ++i) // For every event in the event queue;
    {
        this_event = HID_input_events_queue[i]; //fetch the event.
        switch (this_event){
            case(BUTTON_A_PRESS): // Scroll up the list but one per keypress.
	       SCENE_CURSOR_POS -= 1;
	       if (SCENE_CURSOR_POS < 0){
		    SCENE_CURSOR_POS = STAGESEL_active_lines;
		    SCENE_CURRENT_PAGE -= 1;
		    if (SCENE_CURRENT_PAGE < 0){
		        SCENE_CURRENT_PAGE = SCENE_PAGE_COUNT - 1;
		    }
	       }
            	break;
            case(BUTTON_B_PRESS): // scroll down the list by one per keypress
		SCENE_CURSOR_POS += 1;
		if (SCENE_CURSOR_POS >= STAGESEL_active_lines){
		    SCENE_CURRENT_PAGE += 1;
		    SCENE_CURSOR_POS = 0;
		    if (SCENE_CURRENT_PAGE >= SCENE_PAGE_COUNT){
		        SCENE_CURRENT_PAGE = 0;
		    }
		}
                break;
            case(BUTTON_C_PRESS): //Make a selection of the current line.
		SCENE_EXIT_FLAG = true;
		if (SCENE_CURSOR_POS + (SCENE_CURRENT_PAGE*STAGESEL_active_lines) < target_count_opts){
		    selection = SCENE_CURSOR_POS+(SCENE_CURRENT_PAGE*7);
		    StateMachine.STAGE_ID = target_SARRAY[selection].stage_id;
		    SCENE_ACT = SCENEADDR_main_game;
		    SCENE_EXIT_FLAG = true;
		}
                break;
            case(BUTTON_D_PRESS): // exit back to the main game page
		SCENE_EXIT_FLAG = true;
		SCENE_ACT = SCENEADDR_debug_menu; // We are cancelling, get me outta here.
                break;
        }
        HID_input_events_queue[i] = BUTTON_NO_PRESS;
    };
    HID_input_events_queue_depth = 0;
}

// Returns the text from `options` for the current cursor position, assuming in fact it exists.
char* STAGESEL_computeLine(int line_number, char ** options, int count_opts){
    if (line_number + (SCENE_CURRENT_PAGE*STAGESEL_active_lines) < count_opts){ // The requested index is in-bound and we can do this.
        return options[line_number + (SCENE_CURRENT_PAGE*STAGESEL_active_lines)];
    }
    else {
        return "";
    }
}

// State controller based on the inputs to control which page gets drawn.
void STAGESEL_computeNextFrame(char* header, char * options, int count_opts){
    int row, col;
    //Display the header row and the static icon rows.
    //These rows are never highlighted and will only refresh when the FORCE_REFRESH bit is set, which is fine.
    strcpy(DISPLAY_FRAME.frame[0].line, header);
    strcpy(DISPLAY_FRAME.frame[1].line, "\x06              \x16");
    strcpy(DISPLAY_FRAME.frame[9].line, "\x01              \x15");

    for (row = 0; row<PIXELS_Y/FONT_SIZE_FLOOR_Y; row++){
        for (col=0; col<PIXELS_X/FONT_SIZE_FLOOR_X; col++){
            DISPLAY_FRAME.frame[row].directives[col] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
        }
    }


    //Iteratively set the remaining rows.
    for (row=0; row<STAGESEL_active_lines; row++){
        strcpy(DISPLAY_FRAME.frame[2+row].line, STAGESEL_computeLine(row, options, count_opts));
        if (row == SCENE_CURSOR_POS){ // In this case, print.
            for (col=0; col<PIXELS_X/FONT_SIZE_FLOOR_X; col++){
                DISPLAY_FRAME.frame[2+row].directives[col] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
                }
            }
        }
    }

// This is the public function of the menu generator module. By calling this with the following arguments you are drawing a menu to screen.
//      target_LSTRING_HEADER: a char* pseudostring, ideally from your locale file, that gives the menu title.
//      target_LARRAY: a char** array of char* pseudostrings, ideally from the locale file, giving the names of each menu option.
//      target_SARRAY: a Food ** array of pointers to functions. This must be in the same order as target_LARRAY
//      target_count_opts: an integer describing the number of items (not the max index!) of the two arrays above.
void SCENE_StageSelect(char * target_LSTRING_HEADER, char ** target_LARRAY, Stage * target_SARRAY, int target_count_opts){
    SCENE_PAGE_COUNT = STAGESEL_computePagination(target_count_opts);
    STAGESEL_handleInputs(target_SARRAY, target_count_opts);
    STAGESEL_computeNextFrame(target_LSTRING_HEADER, target_LARRAY, target_count_opts);
    DISPLAY_updatesOnly_enhanced(&DISPLAY_FRAME, MODE_MENU); // Updating the LCD is slow, please update just the parts that matter, and use the MENU layout.
    if (SCENE_EXIT_FLAG){ // The user has asked to leave.
        SCENE_EXIT_FLAG = false; // The player can come back to this menu, so we need to reset this.
        SCENE_CURRENT_PAGE = 0;
        SCENE_CURSOR_POS = 0;
    }
}
