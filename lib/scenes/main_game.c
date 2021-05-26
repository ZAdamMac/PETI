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
#include "lib/display/display.h"
#include "lib/game/game_manager.h"
#include "lib/game/evo_data.h"
#include "driverlib.h"
#include "main_game.h"
#include "scenes_manager.h"
#include "main.h"
#include "lib/scenes/main_game/metanimations.h"

DisplayFrame MainGameFrame;
char mg_top_alterable[17];                         //Represents the top menu. Consider deprecating and using a method similar to playfield to allow cleaner code.
char mg_bottom_alterable[17];
char playfield[6][8];                              //Array of arrays of characters used to set each row of the output frame programmatically rather than manually for tidiness.
char directives_out[6][8];                         //Array of arrays of characters used for the enhance larged text print function.
unsigned int mg_cursor_position = 0x00;            //Cursor position, used to track which digit is being manipulated
unsigned int char_tracker;                         //Charts the count we are at in terms of the icon for the species character animation
unsigned int icon_size;                            //effectively just holds the thing.
unsigned int touched_this_row;                     // flag that is set each time MG_computeMeta is called and tells the parent function what to set the refresh bit to.
unsigned int MG_currentFrame;                      // Frame value bounded 0-3 which determines which frames of animation data are referenced to compute the output text.

// For a given input species_charset (being the animation data from the EVO object) and meta_placements (a string from the metaanimation),
// compute a given row. This function is going to be subject to further development that is likely to add arguments, which is why it is not exposed outside
// this package.
char* MG_computeMeta(const char* species_charset, const char* meta_placements, const unsigned int which_row){
    unsigned int charindex = 0;
    unsigned int this_char_tracker;
    touched_this_row = false;
    unsigned int reversal_array[4] = {1, 0, 3, 2};
    while (charindex < PIXELS_X/16){ // this assumes the 16x16 graphical font is being used.
        if (meta_placements[charindex] == '-' || meta_placements[charindex] == '_'){ // Dashes are empty cells, underscores are reversed empty cells
             playfield[which_row][charindex] = ' ';
        }
        else if (meta_placements[charindex] == '0' || meta_placements[charindex] == '2'){ // Zeros say "put the character here, unmodified", 2 is color inverted
            playfield[which_row][charindex] = species_charset[char_tracker];
            char_tracker++; // we can go to the next character in the frame
            char_tracker = char_tracker % icon_size; // Here to prevent out of index crashes. You should never define an animation that violates this.
            touched_this_row = true; // Because the animations are two-frame, if we had to put a character in at all, we have to rewrite that line.
        }
        else if (meta_placements[charindex] == '1' || meta_placements[charindex] == '3'){ // 1 says to put the character here, reversed along the x; 3 says to do that and color invert
            this_char_tracker = reversal_array[char_tracker]; // In this case the characters need to arrange differently.
            this_char_tracker = this_char_tracker % icon_size;
            playfield[which_row][charindex] = species_charset[this_char_tracker];
            char_tracker++; // we can go to the next character in the frame
            char_tracker = char_tracker % icon_size; // Here to prevent out of index crashes. You should never define an animation that violates this.
            touched_this_row = true; // Because the animations are two-frame, if we had to put a character in at all, we have to rewrite that line.
        }
        charindex++;

    }
    return playfield[which_row];
}

//Computes a printTextLarge_Enhanced display directives string based on the metanimation string provided
char* MG_computeDirective(const char* meta_placements, const unsigned int which_row){
    unsigned int dirindex = 0;
    while (dirindex < PIXELS_X/16){
        if (meta_placements[dirindex] == '-'){
            directives_out[which_row][dirindex] = '0';
        }
        else if (meta_placements[dirindex] == '_'){
            directives_out[which_row][dirindex] = '2'; // These underscores are Dark cells
        }
        else {
            directives_out[which_row][dirindex] = meta_placements[dirindex];
        }
        dirindex++;
    }
    return directives_out[which_row];
}

