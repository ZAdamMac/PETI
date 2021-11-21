//***************************************************************************************
//  PETI FOSS Virtual Pet - Pet Status Display
//
//  Simplistic Menu/Stats Page to show a user-friendly interpretation of the State object
//
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  May 2021
//***************************************************************************************

#include "lib/display/display.h"
#include "lib/hwinit/hwinit.h"
#include "driverlib.h"
#include <msp430.h>
#include <string.h>
#include "scenes_manager.h"
#include "main.h"
#include "lib/locales/enCA_strings.h"
#include "lib/game/game_manager.h"

#define NEXT_SCENE SCENEADDR_main_game  // We want this scene to exit to the main game
#define STATMENU_lastpage 2             // Index of the final page on the menu, for looping reasons.

int statmenu_page = 0;
unsigned int statmenu_exiting = false;


// We need our own scene-specific input handling, which will probably almost always be the case for scenes.
// In this case, we can really only go up, down, or exit
void STATMENU_handleInputs(void){
    if (buttons_state & button_a_toggle){  //A scrolls up by 1
        statmenu_page -= 1;
        if (statmenu_page < 0){
            statmenu_page = STATMENU_lastpage;
        }
        buttons_state ^= button_a_toggle;
    }
    if (buttons_state & button_b_toggle){  //B scrolls down by 1
        statmenu_page += 1;
        if (statmenu_page > STATMENU_lastpage){
            statmenu_page = 0;
        }
        buttons_state ^= button_b_toggle;
    }
    if (buttons_state & button_c_toggle){  //C exits the menu outright
        statmenu_exiting = true;
        buttons_state ^= button_c_toggle;
    }
    if (buttons_state & button_d_toggle){  //D also exits the menu outright.
        statmenu_exiting = true;
        buttons_state ^= button_d_toggle;
        }
}


//Render a graphical bar given a numerator and a position index for the bar.
//The denominator is assumed to be PIXELS_X/8-1, as this uses the Font8x12 display font.
char * STATMENU_renderBar(unsigned int numerator){
   unsigned int text_index;
    if (numerator < 1){ //Check for the alert condition
        WORK_STRING[0] = 20; // If the score is low enough, display this special warning icon
        WORK_STRING[1] = 14; // And an empty left edge for the bar.
    }
    else {
        WORK_STRING[0] = ' '; // Conversely, show no icon
        WORK_STRING[1] = 17; // And the filled left edge.
    }
    for (text_index = 2; text_index <= numerator; ++text_index){ // Iterate over the bar for count numerator places, drawing filled bar sections.
        WORK_STRING[text_index] = 18;
    }
    while (text_index < (PIXELS_X/8-1)){ // Then, fill the rest of the bar with empty spots.
        WORK_STRING[text_index] = 15;
        text_index++;
    }
    if (numerator == (PIXELS_X/8-1)){ // If the line is maximized, draw a filled right bar edge.
        WORK_STRING[15] = 19;
    }
    else { // Else, show empty.
        WORK_STRING[15] = 16;
    }
    return &WORK_STRING;
}

// A dummy function to render the weight of the pet given State.health_byte.
char * STATMENU_renderWeight(void){
    return "Not Implemented!";
}

// A dummy function to render special status flags given State.health_byte
char * STATMENU_renderSpecial(void){
    return "Not Implemented!";
}

// Render the age of the pet in days and display it tidly as a line of text.
char * STATMENU_renderAge(void){
    WORK_STRING[0] = ' ';
    WORK_STRING[1] = ' ';
    WORK_STRING[2] = StateMachine.AGE > 999 ? DISPLAY_nthDigit(3, StateMachine.AGE) : ' ';
    WORK_STRING[3] = StateMachine.AGE > 99 ? DISPLAY_nthDigit(2, StateMachine.AGE) : ' ';
    WORK_STRING[4] = StateMachine.AGE > 9 ? DISPLAY_nthDigit(1, StateMachine.AGE) : ' ';
    WORK_STRING[5] = DISPLAY_nthDigit(0, StateMachine.AGE); // We always wish to see the 0th character/tens digit.
    WORK_STRING[6] = ' ';
    WORK_STRING[7] = 'D'; //Suboptimal for localizing. Find general solution.
    WORK_STRING[8] = 'a';
    WORK_STRING[9] = 'y';
    WORK_STRING[10] = 's';
    WORK_STRING[11] = ' ';
    WORK_STRING[12] = ' ';
    WORK_STRING[13] = ' ';
    WORK_STRING[14] = ' ';
    WORK_STRING[15] = ' ';
    return &WORK_STRING;
}

