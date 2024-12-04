//***************************************************************************************
//  PETI FOSS Virtual Pet - Rock Paper Scissors Minigame
//
//  The Pet can play Rock Paper Scissors against he player to potentially increase
//  the happiness score and to help regulate physical health states. The game is
//  played best-of-five.
//
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  April 2023
//***************************************************************************************

#include "lib/display/display.h"
#include "lib/hwinit/hwinit.h"
#include "lib/hwinit/human_input.h"
#include "driverlib.h"
#include <msp430.h>
#include <string.h>
#include "main.h"
#include "lib/locales/enCA_strings.h"
#include "lib/scenes/scenes_manager.h"
#include "lib/scenes/menu_generator.h"
#include "lib/game/evo_data.h"
#include "lib/game/game_manager.h"
#include "lib/game/entropy.h"
#include "lib/alerts/audio.h"


// By defining the choice values in this way, we map them directly to the
// charater positions in FONT_ADDRESS_0 that represents their symbol.
// This makes things cleaner when it comes to other functions later on.
#define MINIGAME_RPS_choiceNotMade '\xF0'
#define MINIGAME_RPS_choiceRock '\xF7'
#define MINIGAME_RPS_choicePaper '\xF6'
#define MINIGAME_RPS_choiceScissors '\xF5'

// Might as well define some other font symbols we use too.
// These are from font16x16 which is font_address_0 for the large font size
#define MINIGAME_RPS_entry_arrow '\xED'
#define MINIGAME_RPS_sb_solidbottom '\xEE'
#define MINIGAME_RPS_sb_side '\xEF'         
#define MINIGAME_RPS_sb_top '\xF1'
#define MINIGAME_RPS_sb_tl '\xF2'
#define MINIGAME_RPS_sb_bl '\xF3'
#define MINIGAME_RPS_sb_pointbottom '\xF4'

// For the sake of convenience, set the midline of the sprite's dances.
#define MINIGAME_RPS_sprite_centerline 5

char MINIGAME_RPS_choice;
char MINIGAME_RPS_petChoice;

signed int MINIGAME_RPS_score;

// This scene will set SCENE_current_page to one of the following depending on 
// what step of the animation we're in.
#define MINIGAME_RPS_attract 0      // Waiting for user input.
#define MINIGAME_RPS_eval 1         // Computing and displaying the result of a move
#define MINIGAME_RPS_final_page 2   // Computing and displaying the result of the gameplay.

// This scene absuses SCENE_PAGE_COUNT as a general purpose store to keep track
// of how many rounds the user has played. If it's equal to the value defined
// it changes state to scoring the game.
#define MINIGAME_RPS_best_of 5

/// For balance reasons, we may want to change how much happiness is granted for each successful play.
#define MINIGAME_RPS_prize 5

// Because the beeping might be annoying, we detect an idle and go back to the main game.
// This is also good for power management later on in development
#define MINIGAME_RPS_idleLimit 16

//This defines the length in frames of each of the three phases of the Evaluation screen.
//At time of writing this is roughly the number of half-seconds each screen should last.
#define MINIGAME_RPS_EVALSCREENLENGTH 4

// We need our own scene-specific input handling, which will probably almost always be the case for scenes.
void MINIGAME_RPS_handleInputs(void){
    int i;
    unsigned int this_event;
    for (i =0; i <= HID_input_events_queue_depth; ++i) // For every event in the event queue;
    {
        this_event = HID_input_events_queue[i]; //fetch the event.
        switch (this_event){
            case(BUTTON_A_PRESS): // Select for Rock
		        MINIGAME_RPS_choice = MINIGAME_RPS_choiceRock;
                SCENE_CURRENT_PAGE = MINIGAME_RPS_eval;
                SCENE_FRAME = 0;
            	break;
            case(BUTTON_B_PRESS): // Select for Paper
		        MINIGAME_RPS_choice = MINIGAME_RPS_choicePaper;
                SCENE_CURRENT_PAGE = MINIGAME_RPS_eval;
                SCENE_FRAME = 0;
                break;
            case(BUTTON_C_PRESS): //Select for Scissors
		        MINIGAME_RPS_choice = MINIGAME_RPS_choiceScissors;
                SCENE_CURRENT_PAGE = MINIGAME_RPS_eval;
                SCENE_FRAME = 0;
                break;
            case(BUTTON_D_PRESS): // Exit
	            SCENE_ACT = SCENEADDR_main_game;
                SCENE_CURRENT_PAGE = MINIGAME_RPS_eval;
                SCENE_FRAME = 0;
                break;
        }
        HID_input_events_queue[i] = BUTTON_NO_PRESS;
    };
    HID_input_events_queue_depth = 0;
}


