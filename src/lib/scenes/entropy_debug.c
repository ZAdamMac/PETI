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
#include "lib/hwinit/human_input.h"
#include "lib/hwinit/accessory_spi.h"

// We need our own scene-specific input handling, which will probably almost always be the case for scenes.
// In this case, we can really only exit
void RNGDBG(void){
    if (HID_input_events_queue[0] != BUTTON_NO_PRESS){ // If any button was pressed at all, skip to the end.
        SCENE_ACT = SCENEADDR_debug_menu;
        HID_input_events_queue[0] = BUTTON_NO_PRESS;
    }
}

// State controller based on the inputs to control which page gets drawn.
void RNGDBG_computeNextFrame(char* header, char * init_seed_string, char * current_state_string){
    unsigned int row, col, result_normalled; float this_result;
    this_result = RNG_drawFloat();
    result_normalled = this_result * 0xFF;
    ACCESSORY_spiSendByte(result_normalled);
    //Display the header row and the static icon rows.
    //These rows are never highlighted and will only refresh when the FORCE_REFRESH bit is set, which is fine.
    strcpy(DISPLAY_FRAME.frame[0].line, header);
    strcpy(DISPLAY_FRAME.frame[1].line, init_seed_string);
    sprintf(WORK_STRING, "%d", RNG_session_seed);
    strcpy(DISPLAY_FRAME.frame[2].line, WORK_STRING); // here is where my formatting logic would go, if I had one.
    strcpy(DISPLAY_FRAME.frame[7].line, current_state_string);
    sprintf(WORK_STRING, "%f", this_result);
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
    RNGDBG();
    DISPLAY_blankFrame();
    RNGDBG_computeNextFrame(target_LSTRING_HEADER, target_LSTRING_SEED, target_LSTRING_CURRENT);
    DISPLAY_updatesOnly_enhanced(&DISPLAY_FRAME, MODE_MENU); // Updating the LCD is slow, please update just the parts that matter, and use the MENU layout.
    if (SCENE_EXIT_FLAG){ // The user has asked to leave.
        SCENE_EXIT_FLAG = 0; // The player can come back to this menu, so we need to reset this.
    }
}
