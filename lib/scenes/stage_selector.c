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
#include "lib/game/game_manager.h"
#include "lib/game/evo_data.h"
#include "driverlib.h"
#include <msp430.h>
#include <string.h>
#include "main.h"
#include "lib/locales/enCA_strings.h"
#include "scenes_manager.h"
#include "menu_generator.h"
#include "stage_selector.h"

#define STAGESEL_active_lines 7                     // Defines the number of lines usable to display menu items on each page.
#define STAGESEL_width_buffered ((PIXELS_X/8)+1)    // Mathmatically ties the buffered menu widht to display.h's PIXELS_X

int stagesel_page = 0;
int stagesel_cursor = 0;
int STAGESEL_page_count = 0;
unsigned int stagesel_exiting = false;



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
    if (buttons_state & button_a_toggle){  //A scrolls up by 1
       stagesel_cursor -= 1;
       if (stagesel_cursor < 0){
            stagesel_cursor = STAGESEL_active_lines;
            stagesel_page -= 1;
            if (stagesel_page < 0){
                stagesel_page = STAGESEL_page_count - 1;
            }
       }
            buttons_state ^= button_a_toggle;
    }
    if (buttons_state & button_b_toggle){  //B scrolls down by 1
        stagesel_cursor += 1;
        if (stagesel_cursor >= STAGESEL_active_lines){
            stagesel_page += 1;
            stagesel_cursor = 0;
            if (stagesel_page >= STAGESEL_page_count){
                stagesel_page = 0;
            }
        }
        buttons_state ^= button_b_toggle;
    }
    if (buttons_state & button_c_toggle){  //C accepts this value by moving onto the next frame, OR hits the set requirement if we're on that index.
        stagesel_exiting = true;
        if (stagesel_cursor + (stagesel_page*STAGESEL_active_lines) < target_count_opts){
            selection = stagesel_cursor+(stagesel_page*7);
            StateMachine.STAGE_ID = target_SARRAY[selection].stage_id;
            SCENE_ACT = SCENEADDR_main_game;
            stagesel_exiting = true;
        }
        buttons_state ^= button_c_toggle;
    }
    if (buttons_state & button_d_toggle){  //D moves back a value to allow for corrections or, if at the thousands of the year, cancels without saving.
        stagesel_exiting = true;
        SCENE_ACT = SCENEADDR_debug_menu; // We are cancelling, get me outta here.
        buttons_state ^= button_d_toggle;
        }
}


// Returns the text from `options` for the current cursor position, assuming in fact it exists.
char* STAGESEL_computeLine(int line_number, char ** options, int count_opts){
    if (line_number + (stagesel_page*STAGESEL_active_lines) < count_opts){ // The requested index is in-bound and we can do this.
        return options[line_number + (stagesel_page*STAGESEL_active_lines)];
    }
    else {
        return "";
    }
}

// State controller based on the inputs to control which page gets drawn.
void STAGESEL_computeNextFrame(char* header, char * options, int count_opts){
    int row;
    //Display the header row and the static icon rows.
    //These rows are never highlighted and will only refresh when the FORCE_REFRESH bit is set, which is fine.
    strcpy(DISPLAY_FRAME.frame[0].line, header);
    strcpy(DISPLAY_FRAME.frame[0].directives, "0000000000000000");
    strcpy(DISPLAY_FRAME.frame[1].line, "\x06              \x16");
    strcpy(DISPLAY_FRAME.frame[1].directives, "0000000000000000");
    strcpy(DISPLAY_FRAME.frame[9].line, "\x01              \x15");
    strcpy(DISPLAY_FRAME.frame[9].directives, "0000000000000000");

    //Iteratively set the remaining rows.
    for (row=0; row<STAGESEL_active_lines; row++){
        strcpy(DISPLAY_FRAME.frame[2+row].line, STAGESEL_computeLine(row, options, count_opts));
        if (row == stagesel_cursor){ // In this case, print.
            strcpy(DISPLAY_FRAME.frame[2+row].directives, "1111111111111111");
        }
        else {
            strcpy(DISPLAY_FRAME.frame[2+row].directives, "0000000000000000");
        }
    }

}




// This is the public function of the menu generator module. By calling this with the following arguments you are drawing a menu to screen.
//      target_LSTRING_HEADER: a char* pseudostring, ideally from your locale file, that gives the menu title.
//      target_LARRAY: a char** array of char* pseudostrings, ideally from the locale file, giving the names of each menu option.
//      target_SARRAY: a Food ** array of pointers to functions. This must be in the same order as target_LARRAY
//      target_count_opts: an integer describing the number of items (not the max index!) of the two arrays above.
void SCENE_StageSelect(char * target_LSTRING_HEADER, char ** target_LARRAY, Stage * target_SARRAY, int target_count_opts){
    STAGESEL_page_count = STAGESEL_computePagination(target_count_opts);
    STAGESEL_handleInputs(target_SARRAY, target_count_opts);
    STAGESEL_computeNextFrame(target_LSTRING_HEADER, target_LARRAY, target_count_opts);
    DISPLAY_updatesOnly_enhanced(&DISPLAY_FRAME, MODE_MENU); // Updating the LCD is slow, please update just the parts that matter, and use the MENU layout.
    if (stagesel_exiting){ // The user has asked to leave.
        stagesel_exiting = false; // The player can come back to this menu, so we need to reset this.
        stagesel_page = 0;
        stagesel_cursor = 0;
    }
}
