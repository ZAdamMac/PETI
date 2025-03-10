//***************************************************************************************
//  PETI FOSS Virtual Pet - Main Game Scene
//
//  This code describes the draw process for the main game screen, heavily referencing
//  header files for animations and tilesets to use. Most of the menu layouts are defined
//  in the header to this code. Alter //CAREFULLY//, can be breaking.
//
//
//
//  Zac Adam-MacEwen (Kensho Security Labs)
//  May 2021
//***************************************************************************************

#include <msp430.h>
#include <string.h>
#include "driverlib/MSP430FR5xx_6xx/driverlib.h"
#include "lib/display/display.h"
#include "lib/display/icons.h"
#include "lib/game/game_manager.h"
#include "lib/game/evo_data.h"
#include "driverlib.h"
#include "main_game.h"
#include "scenes_manager.h"
#include "main.h"
#include "lib/scenes/main_game/metanimations.h"
#include "lib/scenes/menu_generator.h"
#include "lib/menus/main_game.h"
#include "lib/hwinit/human_input.h"
#include "lib/hwinit/battery.h"

unsigned int char_tracker;                         //Charts the count we are at in terms of the icon for the species character animation
unsigned int icon_size;                            //effectively just holds the thing.
unsigned int MG_lights_on = 1;                     //Lights are on by default

// For a given input species_charset (being the animation data from the EVO object) and meta_placements (a string from the metaanimation),
// compute a given row. This function is going to be subject to further development that is likely to add arguments, which is why it is not exposed outside
// this package.
char* MG_computeMeta(const char* species_charset, const char* meta_placements){
    unsigned int charindex = 0;
    unsigned int this_char_tracker;
    unsigned int reversal_array[4] = {1, 0, 3, 2};  // This reverses a 2x2 sprite!
    unsigned int reversal_array_large[9] = {2, 1, 0, 5, 4, 3, 8, 7, 6}; // This reverses a 3x3 sprite!
    while (charindex < PIXELS_X/16){ // this assumes the 16x16 graphical font is being used.
        if (meta_placements[charindex] == '-' || meta_placements[charindex] == '_'){ // Dashes are empty cells, underscores are reversed empty cells
            WORK_STRING[charindex] = ' ';
        }
        else if (meta_placements[charindex] == '0' || meta_placements[charindex] == '2'){ // Zeros say "put the character here, unmodified", 2 is color inverted
            WORK_STRING[charindex] = species_charset[char_tracker];
            char_tracker++; // we can go to the next character in the frame
            char_tracker = char_tracker % icon_size; // Here to prevent out of index crashes. You should never define an animation that violates this.
        }
        else if (meta_placements[charindex] == '1' || meta_placements[charindex] == '3'){ // 1 says to put the character here, reversed along the x; 3 says to do that and color invert
            if (icon_size == EVO_size_large){
                this_char_tracker = reversal_array_large[char_tracker];
            }
            else {
                this_char_tracker = reversal_array[char_tracker]; // In this case the characters need to arrange differently.
            }
            this_char_tracker = this_char_tracker % icon_size;
            WORK_STRING[charindex] = species_charset[this_char_tracker];
            char_tracker++; // we can go to the next character in the frame
            char_tracker = char_tracker % icon_size; // Here to prevent out of index crashes. You should never define an animation that violates this.
        }
        charindex++;

    }
    return &WORK_STRING; // We only want to return the /pointer/ here, which is all strcpy wants.
}

