//******************************************************************************
//  PETI FOSS Virtual Pet - Animation for Bathing the Pet
//
//  Scenes Module for animating the pet being bathed.
//
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  March 2025
//******************************************************************************

#include <msp430.h>
#include <string.h>
#include "driverlib/MSP430FR5xx_6xx/driverlib.h"
#include "lib/display/display.h"
#include "lib/display/icons.h"
#include "lib/game/game_manager.h"
#include "lib/scenes/scenes_manager.h"


// Compute one line of the bathing animation.
char * BATH_drawLine(unsigned int row){
    unsigned int col;
    // We actually want to just draw this right on top of the main game screen
    // We can only get here from the main game screen.
    // Therefore we can copy the screen out and modify it.
    strcpy(WORK_STRING, &(DISPLAY_FRAME.frame[row].line));
    for (col=0; col<8; col++){
        if (col == SCENE_FRAME){ // Column is the frame we're on.
            WORK_STRING[col] = ICON_BATH_EDGE;  //In this case, water is HERE
        }
        else if (col < SCENE_FRAME){
            WORK_STRING[col] = ICON_BATH_FULL; // Water *was* here
        }
        // Implied third else: if greater than the SCENE_FRAME, water isn't there yet.
        // This creates the left-right flow of the wave.
    }
    //Finally, put it back
    return &WORK_STRING;
}


// compute one line of the bathing animation's directives
char * BATH_directLine(unsigned int row){
    unsigned int col;
    // We actually want to just draw this right on top of the main game screen
    // We can only get here from the main game screen.
    // Therefore we can copy the screen out and modify it.
    strcpy(WORK_STRING, &(DISPLAY_FRAME.frame[row].directives));
    for (col=0; col<8; col++){
        if (col <= SCENE_FRAME){ // Column is the frame we're on.
            WORK_STRING[col] = DIRECTIVE_NORMAL + ICON_BATH_FONT;  //In this case, water is HERE
        }
        // Implied else: if greater than the SCENE_FRAME, water isn't there yet. Do not change.
    }
    //Finally, put it back
    return &WORK_STRING;
}


// Iterate over the 6 rows using Large Text after blanking the frame,
// and calls the function to draw and direct each row.
void BATH_computeNextFrame(){
    unsigned int row;
    for (row=1; row<7; row++){
        strcpy(DISPLAY_FRAME.frame[row].line, BATH_drawLine(row));
        strcpy(DISPLAY_FRAME.frame[row].directives, BATH_directLine(row));
    }
}

// Convenience wrapper to add to Scenes_Manager to make this scene callable.
void SCENE_bathtime(void){
    BATH_computeNextFrame();
    if (SCENE_FRAME > 9) {
        StateMachine.POOP_COUNT = 0;
        SCENE_ACT = SCENEADDR_main_game;
        SCENE_FRAME = 0;
    }
    else {
        SCENE_FRAME++;
    }
    DISPLAY_updatesOnly_enhanced(&DISPLAY_FRAME, MODE_GAME);
}