char* MINIGAME_RPS_drawChoices(char choice_a, char choice_b){
    int col;
    for (col = 0; col < PIXELS_X/16; col++){
        WORK_STRING[col] = ' '; // For some reason, setting this iteratively is the only way I've been able to make it work.
    }
    col = (PIXELS_X/(2*FONT_SIZE_FLOOR_X))-1; // I say dear fellow, what is the last spot in the array?
    WORK_STRING[0] = MINIGAME_RPS_entry_arrow;
    WORK_STRING[col] = MINIGAME_RPS_entry_arrow;
    col--; // And the one before that?
    WORK_STRING[1] = choice_a;
    WORK_STRING[col] = choice_b;

    return &WORK_STRING;
}


char* MINIGAME_RPS_choiceDirective(){
    int col;
    //FUTURE: convenience function for this
    for (col = 0; col < PIXELS_X/16; col++){
        WORK_STRING[col] = FONT_ADDR_0 + DIRECTIVE_NORMAL; // For some reason, setting this iteratively is the only way I've been able to make it work.
    }
    WORK_STRING[0] = FONT_ADDR_0 + DIRECTIVE_REVERSED;

    return &WORK_STRING;
}


char* MINIGAME_RPS_drawWideLine(void){
    int col;
    //We don't need to blank the workstring because we are writing to the whole thing at once.
    for (col=0; col<PIXELS_X/16; col++){
        WORK_STRING[col] = MINIGAME_RPS_sb_solidbottom; // Easiest to draw the line first
    }
    col = (PIXELS_X/(2*FONT_SIZE_FLOOR_X))/2; // and then place the arrow
    WORK_STRING[col] = MINIGAME_RPS_sb_pointbottom;

    return &WORK_STRING;
}


char* MINIGAME_RPS_drawDance(int row_offset, int width, char facing){
    int col, offset, char_index;
    Stage active_species = EVO_metaStruct[StateMachine.STAGE_ID];
    if (width == 1) {
        offset = ((PIXELS_X/(2*FONT_SIZE_FLOOR_X))/2); // Lots of fudge-factor in here for prettyness
    }
    else {
        offset = ((PIXELS_X/(2*FONT_SIZE_FLOOR_X))/2)-2; // Lots of fudge-factor in here for prettyness
    }
    for (col = 0; col < PIXELS_X/16; col++){
        WORK_STRING[col] = ' '; // For some reason, setting this iteratively is the only way I've been able to make it work.
    }
    switch (facing){  // Special math is needed to do this. In other scenes this is handled with lookup arrays.
        case DIRECTIVE_NORMAL:
            char_index = width*row_offset;
            for (col = 0; col<width; col++){
                WORK_STRING[col+offset] = active_species.faceRight[char_index];
                char_index++;
            }
            break;
        case DIRECTIVE_REVERSED: // In this case we're actually counting back from the end of this row rather than up.
            char_index = width*(row_offset+1)-1;
            for (col = 0; col<width; col++){
                WORK_STRING[col+offset] = active_species.faceRight[char_index];
                char_index--;
            }
            break;
    }

    return &WORK_STRING;
}


char* MINIGAME_RPS_directDance(char facing){
    int col;
    Stage active_species = EVO_metaStruct[StateMachine.STAGE_ID];
    for (col = 0; col < PIXELS_X/(2*FONT_SIZE_FLOOR_X); col++){
        WORK_STRING[col] = active_species.font + facing;
    }

    return &WORK_STRING;
}