//Computes a printTextLarge display directives string based on the metanimation string provided
char* MG_computeDirective(const char* meta_placements, char FONT_ADDR){
    unsigned int dirindex = 0;
    while (dirindex < PIXELS_X/16){
        switch (meta_placements[dirindex]){
            case '-' :
                WORK_STRING[dirindex] = FONT_ADDR + DIRECTIVE_NORMAL;
                break;
            case '_' :
                WORK_STRING[dirindex] = FONT_ADDR + DIRECTIVE_NEGATIVE;
                break;
            case '0' :
                WORK_STRING[dirindex] = FONT_ADDR + DIRECTIVE_NORMAL;
                break;
            case '1' :
                WORK_STRING[dirindex] = FONT_ADDR + DIRECTIVE_REVERSED;
                break;
            case '2' :
                WORK_STRING[dirindex] = FONT_ADDR + DIRECTIVE_NEGATIVE;
                break;
            case '3' :
                WORK_STRING[dirindex] = FONT_ADDR + DIRECTIVE_REVERSED_NEGATIVE;
                break;
        }
        dirindex++;
    }
    return &WORK_STRING;
}


/* Fed the sleep_animation from a given Stage, computes one line of sleep
 * animation data, places it on the WORK_STRING, and returns the pointer for it.
 */
char* MG_computeLineSleep(const char * sleep_animation){
    unsigned int column;

    for (column=0; column<(PIXELS_X/16); column++){
        WORK_STRING[column] = ' ';
    }

    switch (icon_size){
        case EVO_size_small:
            WORK_STRING[(PIXELS_X/16)/2] = sleep_animation[char_tracker];
            break;
        case EVO_size_med:
            WORK_STRING[(PIXELS_X/16)/2 - 1] = sleep_animation[char_tracker];
            char_tracker++;
            WORK_STRING[(PIXELS_X/16)/2] = sleep_animation[char_tracker];
            char_tracker++;
            break;
        case EVO_size_large:
            WORK_STRING[((PIXELS_X/16)/2) - 1] = sleep_animation[char_tracker];
            char_tracker++;
            WORK_STRING[((PIXELS_X/16)/2)] = sleep_animation[char_tracker];
            char_tracker++;
            WORK_STRING[((PIXELS_X/16)/2) + 1] = sleep_animation[char_tracker];
            char_tracker++;
            break;
    }
    return &WORK_STRING; // We only want to return the /pointer/ here, which is all strcpy wants.
}


/* Bulk apply the directives to a sleeping pet.
 */
char* MG_computeDirectiveSleep(char FONT_ADDR){
    unsigned int dirindex = 0;
    while (dirindex < PIXELS_X/16){
        WORK_STRING[dirindex] = FONT_ADDR + DIRECTIVE_NORMAL;
        dirindex++;
    }
    return &WORK_STRING;
}


/* Performs the necessary calculations to determine which status icons need to
 * be displayed through the work string. This is a highly simplistic version of
 * this function and will likely be altered in the future.
 */
char* MG_placeStatusIcons(void){ // FUTURE: give a root positional coordinate
    unsigned int col;

    for (col = 0; col < PIXELS_X/16; col++){
        WORK_STRING[col] = ' '; // For some reason, setting this iteratively is the only way I've been able to make it work.
    }
    if (BATTERY_LOW){
        WORK_STRING[0] = 0xF9; //FUTURE: Less hardcode for the line positions please.
    }
    if (StateMachine.ACT == 0){
        WORK_STRING[1] = 0xFA; // Display the sleep icon while sleeping.
    }

    return &WORK_STRING;
}


