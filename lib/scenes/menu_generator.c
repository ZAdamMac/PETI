//***************************************************************************************
//  PETI FOSS Virtual Pet - Menu Generator
//
//  Generalized menu display code for text-oriented menus in game.
//
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  November 2021
//***************************************************************************************

#include "lib/display/display.h"
#include "lib/hwinit/hwinit.h"
#include "driverlib.h"
#include <msp430.h>
#include "main.h"
#include "lib/locales/enCA_strings.h"
#include "scenes_manager.h"
#include "menu_generator.h"

#define MENU_active_lines 7                     // Defines the number of lines usable to display menu items on each page.
#define MENU_width_buffered ((PIXELS_X/8)+1)    // Mathmatically ties the buffered menu widht to display.h's PIXELS_X

DisplayFrame MENU_Frame;

int actmenu_page = 0;
int actmenu_cursor = 0;
int MENU_page_count = 0;
unsigned int actmenu_exiting = false;
char bars[MENU_active_lines][MENU_width_buffered];                   // An array to hold the status
char bars_refresh[MENU_active_lines];                                // To help compute dynamic refreshes.
char bars_directive[MENU_active_lines][MENU_width_buffered];         // To help determine highlighting.



// Computes the total number of pages needed based on the value of MENU_active_lines and the argued-in count of menu options
int MENU_computePagination(int count_opts){
    int pages_needed;
    if (count_opts <= MENU_active_lines){ // The division operation would round to 0, which is impossible to render!
        pages_needed = 1;
    }
    else if (count_opts % MENU_active_lines == 0){ // The division is perfect and there are no remainders to render on an extra page
        pages_needed = count_opts/MENU_active_lines;
    }
    else{
        pages_needed = count_opts/MENU_active_lines + 1; // The division would have a remainder, and we need an extra page to account for it.
    }
    return pages_needed;
}


// We need our own scene-specific input handling, which will probably almost always be the case for scenes.
// In this case, we can really only go up, down, or exit
void MENU_handleInputs(voidFuncPointer *target_MARRAY, int target_count_opts){
    int index; int selection;
    if (buttons_state & button_a_toggle){  //A scrolls up by 1
       actmenu_cursor -= 1;
       if (actmenu_cursor < 0){
            actmenu_cursor = MENU_active_lines;
            actmenu_page -= 1;
            if (actmenu_page < 0){
                actmenu_page = MENU_page_count - 1;
                for (index=0; index<(MENU_active_lines+1); index++){
                    bars_refresh[index] = true; // If the page changes, just redraw all 7 lines.
                }
            }
       }
            buttons_state ^= button_a_toggle;
            bars_refresh[actmenu_cursor] = true; // We need to redraw the current line, as well as the line on either side.
            bars_refresh[(actmenu_cursor + 1) % 7] = true;
            bars_refresh[(actmenu_cursor - 1) % 7] = true;
    }
    if (buttons_state & button_b_toggle){  //B scrolls down by 1
        actmenu_cursor += 1;
        if (actmenu_cursor >= MENU_active_lines){
            actmenu_page += 1;
            actmenu_cursor = 0;
            if (actmenu_page >= MENU_page_count){
                actmenu_page = 0;
                for (index=0; index<(MENU_active_lines+1); index++){
                    bars_refresh[index] = true; // If the page changes, just redraw all 7 lines.
                }
            }
        }
        buttons_state ^= button_b_toggle;
        bars_refresh[actmenu_cursor] = true;
        bars_refresh[(actmenu_cursor + 1) % (MENU_active_lines+1)] = true;
        bars_refresh[(actmenu_cursor - 1) % (MENU_active_lines+1)] = true;
    }
    if (buttons_state & button_c_toggle){  //C accepts this value by moving onto the next frame, OR hits the set requirement if we're on that index.
        actmenu_exiting = true;
        if (actmenu_cursor + (actmenu_page*MENU_active_lines) < target_count_opts){
            selection = actmenu_cursor+(actmenu_page*7);
            target_MARRAY[selection]();
        }
        buttons_state ^= button_c_toggle;
    }
    if (buttons_state & button_d_toggle){  //D moves back a value to allow for corrections or, if at the thousands of the year, cancels without saving.
        actmenu_exiting = true;
        SCENE_ACT = SCENEADDR_main_game; // We are cancelling, get me outta here.
        buttons_state ^= button_d_toggle;
        }
}