//Called without argument, this function draws one frame of the attract mode
//animation. On frame zero it also draws the LSTRING_MINIGAME_PICK and decorates
//it alongside the legend of button choices.
void MINIGAME_RPS_attractFrame(void){ // FUTURE: This could be more elegant
    int row, col, facing;
    Stage active_species = EVO_metaStruct[StateMachine.STAGE_ID];
    if ( SCENE_FRAME == 0 ) {   // This is the "first" frame so we want to treat it as such.
        DISPLAY_blankFrame(); // Which is easier to do from a blank canvas.
        //A few lines never change and can just be set the first time we hit the attractFrame:
        strcpy(DISPLAY_FRAME.frame[0].line, LSTRING_MINIGAME_PICK); // Line 0 is the attract string
        strcpy(DISPLAY_FRAME.frame[1].line, MINIGAME_RPS_drawWideLine()); // Line 1 is the speech bubble under it
        strcpy(DISPLAY_FRAME.frame[2].line, MINIGAME_RPS_drawChoices(MINIGAME_RPS_choiceRock, MINIGAME_RPS_choiceScissors));  // The Rock and Scissors choices
        strcpy(DISPLAY_FRAME.frame[2].directives, MINIGAME_RPS_choiceDirective());
        strcpy(DISPLAY_FRAME.frame[6].line, MINIGAME_RPS_drawChoices(MINIGAME_RPS_choicePaper, MINIGAME_RPS_choiceNotMade)); // The Paper and Cancel choices
        strcpy(DISPLAY_FRAME.frame[6].directives, MINIGAME_RPS_choiceDirective());
        strcpy(DISPLAY_FRAME.frame[7].line, " ");
        strcpy(DISPLAY_FRAME.frame[8].line, " ");
    }

    // Only the facing and the length of the attract tone change frame-to-frame.
    if ((SCENE_FRAME % 2) == 0) {
        // Set the character to face RIGHT.
        facing = DIRECTIVE_NORMAL;
        AUDIO_pulse(AUDIO_LONG_PULSE);
    }
    else {
        //set the character to face LEFT
        facing = DIRECTIVE_REVERSED;
        AUDIO_pulse(AUDIO_SHORT_PULSE);
    }

    // now that we know our facing we're free to draw in our character sprites
    switch (active_species.size) {
        case EVO_size_small:
            strcpy(DISPLAY_FRAME.frame[MINIGAME_RPS_sprite_centerline].line, MINIGAME_RPS_drawDance(0,1,facing));
            strcpy(DISPLAY_FRAME.frame[MINIGAME_RPS_sprite_centerline].directives, MINIGAME_RPS_directDance(facing));
            break;
        case EVO_size_med:
            strcpy(DISPLAY_FRAME.frame[MINIGAME_RPS_sprite_centerline-1].line, MINIGAME_RPS_drawDance(0,2,facing));
            strcpy(DISPLAY_FRAME.frame[MINIGAME_RPS_sprite_centerline-1].directives, MINIGAME_RPS_directDance(facing));
            strcpy(DISPLAY_FRAME.frame[MINIGAME_RPS_sprite_centerline].line, MINIGAME_RPS_drawDance(1,2,facing));
            strcpy(DISPLAY_FRAME.frame[MINIGAME_RPS_sprite_centerline].directives, MINIGAME_RPS_directDance(facing));
            break;
        case EVO_size_large:
            strcpy(DISPLAY_FRAME.frame[MINIGAME_RPS_sprite_centerline-2].line, MINIGAME_RPS_drawDance(0,3,facing));
            strcpy(DISPLAY_FRAME.frame[MINIGAME_RPS_sprite_centerline-2].directives, MINIGAME_RPS_directDance(facing));
            strcpy(DISPLAY_FRAME.frame[MINIGAME_RPS_sprite_centerline-1].line, MINIGAME_RPS_drawDance(1,3,facing));
            strcpy(DISPLAY_FRAME.frame[MINIGAME_RPS_sprite_centerline-1].directives, MINIGAME_RPS_directDance(facing));
            strcpy(DISPLAY_FRAME.frame[MINIGAME_RPS_sprite_centerline].line, MINIGAME_RPS_drawDance(2,3,facing));
            strcpy(DISPLAY_FRAME.frame[MINIGAME_RPS_sprite_centerline].directives, MINIGAME_RPS_directDance(facing));
            break;
    }
    
    if (SCENE_FRAME >= MINIGAME_RPS_idleLimit) {
        SCENE_ACT = SCENEADDR_main_game;
    }

    SCENE_FRAME++;
}

//Fed a random float, translates to an MINIGAME_RPS_choice Binding.
//WARN: Not necessarily balanced because the homogeneity of the randomness
//generator is not understood.
void MINIGAME_RPS_drawChoice(float random){
        if (random > 0.6666){
            MINIGAME_RPS_petChoice = MINIGAME_RPS_choiceRock;
        }
        else if (random > 0.3333){
            MINIGAME_RPS_petChoice = MINIGAME_RPS_choicePaper;
        }
        else {
            MINIGAME_RPS_petChoice = MINIGAME_RPS_choiceScissors;
        }
}

