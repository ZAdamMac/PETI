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
#include "lib/display/display.h"
#include "lib/game/game_manager.h"
#include "lib/game/evo_data.h"
#include "driverlib.h"
#include "main_game.h"
#include "scenes_manager.h"
#include "main.h"
#include "lib/scenes/main_game/metanimations.h"
#include "lib/menus/main_game.h"
#include "lib/hwinit/human_input.h"
#include "lib/hwinit/battery.h"

unsigned int char_tracker;                         //Charts the count we are at in terms of the icon for the species character animation
unsigned int icon_size;                            //effectively just holds the thing.

// For a given input species_charset (being the animation data from the EVO object) and meta_placements (a string from the metaanimation),
// compute a given row. This function is going to be subject to further development that is likely to add arguments, which is why it is not exposed outside
// this package.
char* MG_computeMeta(const char* species_charset, const char* meta_placements){
    unsigned int charindex = 0;
    unsigned int this_char_tracker;
    char  results[PIXELS_X/16+1]; // We need to briefly fill SOME var with the text we're computing
    unsigned int reversal_array[4] = {1, 0, 3, 2};  // This reverses a 2x2 sprite!
    while (charindex < PIXELS_X/16){ // this assumes the 16x16 graphical font is being used.
        if (meta_placements[charindex] == '-' || meta_placements[charindex] == '_'){ // Dashes are empty cells, underscores are reversed empty cells
            results[charindex] = ' ';
        }
        else if (meta_placements[charindex] == '0' || meta_placements[charindex] == '2'){ // Zeros say "put the character here, unmodified", 2 is color inverted
            results[charindex] = species_charset[char_tracker];
            char_tracker++; // we can go to the next character in the frame
            char_tracker = char_tracker % icon_size; // Here to prevent out of index crashes. You should never define an animation that violates this.
        }
        else if (meta_placements[charindex] == '1' || meta_placements[charindex] == '3'){ // 1 says to put the character here, reversed along the x; 3 says to do that and color invert
            this_char_tracker = reversal_array[char_tracker]; // In this case the characters need to arrange differently.
            this_char_tracker = this_char_tracker % icon_size;
            results[charindex] = species_charset[this_char_tracker];
            char_tracker++; // we can go to the next character in the frame
            char_tracker = char_tracker % icon_size; // Here to prevent out of index crashes. You should never define an animation that violates this.
        }
        charindex++;

    }
    return &results; // We only want to return the /pointer/ here, which is all strcpy wants.
}

//Computes a printTextLarge display directives string based on the metanimation string provided
char* MG_computeDirective(const char* meta_placements){
    unsigned int dirindex = 0;
    char directives_out[PIXELS_X/16+1];
    while (dirindex < PIXELS_X/16){
        switch (meta_placements[dirindex]){
            case '-' :
                directives_out[dirindex] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                break;
            case '_' :
                directives_out[dirindex] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
                break;
            case '0' :
                directives_out[dirindex] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                break;
            case '1' :
                directives_out[dirindex] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
                break;
            case '2' :
                directives_out[dirindex] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
                break;
            case '3' :
                directives_out[dirindex] = FONT_ADDR_0 + DIRECTIVE_REVERSED_NEGATIVE;
                break;
        }
        dirindex++;
    }
    return &directives_out;
}

//Parent function that keeps track of what frame of animation we're on and uses MG_updatePlayfield
// to determine the content of each output frame in the main 6 rows of the display, as well as which
// of those flags need to be reset.
void MG_updatePlayfield(void){
    int active_line;
    Stage active_species = EVO_metaStruct[StateMachine.STAGE_ID];
    icon_size = active_species.size; // This will either be 1 or 4 in the current spec.
    if (FORCE_REFRESH){ // In this case we're freshly arriving so let's start at 0
        SCENE_FRAME = 0x00;
    }
    Metanimation active_meta = SCENES_metanimations[active_species.phase]; // Get the metanimation needed based on the `phase` value of the current active species.
    const char *species_anim = active_species.animation[SCENE_FRAME % 2]; // Get the right frame of animation data from the active species.
    char_tracker = 0;
    for (active_line = 0;active_line<6; active_line++){
        strcpy(DISPLAY_FRAME.frame[active_line+1].line, MG_computeMeta(species_anim, active_meta.d[SCENE_FRAME][active_line])); // Compute the animated text for this row.
        strcpy(DISPLAY_FRAME.frame[active_line+1].directives, MG_computeDirective(active_meta.d[SCENE_FRAME][active_line])); // Comput the directive information.
    }
    SCENE_FRAME++;
    SCENE_FRAME = SCENE_FRAME % 4; // Automatic index rollover because manual coding sucks.
    if (BATTERY_LOW){
        DISPLAY_FRAME.frame[1].line[0] = 0xF9; //TODO: Replace with a much prettier version of the same operation.
    }
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
		    SCENE_ACT = MG_menu_scene_addresses[SCENE_CURSOR_POS-1]; // This will update the scene. The current frame will be drawn and the next frame will take you into the menu you picked.
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
    unsigned int top_slice; unsigned int blank_spaces; unsigned int left_pad; unsigned int text_index; unsigned int icon_index; char mg_top_alterable[PIXELS_X/FONT_SIZE_FLOOR_X];
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
            mg_top_alterable[text_index] = ' '; // this is a non-printing padding space.
            left_pad--;
            text_index++;
        }
        else if (top_slice > 0){
            mg_top_alterable[text_index] = MG_menu_icons[icon_index];
            text_index++;
            icon_index++;
            top_slice--;
        }
        else { // We need to pad right.
            mg_top_alterable[text_index]= ' ';
            text_index++;
        }
    }
    return &mg_top_alterable;
}

