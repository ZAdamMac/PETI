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
char mg_top_alterable[17];
char mg_bottom_alterable[17];
char playfield[6][8];
unsigned int mg_cursor_position = 0x00;            //Cursor position, used to track which digit is being manipulated
unsigned int char_tracker;                         //Charts the count we are at in terms of the icon for the species character animation
unsigned int icon_size;                            //effectively just holds the thing.
unsigned int touched_this_row;
unsigned int MG_currentFrame;

char* MG_computeMeta(const char* species_charset, const char* meta_placements, const unsigned int which_row){
    unsigned int charindex = 0;
    touched_this_row = false;
    while (charindex < PIXELS_X/16){ // this assumes the 16x16 graphical font is being used.
        if (meta_placements[charindex] == '-'){ // Dashes are empty cells
             playfield[which_row][charindex] = ' ';
        }
        else if (meta_placements[charindex] == '0'){ // Zeros say "put the character here, unmodified"
            playfield[which_row][charindex] = species_charset[char_tracker];
            char_tracker++; // we can go to the next character in the frame
            char_tracker = char_tracker % icon_size; // Here to prevent out of index crashes. You should never define an animation that violates this.
            touched_this_row = true; // Because the animations are two-frame, if we had to put a character in at all, we have to rewrite that line.
        }
        charindex++;

    }
    return playfield[which_row];
}


void MG_updatePlayfield(void){
    Stage active_species = EVO_metaStruct[StateMachine.STAGE_ID];
    icon_size = active_species.size; // This will either be 1 or 4 in the current spec.
    if (FORCE_REFRESH){ // In this case we're freshly arriving so let's start at 0
        MG_currentFrame = 0x00;
    }
    Metanimation active_meta = SCENES_metanimations[active_species.phase];
    const char *species_anim = active_species.animation[MG_currentFrame % 2];
    char_tracker = 0;
    MainGameFrame.line1 = MG_computeMeta(species_anim, active_meta.d[MG_currentFrame][0], 0); //RESUME here. The call from active meta is not working as expected
    MainGameFrame.refresh_L1 = touched_this_row;
    MainGameFrame.line2 = MG_computeMeta(species_anim, active_meta.d[MG_currentFrame][1], 1);
    MainGameFrame.refresh_L2 = touched_this_row;
    MainGameFrame.line3 = MG_computeMeta(species_anim, active_meta.d[MG_currentFrame][2], 2);
    MainGameFrame.refresh_L3 = touched_this_row;
    MainGameFrame.line4 = MG_computeMeta(species_anim, active_meta.d[MG_currentFrame][3], 3);
    MainGameFrame.refresh_L4 = touched_this_row;
    MainGameFrame.line5 = MG_computeMeta(species_anim, active_meta.d[MG_currentFrame][4], 4);
    MainGameFrame.refresh_L5 = touched_this_row;
    MainGameFrame.line6 = MG_computeMeta(species_anim, active_meta.d[MG_currentFrame][5], 5);
    MainGameFrame.refresh_L6 = touched_this_row;
    MG_currentFrame++;
    MG_currentFrame = MG_currentFrame % 4; // Automatic index rollover because manual coding sucks.
}


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

void SCENE_main_game(void){
    //MG_handleInputs(); Currently undefinied because the egg won't use it and these are just tests.
    MG_computeNextFrame();
    DISPLAY_updatesOnly(MainGameFrame, MODE_GAME);

}