//Checks the player choice against the pet's random selection and modifies
//MINIGAME_RPS_score accordingly. The score is positively incremented on a win
//for the player.
int MINIGAME_RPS_evaluateForScoring(void){
    int score_change;
    SCENE_PAGE_COUNT++;
    if (MINIGAME_RPS_choice == MINIGAME_RPS_petChoice){ // This is the general case of a draw.
        MINIGAME_RPS_score += 0;
        score_change = 0;
    }
    else { // Handling actual disparities is a little more branchy
        switch(MINIGAME_RPS_choice){
            case (MINIGAME_RPS_choiceRock):
                if (MINIGAME_RPS_petChoice == MINIGAME_RPS_choiceScissors) {
                    MINIGAME_RPS_score += 1; // You won the hand
                    score_change = 1;
                }
                else {
                    MINIGAME_RPS_score += -1; // You have necessarily lost the hand
                    score_change = -1;
                }
                break;
            case (MINIGAME_RPS_choicePaper):
                if (MINIGAME_RPS_petChoice == MINIGAME_RPS_choiceRock) {
                    MINIGAME_RPS_score += 1; // You won the hand
                    score_change = 1;
                }
                else {
                    MINIGAME_RPS_score += -1; // You have necessarily lost the hand
                    score_change = -1;
                }
                break;
            case (MINIGAME_RPS_choiceScissors):
                if (MINIGAME_RPS_petChoice == MINIGAME_RPS_choicePaper) {
                    MINIGAME_RPS_score += 1; // You won the hand
                    score_change = 1;
                }
                else {
                    MINIGAME_RPS_score += -1; // You have necessarily lost the hand
                    score_change = -1;
                }
                break;
        }
    }

    return score_change;
}

//Minimalistic evaluation function that displays the two choices made by the
//player and the pet. Future work will expand this to include a visual
//legend of which player is on which side.
// FUTURE: This could be generalized, but it's not breaking anything and it's actually reasonably elegant.
void MINIGAME_RPS_evaluateFrame_choices(void){
    int row;
    DISPLAY_FRAME.frame[2].line[0] = MINIGAME_RPS_sb_tl;
    DISPLAY_FRAME.frame[2].line[1] = MINIGAME_RPS_sb_top;
    DISPLAY_FRAME.frame[2].line[2] = MINIGAME_RPS_sb_tl;
    DISPLAY_FRAME.frame[2].line[5] = MINIGAME_RPS_sb_tl;
    DISPLAY_FRAME.frame[2].line[6] = MINIGAME_RPS_sb_top;
    DISPLAY_FRAME.frame[2].line[7] = MINIGAME_RPS_sb_tl;
    DISPLAY_FRAME.frame[3].line[0] = MINIGAME_RPS_sb_side;
    DISPLAY_FRAME.frame[3].line[1] = MINIGAME_RPS_choice;
    DISPLAY_FRAME.frame[3].line[2] = MINIGAME_RPS_sb_side;
    DISPLAY_FRAME.frame[3].line[5] = MINIGAME_RPS_sb_side;
    DISPLAY_FRAME.frame[3].line[6] = MINIGAME_RPS_petChoice;
    DISPLAY_FRAME.frame[3].line[7] = MINIGAME_RPS_sb_side;
    DISPLAY_FRAME.frame[4].line[0] = MINIGAME_RPS_sb_bl;
    DISPLAY_FRAME.frame[4].line[1] = MINIGAME_RPS_sb_pointbottom;
    DISPLAY_FRAME.frame[4].line[2] = MINIGAME_RPS_sb_bl;
    DISPLAY_FRAME.frame[4].line[5] = MINIGAME_RPS_sb_bl;
    DISPLAY_FRAME.frame[4].line[6] = MINIGAME_RPS_sb_pointbottom;
    DISPLAY_FRAME.frame[4].line[7] = MINIGAME_RPS_sb_bl;
    for (row=2; row<5; row++){ // Right hand sides of speech bubbles involve reversal.
        DISPLAY_FRAME.frame[row].directives[2] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
        DISPLAY_FRAME.frame[row].directives[7] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
    }
    //FUTURE not really done, should probably draw the monster under its choice, and a player icon under the other.
}


// A dummied function provided so that space could be left for it inside the
// main evaluateFrame function. It currently does nothing.
void MINIGAME_RPS_evaluateFrame_dance(int winloss){
    //FUTURE fully implement this using winloss to draw win/loss/draw animations.
    return;
}