//Parent function that keeps track of what frame of animation we're on and uses MG_updatePlayfield
// to determine the content of each output frame in the main 6 rows of the display, as well as which
// of those flags need to be reset.
void MG_updatePlayfield(void){
    Stage active_species = EVO_metaStruct[StateMachine.STAGE_ID];
    icon_size = active_species.size; // This will either be 1 or 4 in the current spec.
    if (FORCE_REFRESH){ // In this case we're freshly arriving so let's start at 0
        MG_currentFrame = 0x00;
    }
    Metanimation active_meta = SCENES_metanimations[active_species.phase]; // Get the metanimation needed based on the `phase` value of the current active species.
    const char *species_anim = active_species.animation[MG_currentFrame % 2]; // Get the right frame of animation data from the active species.
    char_tracker = 0;
    MainGameFrame.line1 = MG_computeMeta(species_anim, active_meta.d[MG_currentFrame][0], 0);
    MainGameFrame.refresh_L1 = touched_this_row;
    MainGameFrame.directive_L1 = MG_computeDirective(active_meta.d[MG_currentFrame][0], 0);
    MainGameFrame.line2 = MG_computeMeta(species_anim, active_meta.d[MG_currentFrame][1], 1);
    MainGameFrame.refresh_L2 = touched_this_row;
    MainGameFrame.directive_L2 = MG_computeDirective(active_meta.d[MG_currentFrame][1], 1);
    MainGameFrame.line3 = MG_computeMeta(species_anim, active_meta.d[MG_currentFrame][2], 2);
    MainGameFrame.refresh_L3 = touched_this_row;
    MainGameFrame.directive_L3 = MG_computeDirective(active_meta.d[MG_currentFrame][2], 2);
    MainGameFrame.line4 = MG_computeMeta(species_anim, active_meta.d[MG_currentFrame][3], 3);
    MainGameFrame.refresh_L4 = touched_this_row;
    MainGameFrame.directive_L4 = MG_computeDirective(active_meta.d[MG_currentFrame][3], 3);
    MainGameFrame.line5 = MG_computeMeta(species_anim, active_meta.d[MG_currentFrame][4], 4);
    MainGameFrame.refresh_L5 = touched_this_row;
    MainGameFrame.directive_L5 = MG_computeDirective(active_meta.d[MG_currentFrame][4], 4);
    MainGameFrame.line6 = MG_computeMeta(species_anim, active_meta.d[MG_currentFrame][5], 5);
    MainGameFrame.refresh_L6 = touched_this_row;
    MainGameFrame.directive_L6 = MG_computeDirective(active_meta.d[MG_currentFrame][5], 5);
    MG_currentFrame++;
    MG_currentFrame = MG_currentFrame % 4; // Automatic index rollover because manual coding sucks.
}

// Called once per cicle to update the menu and playfield based on their various subfunctions for update.
// Some functions (those used for menuing) are not defined and present only as comments.
void MG_computeNextFrame(void){
    if (StateMachine.STAGE_ID != 0x00){ // Eggs don't get menus
        //MainGameFrame.line0 = MG_printTopMenu();
        //MainGameFrame.line9 = MG_printBottomMenu();
    }
    else{
        MainGameFrame.line0 = " ";
        MainGameFrame.line9 = " ";
        MainGameFrame.line8 = " ";
    }
    MG_updatePlayfield(); // Animations are gross, so this also handles the
}

// A main function to be called by the scene_manger to handle the inputs and then compute the next frame of animation, before sending all the updates to
// DISPLAY. Handling the inputs itself can set the exit conditions for this scene so there is no need for special exit logic.
void SCENE_main_game(void){
    //MG_handleInputs(); Currently undefinied because the egg won't use it and these are just tests.
    MG_computeNextFrame();
    DISPLAY_updatesOnly(MainGameFrame, MODE_GAME);

}
