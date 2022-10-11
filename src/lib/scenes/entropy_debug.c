//***************************************************************************************
//  PETI FOSS Virtual Pet - Debug Information for Entropy Module
//
//  Displays the Initial and Current Random Values
//
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  September 2022
//***************************************************************************************

#include "lib/display/display.h"
#include "lib/hwinit/hwinit.h"
#include "lib/game/game_manager.h"
#include "lib/game/evo_data.h"
#include "lib/game/entropy.h"
#include "driverlib.h"
#include <msp430.h>
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "lib/locales/enCA_strings.h"
#include "scenes_manager.h"
#include "menu_generator.h"
#include "stage_selector.h"

unsigned int rngdbg_exiting = false;


// We need our own scene-specific input handling, which will probably almost always be the case for scenes.
// In this case, we can really only exit
void RNGDBG_handleInputs(){
    if (buttons_state & button_d_toggle){  //D moves back a value to allow for corrections or, if at the thousands of the year, cancels without saving.
        rngdbg_exiting = true;
        SCENE_ACT = SCENEADDR_debug_menu; // We are cancelling, get me outta here.
        buttons_state ^= button_d_toggle;
        }
}

// State controller based on the inputs to control which page gets drawn.
void RNGDBG_computeNextFrame(char* header, char * init_seed_string, char * current_state_string){
    int row, col;
    //Display the header row and the static icon rows.
    //These rows are never highlighted and will only refresh when the FORCE_REFRESH bit is set, which is fine.
    strcpy(DISPLAY_FRAME.frame[0].line, header);
    strcpy(DISPLAY_FRAME.frame[1].line, init_seed_string);
    sprintf(WORK_STRING, "%g", RNG_session_seed);
    strcpy(DISPLAY_FRAME.frame[2].line, WORK_STRING); // here is where my formatting logic would go, if I had one.
    strcpy(DISPLAY_FRAME.frame[7].line, current_state_string);
    sprintf(WORK_STRING, "%f", RNG_drawFloat());
    strcpy(DISPLAY_FRAME.frame[8].line, WORK_STRING);

    for (row = 0; row<PIXELS_Y/FONT_SIZE_FLOOR_Y; row++){
        for (col=0; col<PIXELS_X/FONT_SIZE_FLOOR_X; col++){
            DISPLAY_FRAME.frame[row].directives[col] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
        }
    }
}

// This is the public function of the menu generator module. By calling this with the following arguments you are drawing a menu to screen.
//      target_LSTRING_HEADER: a char* pseudostring, ideally from your locale file, that gives the menu title.
//      target_LSTRING_SEED: a char* pseudostring, ideally from your locale file, that gives the name for the intial seed.
//      target_LSTRING_CURRENT: a char* pseudostring, ideally from your locale file, that gives the name for the current RNG state.
void SCENE_DebugRNG(char * target_LSTRING_HEADER, char * target_LSTRING_SEED, char * target_LSTRING_CURRENT){
    RNGDBG_handleInputs();
    RNGDBG_computeNextFrame(target_LSTRING_HEADER, target_LSTRING_SEED, target_LSTRING_CURRENT);
    DISPLAY_updatesOnly_enhanced(&DISPLAY_FRAME, MODE_MENU); // Updating the LCD is slow, please update just the parts that matter, and use the MENU layout.
    if (rngdbg_exiting){ // The user has asked to leave.
        rngdbg_exiting = false; // The player can come back to this menu, so we need to reset this.
    }
}