/* Given an x-y position in *character cells*, computes a stack of poop icons
*  and places them into the global DISPLAY_FRAME. This overwrites any other
*  tiles placed in those positions but dynamically respects the lighting state.
*  width of pile is determined by MG_poop_stack_width and max rows is a function
*  of that and GM_MAX_POOPS.
*/
void MG_displayPoop(unsigned int root_x, unsigned int root_y){
    unsigned int poop_rows, row, poops_this_row, col;
    char used_icon, used_directive;

    // Currently, a two-frame animation is supported. Modifying the values in
    // icons.h will transparently retarget the poop icons as long as they share
    // a font. The font could also be modified in-place without changing the
    // characters if you really wanted.
    if (SCENE_FRAME % 2){
        used_icon = ICON_POOP_A;
    }
    else {
        used_icon = ICON_POOP_B;
    }

    // We also need this, to preserve the directive already in place. This is
    // easier to compute than dynamically pulling it from the bitfield.
    if (MG_lights_on){
        used_directive = DIRECTIVE_NORMAL;
    }
    else {
        used_directive = DIRECTIVE_BLACKOUT; 
    }

    poop_rows = StateMachine.POOP_COUNT / MG_poop_stack_width;
    if (StateMachine.POOP_COUNT % MG_poop_stack_width){
        poop_rows++; // If there is a remainder, we need an extra row.
    }
    
    // Now we can do this row-by row.
    for (row=0; row<=poop_rows; row++){
        //First Pass: Set the poop icon onto the line for the row.
        strcpy(WORK_STRING, &DISPLAY_FRAME.frame[root_x - row].line);
        if ((row == poop_rows) && (StateMachine.POOP_COUNT % MG_poop_stack_width)){ // Special Remainder Pile
            poops_this_row = StateMachine.POOP_COUNT % MG_poop_stack_width;
        }
        else { //for the TOP row
            poops_this_row = MG_poop_stack_width;
        }
        for (col=0; col<poops_this_row; col++){
            WORK_STRING[root_y-col] = used_icon;
        }
        strcpy(DISPLAY_FRAME.frame[root_x - row].line, &WORK_STRING);
        //second pass: set the font directive.
        strcpy(WORK_STRING, &DISPLAY_FRAME.frame[root_x - row].directives);
        if ((row == poop_rows) && (StateMachine.POOP_COUNT % MG_poop_stack_width)){ // Special Remainder Pile
            poops_this_row = StateMachine.POOP_COUNT % MG_poop_stack_width;
        }
        else { //for the TOP row
            poops_this_row = MG_poop_stack_width;
        }
        for (col=0; col<poops_this_row; col++){
            WORK_STRING[root_y-col] = ICON_POOP_FONT + used_directive;
        }
        strcpy(DISPLAY_FRAME.frame[root_x - row].directives, &WORK_STRING);
    }
}

//Parent function that keeps track of what frame of animation we're on and uses MG_updatePlayfieldIdle
// to determine the content of each output frame in the main 6 rows of the display, as well as which
// of those flags need to be reset.
void MG_updatePlayfieldIdle(void){
    unsigned int active_line;
    Stage active_species = EVO_metaStruct[StateMachine.STAGE_ID];
    icon_size = active_species.size; 
    int font_used = active_species.font;
    if (FORCE_REFRESH){ // In this case we're freshly arriving so let's start at 0
        SCENE_FRAME = 0x00;
    }
    Metanimation active_meta = SCENES_metanimations[active_species.metanimation_id]; // Get the metanimation needed based on the `phase` value of the current active species.
    const char *species_anim = active_species.animation[SCENE_FRAME % 2]; // Get the right frame of animation data from the active species.
    char_tracker = 0;
    for (active_line = 0;active_line<6; active_line++){
        strcpy(DISPLAY_FRAME.frame[active_line+1].line, MG_computeMeta(species_anim, active_meta.d[SCENE_FRAME][active_line])); // Compute the animated text for this row.
        strcpy(DISPLAY_FRAME.frame[active_line+1].directives, MG_computeDirective(active_meta.d[SCENE_FRAME][active_line], font_used)); // Comput the directive information.
    }
    SCENE_FRAME++;
    SCENE_FRAME = SCENE_FRAME % 4; // Automatic index rollover because manual coding sucks.
    strcpy(DISPLAY_FRAME.frame[1].line, MG_placeStatusIcons());
}

/* Parent logic for displaying the sleeping pet animation. Pulls the animation
 * data based on the current stage ID on the state machine and passes it to the
 * sleep animation handler, then calls MG_placeStatusIcons.
 */