// Returns the text from `options` for the current cursor position, assuming in fact it exists.
char* MENU_computeLine(int line_number, char ** options, int count_opts){
    if (line_number + (actmenu_page*MENU_active_lines) < count_opts){ // The requested index is in-bound and we can do this.
        return options[line_number + (actmenu_page*MENU_active_lines)];
    }
    else {
        return "";
    }
}

// State controller based on the inputs to control which page gets drawn.
void MENU_computeNextFrame(char* header, char * options, int count_opts){
    int x_index;
    MENU_Frame.line0 = header;
    MENU_Frame.line1 = "\x06              \x16";

    for (x_index=0; x_index<MENU_width_buffered; x_index++){
        bars_directive[actmenu_cursor][x_index] = '1';
    }

    // The section below is the reason for MENU_active_lines. If adding or removing lines that must be changed.
    MENU_Frame.line2 = MENU_computeLine(0, options, count_opts);
    MENU_Frame.refresh_L2 = bars_refresh[0];
    MENU_Frame.directive_L2 = bars_directive[0];
    MENU_Frame.line3 = MENU_computeLine(1, options, count_opts);
    MENU_Frame.refresh_L3 = bars_refresh[1];
    MENU_Frame.directive_L3 = bars_directive[1];
    MENU_Frame.line4 = MENU_computeLine(2, options, count_opts);
    MENU_Frame.refresh_L4 = bars_refresh[2];
    MENU_Frame.directive_L4 = bars_directive[2];
    MENU_Frame.line5 = MENU_computeLine(3, options, count_opts);
    MENU_Frame.refresh_L5 = bars_refresh[3];
    MENU_Frame.directive_L5 = bars_directive[3];
    MENU_Frame.line6 = MENU_computeLine(4, options, count_opts);
    MENU_Frame.refresh_L6 = bars_refresh[4];
    MENU_Frame.directive_L6 = bars_directive[4];
    MENU_Frame.line7 = MENU_computeLine(5, options, count_opts);
    MENU_Frame.refresh_L7 = bars_refresh[5];
    MENU_Frame.directive_L7 = bars_directive[5];
    MENU_Frame.line8 = MENU_computeLine(6, options, count_opts);
    MENU_Frame.refresh_L8 = bars_refresh[6];
    MENU_Frame.directive_L8 = bars_directive[6];
    MENU_Frame.line9 = "\x01              \x15";
}



// This just loops over all of the bars_refresh ints and the bars_directive "strings" to set them all to defaults.
void MENU_resetBarsets(void){
    int y_index; int x_index;
    for (y_index=0; y_index<=MENU_active_lines; y_index++){
        bars_refresh[y_index] = false;
        for (x_index=0; x_index<MENU_width_buffered; x_index++){
            bars_directive[y_index][x_index] = '0';
        }
    }
}

// This is the public function of the menu generator module. By calling this with the following arguments you are drawing a menu to screen.
//      target_LSTRING_HEADER: a char* pseudostring, ideally from your locale file, that gives the menu title.
//      target_LARRAY: a char** array of char* pseudostrings, ideally from the locale file, giving the names of each menu option.
//      target_MARRAY: a voidFuncPointer ** array of pointers to functions. This must be in the same order as target_LARRAY
//      target_count_opts: an integer describing the number of items (not the max index!) of the two arrays above.
void SCENE_TextMenu(char * target_LSTRING_HEADER, char ** target_LARRAY, voidFuncPointer ** target_MARRAY, int target_count_opts){
    MENU_resetBarsets();
    MENU_page_count = MENU_computePagination(target_count_opts);
    MENU_handleInputs(target_MARRAY, target_count_opts);
    MENU_computeNextFrame(target_LSTRING_HEADER, target_LARRAY, target_count_opts);
    DISPLAY_updatesOnly(MENU_Frame, MODE_MENU); // Updating the LCD is slow, please update just the parts that matter, and use the MENU layout.
    if (actmenu_exiting){ // The user has asked to leave.
        actmenu_exiting = false; // The player can come back to this menu, so we need to reset this.
        actmenu_page = 0;
    }
}
