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
#include "scenes_manager.h"
#include "main.h"
#include "lib/locales/enCA_strings.h"
#include "lib/game/game_manager.h"

#define NEXT_SCENE SCENEADDR_main_game // We want this scene to exit to the main game

DisplayFrame STATMENU_Frame;

int statmenu_page = 0;
unsigned int statmenu_exiting = false;
char bars[3][17];                   // An array to hold the status
char str_weight[17];                // an array to hold the weight output string
char str_age[17];                   // an array to hold the height output string
char str_special[17];               //an array to hold the special output string.

// We need our own scene-specific input handling, which will probably almost always be the case for scenes.
// In this case, we can really only go up, down, or exit
void STATMENU_handleInputs(void){
    if (buttons_state & button_a_toggle){  //A scrolls up by 1
        statmenu_page -= 1;
        if (statmenu_page < 0){
            statmenu_page = 2;
        }
        buttons_state ^= button_a_toggle;
    }
    if (buttons_state & button_b_toggle){  //B scrolls down by 1
        statmenu_page += 1;
        if (statmenu_page > 2){
            statmenu_page = 0;
        }
        buttons_state ^= button_b_toggle;
    }
    if (buttons_state & button_c_toggle){  //C accepts this value by moving onto the next frame, OR hits the set requirement if we're on that index.
        statmenu_exiting = true;
        buttons_state ^= button_c_toggle;
    }
    if (buttons_state & button_d_toggle){  //D moves back a value to allow for corrections or, if at the thousands of the year, cancels without saving.
        statmenu_exiting = true;
        buttons_state ^= button_d_toggle;
        }
}

char * STATMENU_renderBar(unsigned int numerator, int which_bar){
   unsigned int text_index;
    if (numerator < 1){ // we need the special alert icon
        bars[which_bar][0] = 20;
        bars[which_bar][1] = 14;
    }
    else {
        bars[which_bar][0] = ' ';
        bars[which_bar][1] = 17;
    }
    for (text_index = 2; text_index <= numerator; ++text_index){
        bars[which_bar][text_index] = 18;
    }
    while (text_index < 15){
        bars[which_bar][text_index] = 15;
        text_index++;
    }
    if (numerator == 15){
        bars[which_bar][15] = 19;
    }
    else {
        bars[which_bar][15] = 16;
    }
    return bars[which_bar];
}

char * STATMENU_renderWeight(void){
    return "Not Implemented!";
}

char * STATMENU_renderSpecial(void){
    return "Not Implemented!";
}

char * STATMENU_renderAge(void){
    str_age[0] = ' ';
    str_age[1] = ' ';
    str_age[2] = DISPLAY_nthDigit(3, StateMachine.AGE); // Will this display 0s if no leading digit?
    str_age[3] = DISPLAY_nthDigit(2, StateMachine.AGE);
    str_age[4] = DISPLAY_nthDigit(1, StateMachine.AGE);
    str_age[5] = DISPLAY_nthDigit(0, StateMachine.AGE);
    str_age[6] = ' ';
    str_age[7] = 'D'; //Suboptimal for localizing. Find general solution.
    str_age[8] = 'a';
    str_age[9] = 'y';
    str_age[10] = 's';
    str_age[11] = ' ';
    str_age[12] = ' ';
    str_age[13] = ' ';
    str_age[14] = ' ';
    str_age[15] = ' ';
    return str_age;
}

//Draws the first/top page of the status menu, which displays hunger and fun scores.
void STATMENU_renderHungerFunPage(void){
    unsigned int hunger; unsigned int fun;
    hunger = (StateMachine.HUNGER_FUN >> 4 & 0xF); // HUNGER is the upper nibble of the byte
    fun = (StateMachine.HUNGER_FUN & 0xF); //FUN is the lower nibble.
    STATMENU_Frame.line2 = LSTRING_HUNGER;
    STATMENU_Frame.line3 = STATMENU_renderBar(hunger, 0);
    STATMENU_Frame.line6 = LSTRING_FUN;
    STATMENU_Frame.line7 = STATMENU_renderBar(fun, 1);
    STATMENU_Frame.refresh_L2 = true;
    STATMENU_Frame.refresh_L3 = true;
    STATMENU_Frame.refresh_L6 = true;
    STATMENU_Frame.refresh_L7 = true;
}

void STATMENU_renderDisciplineWeightPage(void){
    unsigned int discipline;
    discipline = (StateMachine.DISCIPLINE / 0xFF) * 15; // We don't want to hurt the bar system, do we?
    STATMENU_Frame.line2 = LSTRING_DISCIPLINE;
    STATMENU_Frame.line3 = STATMENU_renderBar(StateMachine.DISCIPLINE, 2);
    STATMENU_Frame.line6 = LSTRING_WEIGHT;
    STATMENU_Frame.line7 = STATMENU_renderWeight();
    STATMENU_Frame.refresh_L2 = true;
    STATMENU_Frame.refresh_L3 = true;
    STATMENU_Frame.refresh_L6 = true;
    STATMENU_Frame.refresh_L7 = true;
}

void STATMENU_renderSpecialPage(void){
    STATMENU_Frame.line2 = LSTRING_AGE;
    STATMENU_Frame.line3 = STATMENU_renderAge();
    STATMENU_Frame.line6 = LSTRING_SPECIAL;
    STATMENU_Frame.line7 = STATMENU_renderSpecial();
    STATMENU_Frame.refresh_L2 = true;
    STATMENU_Frame.refresh_L3 = true;
    STATMENU_Frame.refresh_L6 = true;
    STATMENU_Frame.refresh_L7 = true;
}



// State controller based on the inputs to control which page gets drawn.
void STATMENU_computeNextFrame(void){
    STATMENU_Frame.line0 = LSTRING_STATUS_HEADER;
    STATMENU_Frame.line1 = "\x06               ";
    STATMENU_Frame.line4 = "                ";
    STATMENU_Frame.line5 = "                ";
    STATMENU_Frame.line8 = "                ";
    STATMENU_Frame.line9 = "\x01              \x15";
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
    DISPLAY_updatesOnly(STATMENU_Frame, MODE_MENU); // Updating the LCD is slow, please update just the parts that matter, and use the MENU layout.
    if (statmenu_exiting){ // The user has asked to leave.
        SCENE_ACT = NEXT_SCENE; // We need to go back to wherever this leads, usually to the main game screne.
        statmenu_exiting = false; // The player can come back to this menu, so we need to reset this.
        statmenu_page = 0;
    }
}