void MG_updatePlayfieldSleeping(void){
    unsigned int row;
    Stage active_species = EVO_metaStruct[StateMachine.STAGE_ID];
    const char * sleep_animation = active_species.animationSleeping;
    char font_used = active_species.font;
    icon_size = active_species.size;
    // Blank the top three lines since none of the sleep animations use them.
    strcpy(DISPLAY_FRAME.frame[1].line, " ");
    strcpy(DISPLAY_FRAME.frame[2].line, " ");
    strcpy(DISPLAY_FRAME.frame[3].line, " ");
    char_tracker = 0;
    switch (icon_size){
        case EVO_size_small: 
            strcpy(DISPLAY_FRAME.frame[4].line, " ");
            strcpy(DISPLAY_FRAME.frame[5].line, " ");
            strcpy(DISPLAY_FRAME.frame[6].line, MG_computeLineSleep(sleep_animation));
            break;
        case EVO_size_med:
            strcpy(DISPLAY_FRAME.frame[4].line, " ");
            strcpy(DISPLAY_FRAME.frame[5].line, MG_computeLineSleep(sleep_animation));
            strcpy(DISPLAY_FRAME.frame[6].line, MG_computeLineSleep(sleep_animation));
            break;
        case EVO_size_large:
            strcpy(DISPLAY_FRAME.frame[4].line, MG_computeLineSleep(sleep_animation));
            strcpy(DISPLAY_FRAME.frame[5].line, MG_computeLineSleep(sleep_animation));
            strcpy(DISPLAY_FRAME.frame[6].line, MG_computeLineSleep(sleep_animation));
            break;
    }

    for (row=1;row<8;row++){
        strcpy(DISPLAY_FRAME.frame[row].directives, MG_computeDirectiveSleep(font_used));
    }

    strcpy(DISPLAY_FRAME.frame[1].line, MG_placeStatusIcons());
    strcpy(DISPLAY_FRAME.frame[1].directives, MG_computeDirectiveSleep(FONT_ADDR_0));
}

// Boilerplate input handler, similar to those seen in all other parts of the firmware.
void MG_handleInputs(void){
    int i;
    unsigned int this_event;
    for (i =0; i <= HID_input_events_queue_depth; ++i) // For every event in the event queue;
    {
        this_event = HID_input_events_queue[i]; //fetch the event.
        switch (this_event){
            case(BUTTON_A_PRESS): // scroll forward by one slot
		        SCENE_CURSOR_POS++;
                if (SCENE_CURSOR_POS > SCENE_PAGE_COUNT){
                    SCENE_CURSOR_POS = 0;
                }
            	break;
            case(BUTTON_B_PRESS): // scroll backward by one slot
                SCENE_CURSOR_POS--;
                if (SCENE_CURSOR_POS > SCENE_PAGE_COUNT){
                    SCENE_CURSOR_POS = 0;
                }
                break;
            case(BUTTON_C_PRESS): //Make a selection of the current icon
                if (SCENE_CURSOR_POS != 0){
                    MG_menu_options_array[SCENE_CURSOR_POS-1](); // This will update the scene. The current frame will be drawn and the next frame will take you into the menu you picked.
                    SCENE_CURSOR_POS = 0;
                }
                break;
            case(BUTTON_D_PRESS): // clear the highlights
		        SCENE_CURSOR_POS = 0;
                break;
        }
        HID_input_events_queue[i] = BUTTON_NO_PRESS;
        
    };
    HID_input_events_queue_depth = 0;
}

//Renders the top menu bar. This will have the first half of the MG_menu_icons array, rounded up.
char* MG_printTopMenu(void){
    unsigned int top_slice; unsigned int blank_spaces; unsigned int left_pad; unsigned int text_index; unsigned int icon_index;
    top_slice = SCENE_PAGE_COUNT/2;
    if (SCENE_PAGE_COUNT % 2 != 0){
        top_slice++; //Top row always gets the extra item
    }
    blank_spaces = (PIXELS_X/FONT_SIZE_FLOOR_X) - top_slice;
    left_pad = blank_spaces/2; // This will always round down, which we actually want
    text_index = 0;
    icon_index = 0;
    while (text_index < PIXELS_X/FONT_SIZE_FLOOR_X){
        if (left_pad > 0){
            WORK_STRING[text_index] = ' '; // this is a non-printing padding space.
            left_pad--;
            text_index++;
        }
        else if (top_slice > 0){
            WORK_STRING[text_index] = MG_menu_icons[icon_index];
            text_index++;
            icon_index++;
            top_slice--;
        }
        else { // We need to pad right.
            WORK_STRING[text_index]= ' ';
            text_index++;
        }
    }
    return &WORK_STRING;
}