//Draws the first/top page of the status menu, which displays hunger and fun scores.
// This is page index 0
void STATMENU_renderHungerFunPage(void){
    unsigned int hunger; unsigned int fun;
    hunger = (StateMachine.HUNGER_FUN >> 4 & 0xF); // HUNGER is the upper nibble of the byte
    fun = (StateMachine.HUNGER_FUN & 0xF); //FUN is the lower nibble.
    strcpy(DISPLAY_FRAME.frame[2].line, LSTRING_HUNGER);
    strncpy(DISPLAY_FRAME.frame[3].line, STATMENU_renderBar(hunger), (PIXELS_X/FONT_SIZE_FLOOR_X));
    strcpy(DISPLAY_FRAME.frame[6].line, LSTRING_FUN);
    strncpy(DISPLAY_FRAME.frame[7].line, STATMENU_renderBar(fun),(PIXELS_X/FONT_SIZE_FLOOR_X));
}


// Display the Disicpline and Weight page of the menu (page index 1)
void STATMENU_renderDisciplineWeightPage(void){
    unsigned int discipline;
    discipline = (StateMachine.DISCIPLINE / 0xFF) * ((PIXELS_X/8)-1); // We don't want to hurt the bar system, so divide by the appropriate factor
    strcpy(DISPLAY_FRAME.frame[2].line, LSTRING_DISCIPLINE);
    strcpy(DISPLAY_FRAME.frame[3].line, STATMENU_renderBar(discipline));
    strcpy(DISPLAY_FRAME.frame[6].line, LSTRING_WEIGHT);
    strcpy(DISPLAY_FRAME.frame[7].line, STATMENU_renderWeight());
}


// Display the special status and age indicators. Page index 2
void STATMENU_renderSpecialPage(void){
    strcpy(DISPLAY_FRAME.frame[2].line, LSTRING_AGE);
    strcpy(DISPLAY_FRAME.frame[3].line, STATMENU_renderAge()); // TODO: Proof we need STRNCPY
    strcpy(DISPLAY_FRAME.frame[6].line, LSTRING_SPECIAL);
    strcpy(DISPLAY_FRAME.frame[7].line, STATMENU_renderSpecial());
}



// State controller based on the inputs to control which page gets drawn.
void STATMENU_computeNextFrame(void){
    strcpy(DISPLAY_FRAME.frame[0].line, LSTRING_STATUS_HEADER);
    strcpy(DISPLAY_FRAME.frame[1].line, "\x06               ");
    strcpy(DISPLAY_FRAME.frame[4].line, "                ");
    strcpy(DISPLAY_FRAME.frame[5].line, "                ");
    strcpy(DISPLAY_FRAME.frame[8].line, "                ");
    strcpy(DISPLAY_FRAME.frame[9].line, "\x01              \x15");
    switch (statmenu_page){
    case 0:
        STATMENU_renderHungerFunPage();
        break;
    case 1:
        STATMENU_renderDisciplineWeightPage();
        break;
    case 2:
        STATMENU_renderSpecialPage();
    }
}

// Runtime/wrapper function for the scene. When called, directs the LCD to draw a page from the status menu.
void SCENE_status_menu(void){
    STATMENU_handleInputs(); // What did the user just do?
    STATMENU_computeNextFrame();  // How do we show them that change?
    DISPLAY_updatesOnly_enhanced(&DISPLAY_FRAME, MODE_MENU); // Updating the LCD is slow, please update just the parts that matter, and use the MENU layout.
    if (statmenu_exiting){ // The user has asked to leave.
        SCENE_ACT = NEXT_SCENE; // We need to go back to wherever this leads, usually to the main game screne.
        statmenu_exiting = false; // The player can come back to this menu, so we need to reset this.
        statmenu_page = 0;
    }
}
