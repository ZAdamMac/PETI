//***************************************************************************************
//  PETI FOSS Virtual Pet - Enhanced Demo Mode Scene
//
//  Sends the boot splash screen to the display and waits until it is time to exit.
//  Should be deprecated as soon as there's enough game available to test the inputs without it.
//
//
//  Zac Adam-MacEwen (Kensho Security Labs)
//  May 2021
//***************************************************************************************

#include <msp430.h>
#include "driverlib.h"
#include "scenes_manager.h"
#include "lib/game/evo_data.h"
#include "demo_mode.h"
#include "lib/display/display.h"
#include "main.h"
#include "lib/locales/enCA_strings.h"

unsigned char DEMO_stage = 0x01; // We start at 1 because we had to sit through 0 already.
unsigned char DEMO_screen_state = 0x00; // Controls which of several draw frames we're going to use.
unsigned char DEMO_current_frame = 0x00; // Frame value bound to a maximum of 2.

char playfield[3][8];                              //Array of arrays of characters used to set each row of the output frame programmatically rather than manually for tidiness.
char directives_out[9];                         //Array of arrays of characters used for the enhance larged text print function.
unsigned int reversed;
unsigned int inverted;
unsigned int char_tracker;                         //Charts the count we are at in terms of the icon for the species character animation
volatile DisplayFrame current_demo_Frame;
unsigned int touched_this_row;

// This is a cludgy function based on the earlier PETI demos to print the current time.
// It was originally uptime but now directly interrogates RTC_C. It's not implemented cleanly and relies on some fauly logic.
char* Print_CurrentTime(void){
    Calendar currentTime = RTC_C_getCalendarTime(RTC_C_BASE); // Returns the calendar time as a struct.
    bufferText[0] = ' '; // Centering Manually for Lulz.
    bufferText[1] = ' ';
    bufferText[2] = ' ';
    bufferText[3] = ' ';
    bufferText[4] = currentTime.Hours / 16 + '0'; //get the 10s of hours then add the appropriate character offset.
    bufferText[5] = currentTime.Hours % 16 + '0'; //16 is used as the divisor because the BCD format expresses the decimal value in a hex space, thus the need for math.
    bufferText[6] = ':';
    bufferText[7] = currentTime.Minutes / 16 + '0';
    bufferText[8] = currentTime.Minutes % 16 + '0';
    bufferText[9] = ':';
    bufferText[10] = currentTime.Seconds / 16 + '0';
    bufferText[11] = currentTime.Seconds % 16 + '0';
    bufferText[12] = ' ';
    bufferText[13] = ' ';
    bufferText[14] = ' ';
    bufferText[15] = ' ';
    return bufferText;
}

//We also need a simple function for updating the magic string:

void DEMO_handleInputs(void){
    if (buttons_state & button_a_toggle){ // The A button decrements the stage
        buttons_state ^= button_a_toggle; // We can unset the flag as soon as it's observed.
        DEMO_stage--;
        if (DEMO_stage > 0x0E){
            DEMO_stage = 0x0E; // TODO make this dynamic
        }
        touched_this_row = true;
        current_demo_Frame.refresh_L9 = true;
    }
    if (buttons_state & button_b_toggle){
        buttons_state ^= button_b_toggle;
        DEMO_stage++;
        DEMO_stage = DEMO_stage % 0x0F; // TODO make this dynamic
        touched_this_row = true;
        current_demo_Frame.refresh_L9 = true;
    }
    if (buttons_state & button_c_toggle){
        buttons_state ^= button_c_toggle;
        touched_this_row = true;
        DEMO_screen_state++;
        DEMO_screen_state = DEMO_screen_state % 0x04;
    }
}