//Renders the bottom menu bar. This will have the latter half of the MG_menu_icons array, rounded down.
char* MG_printBottomMenu(void){
    unsigned int bottom_slice; unsigned int blank_spaces; unsigned int left_pad; unsigned int text_index; unsigned int icon_index;
    bottom_slice = SCENE_PAGE_COUNT/2;
    if (SCENE_PAGE_COUNT % 2 == 1){
        icon_index = bottom_slice + 1;
    }
    else {
        icon_index = bottom_slice;
    }
    blank_spaces = (PIXELS_X/FONT_SIZE_FLOOR_X) - bottom_slice;
    left_pad = blank_spaces/2; // This will always round down, which we actually want
    text_index = 0;
    while (text_index < PIXELS_X/FONT_SIZE_FLOOR_X){
        if (left_pad > 0){
            WORK_STRING[text_index] = ' '; // this is a non-printing padding space.
            left_pad--;
            text_index++;
        }
        else if (bottom_slice > 0){
            WORK_STRING[text_index] = MG_menu_icons[icon_index];
            text_index++;
            icon_index++;
            bottom_slice--;
        }
        else { // We need to pad right.
            WORK_STRING[text_index]= ' ';
            text_index++;
        }
    }
    return &WORK_STRING;
}

// This, along with the function below, compute which icon is highlighted (printed inverted) for the given current cursor position.
char* MG_computeTopDirective(void){
    unsigned int top_slice; unsigned int blank_spaces; unsigned int left_pad; unsigned int text_index; unsigned int cursor_index = 0;
    top_slice = SCENE_PAGE_COUNT/2;
    if (SCENE_PAGE_COUNT % 2 != 0){
        top_slice++; //Top row always gets the extra item
    }
    blank_spaces = (PIXELS_X/FONT_SIZE_FLOOR_X) - top_slice;
    left_pad = blank_spaces/2; // This will always round down, which we actually want
    text_index = 0;
    for (text_index = 0; text_index < PIXELS_X/FONT_SIZE_FLOOR_X; text_index++){
        WORK_STRING[text_index] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
    }
    if (SCENE_CURSOR_POS > 0){ // When the cursor position is 0, we don't need to highlight anything.
        if (top_slice >= SCENE_CURSOR_POS){
            cursor_index = left_pad + SCENE_CURSOR_POS - 1;
            WORK_STRING[cursor_index] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
        }
    }
    return &WORK_STRING;
}

char* MG_computeBottomDirective(void){
    unsigned int bottom_slice; unsigned int top_slice; unsigned int magic_pad; unsigned int blank_spaces; unsigned int left_pad; unsigned int text_index; unsigned int cursor_index = 0;
    bottom_slice = SCENE_PAGE_COUNT/2; //3
    magic_pad = 2;
    top_slice = SCENE_PAGE_COUNT - bottom_slice; //4
    blank_spaces = (PIXELS_X/FONT_SIZE_FLOOR_X) - bottom_slice; //13
    left_pad = blank_spaces/2; // This will always round down, which we actually want 6
    text_index = 0;
    for (text_index = 0; text_index < (PIXELS_X/FONT_SIZE_FLOOR_X); text_index++){
       WORK_STRING[text_index] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
    }
    if (SCENE_CURSOR_POS > top_slice){
        cursor_index = left_pad + SCENE_CURSOR_POS - bottom_slice - top_slice + magic_pad; // FUTURE This is definitely wrong!
        WORK_STRING[cursor_index] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
    }
    return &WORK_STRING;
}