// Parent function for the evaluate frame subscene. Consults the SCENE_FRAME
// globalvar and the MINIGAME_RPS_EVALSCREENLENGTH to determine whether to call
// choices or dance animation phases.
// FUTURE: when tunes support is added to audio, each of those phases needs a
// jingle.
void MINIGAME_RPS_evaluateFrame(void){
    int row, col, victory;
    if (SCENE_FRAME % MINIGAME_RPS_EVALSCREENLENGTH == 0) {
        {
             DISPLAY_blankFrame(); // In such an event, we need to "blank" the frame directives and lines
        }
        if (SCENE_FRAME == 0){ // Specifically on frame 0 we need to do the actual maths.
            MINIGAME_RPS_drawChoice(RNG_drawFloat());
            victory = MINIGAME_RPS_evaluateForScoring();
        }
    }

    //Determine if we are in the second or first half of the evaluation page animations.
    //This is done to simplify the animation by handling each half as their own function for clarity.
    if (SCENE_FRAME <= MINIGAME_RPS_EVALSCREENLENGTH){ //First lap around the EVALSCREENLENGTH, show the player what was chosen.
        MINIGAME_RPS_evaluateFrame_choices();
    }
    else if (MINIGAME_RPS_EVALSCREENLENGTH < SCENE_FRAME <= (2 * MINIGAME_RPS_EVALSCREENLENGTH)){
        MINIGAME_RPS_evaluateFrame_dance(victory); // Second lap around, character does a little dance.
    }
    

    SCENE_FRAME++;
    if (SCENE_FRAME >= (MINIGAME_RPS_EVALSCREENLENGTH)){
        SCENE_FRAME = 0;
        if (SCENE_PAGE_COUNT >= MINIGAME_RPS_best_of){
            SCENE_CURRENT_PAGE = MINIGAME_RPS_final_page;
        }
        else {
            SCENE_CURRENT_PAGE = MINIGAME_RPS_attract;
        }
        
    }
}

// A simplistic function that either diplays a victory or loss message. Players
// win on draw, see internal comment. It's not finished, but is finished enough
// for a minigame prototype.
// FUTURE work will add jingles when the tune system works.
void MINIGAME_RPS_finalScoreFrame(void){
    //FUTURE a prettier version of this, possibly making use of MINIGAME_RPS_evaluateFrame_dance();
    int row, col;
    if (SCENE_FRAME == 0) { // In such an event, we need to "blank" the frame directives and lines
            for (row = 1; row<PIXELS_Y/(2 * FONT_SIZE_FLOOR_Y); row++){
                for (col=0; col<PIXELS_X/(2 *FONT_SIZE_FLOOR_X); col++){
                    DISPLAY_FRAME.frame[row].directives[col] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                    DISPLAY_FRAME.frame[row].line[col]=' ';
                }
            }
        if (MINIGAME_RPS_score >= 0) { //Players win on a draw. Replace with straight greater-than to make them lose.
            GAME_applyHungerFun(0, MINIGAME_RPS_prize);
            strcpy(DISPLAY_FRAME.frame[0].line, LSTRING_MG_WIN);
        }
        else {
            strcpy(DISPLAY_FRAME.frame[0].line, LSTRING_MG_LOSE);
        }
    }

    SCENE_FRAME++;
    if (SCENE_FRAME >= 6) {
        SCENE_EXIT_FLAG = 1;
    }
}

// This is the public function of the minigame. Any needed localization is in 
//the child functions.
void SCENE_RockPaperScissors(void){
    if (SCENE_PAGE_COUNT == 0){
        MINIGAME_RPS_score = 0;
    }
    if (SCENE_FRAME == 0) {
        MINIGAME_RPS_choice = MINIGAME_RPS_choiceNotMade; // By Default
    }
    if (SCENE_CURRENT_PAGE == MINIGAME_RPS_attract){
        MINIGAME_RPS_handleInputs(); // Will override a MINIGAME_RPS_choice if chosen
        // not to occur in the switch or we never enter other scenes at SCENE_FRAME == 0!
    }
    switch (SCENE_CURRENT_PAGE){
        case MINIGAME_RPS_attract :
            MINIGAME_RPS_attractFrame();
            break;
        case MINIGAME_RPS_eval :
            MINIGAME_RPS_evaluateFrame();
            break;
        case MINIGAME_RPS_final_page :
            MINIGAME_RPS_finalScoreFrame(); 
            break;
    }
    DISPLAY_updatesOnly_enhanced(&DISPLAY_FRAME, MODE_GAME); // Updating the LCD is slow, please update just the parts that matter, and use the MENU layout.
    if (SCENE_EXIT_FLAG) {
        SCENE_ACT = SCENEADDR_minigames_menu;
        SCENE_EXIT_FLAG = 0;
        SCENE_FRAME = 0;
    }
}