char* DEMO_computeLine(Stage active_species, const int which_row){
    touched_this_row = false;
    unsigned int reversal_for_four[4] = {1, 0, 3, 2}; // TODO: There must be a way to handle reversals with matrix math and we should generalize that.
    unsigned int reversal_for_nine[9] = {2, 1, 0, 5, 4, 3, 8, 7, 6};
    const char* relevant_frame;
    playfield[which_row][0] = ' ';
    playfield[which_row][1] = ' ';
    playfield[which_row][2] = ' ';
    playfield[which_row][3] = ' ';
    playfield[which_row][4] = ' ';
    playfield[which_row][5] = ' ';
    playfield[which_row][6] = ' ';
    playfield[which_row][7] = ' ';
    reversed = false;
    inverted = false;
    switch(DEMO_screen_state){
        case 0x00 : // We are doing the idle animation
            relevant_frame = active_species.animation[DEMO_current_frame];
            touched_this_row = true;
            break;
        case 0x01 : // We are doing the face-right animation
            relevant_frame = active_species.faceRight;
            touched_this_row = true;
            break;
        case 0x02 : // We are doing the face-left animation
            relevant_frame = active_species.faceRight;
            reversed = true;
            touched_this_row = true;
            break;
        case 0x03 : // We are doing the eating animation
            relevant_frame = active_species.animationEating[DEMO_current_frame];
            touched_this_row = true;
            break;
    };
    if (buttons_state & button_d_toggle){
        inverted = true;
    }
    if (inverted & reversed){
            directives_out[0] = '3';  // TODO this is extremely inelegant, man, just fix it.
            directives_out[1] = '3';
            directives_out[2] = '3';
            directives_out[3] = '3';
            directives_out[4] = '3';
            directives_out[5] = '3';
            directives_out[6] = '3';
            directives_out[7] = '3';
            directives_out[8] = 0;
    }
    else if (inverted){
            directives_out[0] = '2';  // TODO this is extremely inelegant, man, just fix it.
            directives_out[1] = '2';
            directives_out[2] = '2';
            directives_out[3] = '2';
            directives_out[4] = '2';
            directives_out[5] = '2';
            directives_out[6] = '2';
            directives_out[7] = '2';
            directives_out[8] = 0;
    }
    else if (reversed){
            directives_out[0] = '1';  // TODO this is extremely inelegant, man, just fix it.
            directives_out[1] = '1';
            directives_out[2] = '1';
            directives_out[3] = '1';
            directives_out[4] = '1';
            directives_out[5] = '1';
            directives_out[6] = '1';
            directives_out[7] = '1';
            directives_out[8] = 0;
    }
    else {
            directives_out[0] = '0';  // TODO this is extremely inelegant, man, just fix it.
            directives_out[1] = '0';
            directives_out[2] = '0';
            directives_out[3] = '0';
            directives_out[4] = '0';
            directives_out[5] = '0';
            directives_out[6] = '0';
            directives_out[7] = '0';
    }
    if (reversed){ // this will trigger regardless of if it's both or just.
            if ((active_species.size == 0x01) & (which_row == 1) ) {
                playfield[which_row][3] = relevant_frame[0];
            }
            else if (active_species.size == 0x04 & char_tracker < 0x04){
                playfield[which_row][3] = relevant_frame[reversal_for_four[char_tracker]];
                playfield[which_row][4] = relevant_frame[reversal_for_four[char_tracker + 1]];
                char_tracker = char_tracker + 2;
            }
            else if (active_species.size == 0x09 & char_tracker < 0x09){
                playfield[which_row][3] = relevant_frame[reversal_for_nine[char_tracker]];
                playfield[which_row][4] = relevant_frame[reversal_for_nine[char_tracker + 1]];
                playfield[which_row][5] = relevant_frame[reversal_for_nine[char_tracker + 2]];
                char_tracker = char_tracker + 3;
            }
        }
    else{ // otherwise we can draw forwards.
            if ((active_species.size == 0x01) & (which_row == 1) ) {
                playfield[which_row][3] = relevant_frame[0];
            }
            else if (active_species.size == 0x04 & char_tracker < 0x04){
                playfield[which_row][3] = relevant_frame[char_tracker];
                playfield[which_row][4] = relevant_frame[char_tracker + 1];
                char_tracker = char_tracker + 2;
            }
            else if (active_species.size == 0x09 & char_tracker < 0x09){
                playfield[which_row][3] = relevant_frame[char_tracker];
                playfield[which_row][4] = relevant_frame[char_tracker + 1];
                playfield[which_row][5] = relevant_frame[char_tracker + 2];
                char_tracker = char_tracker + 3;
            }
        }
    return playfield[which_row];
}

void DEMO_computeAnimationFrame(void){  // We can use a far simpler version of the MG demo calculation because metanimations will not be applied.
    Stage active_species = EVO_metaStruct[DEMO_stage];
    if (FORCE_REFRESH){
        DEMO_current_frame = 0x00;\
    }
    char_tracker = 0;
    current_demo_Frame.line2 = DEMO_computeLine(active_species, 0);
    current_demo_Frame.refresh_L2 = touched_this_row;
    current_demo_Frame.directive_L2 = directives_out;
    current_demo_Frame.line3 = DEMO_computeLine(active_species, 1);
    current_demo_Frame.refresh_L3 = touched_this_row;
    current_demo_Frame.directive_L3 = directives_out;
    current_demo_Frame.line4 = DEMO_computeLine(active_species, 2);
    current_demo_Frame.refresh_L4 = touched_this_row;
    current_demo_Frame.directive_L4 = directives_out;
    current_demo_Frame.directive_L5 = directives_out; // We can assume these are all the same, actually.
    current_demo_Frame.directive_L6 = directives_out;
    current_demo_Frame.directive_L1 = directives_out;
    DEMO_current_frame++;
    DEMO_current_frame = DEMO_current_frame % 2; // The longest animations in this mode are two frames, so we really just need to flip that bit.
}

// This is the master function that controls the demo mode and is called in other parts of the firmware
void SCENE_demo_mode(void){
    current_demo_Frame.line0 = " PETI DEMO MODE ";
    current_demo_Frame.line1 = "";
    current_demo_Frame.line6 = "";
    current_demo_Frame.line5 = "";
    current_demo_Frame.line8 = Print_CurrentTime();
    current_demo_Frame.refresh_L8 = true;
    current_demo_Frame.refresh_L9 = false; // When we handle inputs this will change to "true" if needed.
    DEMO_handleInputs();
    current_demo_Frame.line9 = stageNames[DEMO_stage];
    DEMO_computeAnimationFrame();
    interacted_flag = false; // we use the presence of this flag to decide to do the Mega Refresh for handling button D.
    DISPLAY_updatesOnly(current_demo_Frame, MODE_GAME);
}