/* Handles the case where the lights have been turned off for the pet by setting
 * the display strings to blank and setting the appropriate directives.
 */
void MG_LightsOut(){
    unsigned int row, col;
    for (row = 1; row < 8; row++){
        //It is too dark to get ye flask.
        for (col = 0; col < PIXELS_X/FONT_SIZE_FLOOR_X; col++){
            DISPLAY_FRAME.frame[row].line[col] = ' '; // NOTHING TO SEE HERE.
            DISPLAY_FRAME.frame[row].directives[col] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE; // IT'S JUST SO DARK GUYS
        }
    }
}

// Called once per cycle to update the menu and playfield based on their various subfunctions for update.
// Some functions (those used for menuing) are not defined and present only as comments.
void MG_computeNextFrame(void){
    if (StateMachine.STAGE_ID != 0x00){ // Eggs don't get menus
        strcpy(DISPLAY_FRAME.frame[0].line, MG_printTopMenu());
        strcpy(DISPLAY_FRAME.frame[0].directives, MG_computeTopDirective());
        strcpy(DISPLAY_FRAME.frame[8].line, MG_printBottomMenu());
        strcpy(DISPLAY_FRAME.frame[8].directives, MG_computeBottomDirective());
        strcpy(DISPLAY_FRAME.frame[7].line, " "); // In some conditions the previous frame may set this!
    }
    else{ // Menu and middle strings not needed
        strcpy(DISPLAY_FRAME.frame[0].line, " ");
        strcpy(DISPLAY_FRAME.frame[7].line, " ");
        strcpy(DISPLAY_FRAME.frame[8].line, " ");
    }

    //Moving on to the large font work, which means we need to "shrink" the work string
    WORK_STRING[8] = '\x00';

    if (MG_lights_on){
        switch (StateMachine.ACT){
            case GM_ACTIVITY_IDLE:
                MG_updatePlayfieldIdle(); // Animations are gross, so this also handles the
                break;
            case GM_ACTIVITY_ISEGG: // For pure legacy reasons, IDLE can also handle this state.
                MG_updatePlayfieldIdle();
                break;
            case GM_ACTIVITY_SLEEPING:
                MG_updatePlayfieldSleeping();
                break;
        }
    }
    else {
        MG_LightsOut();
        strcpy(DISPLAY_FRAME.frame[1].line, MG_placeStatusIcons());
        if (StateMachine.ACT == GM_ACTIVITY_SLEEPING){
            if (SCENE_CURRENT_PAGE >= MG_sleep_display_cycles){ // Piggybacking on an unusued scene-wide var to handle
                DISPLAY_sleepLCD();
                //we can put this back now; this function won't really do anything elsewise.
                SCENE_CURRENT_PAGE = 0;
            }
            SCENE_CURRENT_PAGE++;
        }
    }
    //FUTURE: At some point we should draw the status icons here.
    if (StateMachine.POOP_COUNT){
        MG_displayPoop(7,7); //Encodes for the bottom-right corner of the screen. 
        //FUTURE Replace these magic numbers
    }
}

// A main function to be called by the scene_manger to handle the inputs and then compute the next frame of animation, before sending all the updates to
// DISPLAY. Handling the inputs itself can set the exit conditions for this scene so there is no need for special exit logic.
void SCENE_main_game(void){
    unsigned int debug_disabled;
    debug_disabled = GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN6);
    if (debug_disabled){
        SCENE_PAGE_COUNT = MG_count_menu_icons - 1;
    }
    else {
        SCENE_PAGE_COUNT = MG_count_menu_icons;
    }
    MG_handleInputs();
    MG_computeNextFrame();
    DISPLAY_updatesOnly_enhanced(&DISPLAY_FRAME, MODE_GAME);

}