//Renders the bottom menu bar. This will have the latter half of the MG_menu_icons array, rounded down.
char* MG_printBottomMenu(void){
    unsigned int bottom_slice; unsigned int blank_spaces; unsigned int left_pad; unsigned int text_index; unsigned int icon_index; char mg_bottom_alterable[PIXELS_X/FONT_SIZE_FLOOR_X];
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
            mg_bottom_alterable[text_index] = ' '; // this is a non-printing padding space.
            left_pad--;
            text_index++;
        }
        else if (bottom_slice > 0){
            mg_bottom_alterable[text_index] = MG_menu_icons[icon_index];
            text_index++;
            icon_index++;
            bottom_slice--;
        }
        else { // We need to pad right.
            mg_bottom_alterable[text_index]= ' ';
            text_index++;
        }
    }
    return &mg_bottom_alterable;
}

// This, along with the function below, compute which icon is highlighted (printed inverted) for the given current cursor position.
char* MG_computeTopDirective(void){
    unsigned int top_slice; unsigned int blank_spaces; unsigned int left_pad; unsigned int text_index; unsigned int cursor_index = 0; char directives_top[PIXELS_X/FONT_SIZE_FLOOR_X];
    top_slice = SCENE_PAGE_COUNT/2;
    if (SCENE_PAGE_COUNT % 2 != 0){
        top_slice++; //Top row always gets the extra item
    }
    blank_spaces = (PIXELS_X/FONT_SIZE_FLOOR_X) - top_slice;
    left_pad = blank_spaces/2; // This will always round down, which we actually want
    text_index = 0;
    for (text_index = 0; text_index < PIXELS_X/FONT_SIZE_FLOOR_X; ++text_index){
        directives_top[text_index] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
    }
    if (SCENE_CURSOR_POS > 0){ // When the cursor position is 0, we don't need to highlight anything.
        if (top_slice >= SCENE_CURSOR_POS){
            cursor_index = left_pad + SCENE_CURSOR_POS - 1;
            directives_top[cursor_index] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
        }
    }
    return &directives_top;
}

char* MG_computeBottomDirective(void){
    unsigned int bottom_slice; unsigned int top_slice; unsigned int magic_pad; unsigned int blank_spaces; unsigned int left_pad; unsigned int text_index; unsigned int cursor_index = 0; char directives_bottom[PIXELS_X/FONT_SIZE_FLOOR_X];
    bottom_slice = SCENE_PAGE_COUNT/2;
    magic_pad = SCENE_PAGE_COUNT % 2;
    if (magic_pad == 0) { // Special Edge case for even numbers.
        magic_pad = 2;
    }
    top_slice = SCENE_PAGE_COUNT - bottom_slice;
    blank_spaces = (PIXELS_X/FONT_SIZE_FLOOR_X) - bottom_slice;
    left_pad = blank_spaces/2; // This will always round down, which we actually want
    text_index = 0;
    for (text_index = 0; text_index < PIXELS_X/FONT_SIZE_FLOOR_X; ++text_index){
        directives_bottom[text_index] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
    }
    if (SCENE_CURSOR_POS > top_slice){
        cursor_index = left_pad + SCENE_CURSOR_POS - bottom_slice - top_slice + magic_pad;
        directives_bottom[cursor_index] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
    }
    return &directives_bottom;
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
    MG_updatePlayfield(); // Animations are gross, so this also handles the
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
