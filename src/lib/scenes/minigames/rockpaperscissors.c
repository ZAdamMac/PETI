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


//Called without argument, this function draws one frame of the attract mode
//animation. On frame zero it also draws the LSTRING_MINIGAME_PICK and decorates
//it alongside the legend of button choices.
void MINIGAME_RPS_attractFrame(void){
    int row, col;
    Stage active_species = EVO_metaStruct[StateMachine.STAGE_ID];
    if ( SCENE_FRAME == 0 ) {   // This is the "first" frame so we want to treat it as such.
        
        for (row = 0; row<PIXELS_Y/FONT_SIZE_FLOOR_Y; row++){
            for (col=0; col<PIXELS_X/FONT_SIZE_FLOOR_X; col++){
                DISPLAY_FRAME.frame[row].line[col] = ' ';
                DISPLAY_FRAME.frame[row].directives[col] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
        }
    }   //Draw in the permanent static assets, starting with the title bar.
        strcpy(DISPLAY_FRAME.frame[0].line, LSTRING_MINIGAME_PICK); // Set line zero's full text to the attract prompt.
        for (col=0; col<PIXELS_X/FONT_SIZE_FLOOR_X; col++){
            DISPLAY_FRAME.frame[1].line[col] = MINIGAME_RPS_sb_solidbottom;
        }
        DISPLAY_FRAME.frame[1].line[3] = MINIGAME_RPS_sb_pointbottom; // It is not possible to exactly center this, so

        //The next four lines add in the choice icons to prompt the player which buttons do what.
        DISPLAY_FRAME.frame[2].line[1] = MINIGAME_RPS_choiceRock;
        DISPLAY_FRAME.frame[2].line[6] = MINIGAME_RPS_choiceScissors;
        DISPLAY_FRAME.frame[6].line[1] = MINIGAME_RPS_choicePaper;
        DISPLAY_FRAME.frame[6].line[6] = MINIGAME_RPS_choiceNotMade;
        //The next four lines add the corresponding arrow icons.
        DISPLAY_FRAME.frame[2].line[0] = MINIGAME_RPS_entry_arrow;
        DISPLAY_FRAME.frame[2].directives[0] = FONT_ADDR_0 + DIRECTIVE_REVERSED;  // The font provides no left arrow.
        DISPLAY_FRAME.frame[2].line[7] = MINIGAME_RPS_entry_arrow;
        DISPLAY_FRAME.frame[6].line[0] = MINIGAME_RPS_entry_arrow;
        DISPLAY_FRAME.frame[6].directives[0] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
        DISPLAY_FRAME.frame[6].line[7] = MINIGAME_RPS_entry_arrow;

        //Draw in the character figure. We can do this because the character model is only ever in FaceRight, and
        //FaceRight has only a single frame of animation data.
        switch (active_species.size) {
            case EVO_size_small:
                DISPLAY_FRAME.frame[4].line[3] = active_species.faceRight[0];
                break;
                
        }
        // Let's never do this again until the next time scene-frame is reset.
        SCENE_FRAME = 1;
    }

    if ((SCENE_FRAME % 2) == 0) {
        // Set the character to face RIGHT.
        switch (active_species.size) {
            case EVO_size_small:
                DISPLAY_FRAME.frame[4].directives[3] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                break;
            case EVO_size_med:
                DISPLAY_FRAME.frame[3].line[3] = active_species.faceRight[0];
                DISPLAY_FRAME.frame[3].line[4] = active_species.faceRight[1];
                DISPLAY_FRAME.frame[4].line[3] = active_species.faceRight[2];
                DISPLAY_FRAME.frame[4].line[4] = active_species.faceRight[3];
                DISPLAY_FRAME.frame[3].directives[3] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                DISPLAY_FRAME.frame[3].directives[4] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                DISPLAY_FRAME.frame[4].directives[3] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                DISPLAY_FRAME.frame[4].directives[4] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                break;
            case EVO_size_large:
                DISPLAY_FRAME.frame[3].line[3] = active_species.faceRight[0];
                DISPLAY_FRAME.frame[3].line[4] = active_species.faceRight[1];
                DISPLAY_FRAME.frame[3].line[5] = active_species.faceRight[2];
                DISPLAY_FRAME.frame[4].line[3] = active_species.faceRight[3];
                DISPLAY_FRAME.frame[4].line[4] = active_species.faceRight[4];
                DISPLAY_FRAME.frame[4].line[5] = active_species.faceRight[5];
                DISPLAY_FRAME.frame[5].line[3] = active_species.faceRight[6];
                DISPLAY_FRAME.frame[5].line[4] = active_species.faceRight[7];
                DISPLAY_FRAME.frame[5].line[5] = active_species.faceRight[8];
                DISPLAY_FRAME.frame[3].directives[3] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                DISPLAY_FRAME.frame[3].directives[4] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                DISPLAY_FRAME.frame[3].directives[5] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                DISPLAY_FRAME.frame[4].directives[3] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                DISPLAY_FRAME.frame[4].directives[4] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                DISPLAY_FRAME.frame[4].directives[5] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                DISPLAY_FRAME.frame[5].directives[3] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                DISPLAY_FRAME.frame[5].directives[4] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                DISPLAY_FRAME.frame[5].directives[5] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                break;      
        }
        SCENE_FRAME++;
        AUDIO_pulse(AUDIO_LONG_PULSE);
    }
    else {
        //set the character to face LEFT
        switch (active_species.size) {
            case EVO_size_small:
                DISPLAY_FRAME.frame[4].directives[3] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
                break;
            case EVO_size_med:
                DISPLAY_FRAME.frame[3].line[4] = active_species.faceRight[0];
                DISPLAY_FRAME.frame[3].line[3] = active_species.faceRight[1];
                DISPLAY_FRAME.frame[4].line[4] = active_species.faceRight[2];
                DISPLAY_FRAME.frame[4].line[3] = active_species.faceRight[3];
                DISPLAY_FRAME.frame[3].directives[3] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
                DISPLAY_FRAME.frame[3].directives[4] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
                DISPLAY_FRAME.frame[4].directives[3] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
                DISPLAY_FRAME.frame[4].directives[4] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
                break;
            case EVO_size_large:
                DISPLAY_FRAME.frame[3].line[5] = active_species.faceRight[0];
                DISPLAY_FRAME.frame[3].line[4] = active_species.faceRight[1];
                DISPLAY_FRAME.frame[3].line[3] = active_species.faceRight[2];
                DISPLAY_FRAME.frame[4].line[5] = active_species.faceRight[3];
                DISPLAY_FRAME.frame[4].line[4] = active_species.faceRight[4];
                DISPLAY_FRAME.frame[4].line[3] = active_species.faceRight[5];
                DISPLAY_FRAME.frame[5].line[5] = active_species.faceRight[6];
                DISPLAY_FRAME.frame[5].line[4] = active_species.faceRight[7];
                DISPLAY_FRAME.frame[5].line[3] = active_species.faceRight[8];
                DISPLAY_FRAME.frame[3].directives[3] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
                DISPLAY_FRAME.frame[3].directives[4] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
                DISPLAY_FRAME.frame[3].directives[5] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
                DISPLAY_FRAME.frame[4].directives[3] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
                DISPLAY_FRAME.frame[4].directives[4] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
                DISPLAY_FRAME.frame[4].directives[5] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
                DISPLAY_FRAME.frame[5].directives[3] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
                DISPLAY_FRAME.frame[5].directives[4] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
                DISPLAY_FRAME.frame[5].directives[5] = FONT_ADDR_0 + DIRECTIVE_REVERSED;
                break;
        }
        SCENE_FRAME++;
        AUDIO_pulse(AUDIO_SHORT_PULSE);
    }

    
    
    if (SCENE_FRAME >= MINIGAME_RPS_idleLimit) {
        SCENE_ACT = SCENEADDR_main_game;
    }


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
    SCENE_PAGE_COUNT++;
    if (MINIGAME_RPS_choice == MINIGAME_RPS_petChoice){ // This is the general case of a draw.
        MINIGAME_RPS_score += 0;
        return 0;
    }
    else { // Handling actual disparities is a little more branchy
        switch(MINIGAME_RPS_choice){
            case (MINIGAME_RPS_choiceRock):
                if (MINIGAME_RPS_petChoice == MINIGAME_RPS_choiceScissors) {
                    MINIGAME_RPS_score += 1; // You won the hand
                    return 1;
                }
                else {
                    MINIGAME_RPS_score += -1; // You have necessarily lost the hand
                    return -1;
                }
                break;
            case (MINIGAME_RPS_choicePaper):
                if (MINIGAME_RPS_petChoice == MINIGAME_RPS_choiceRock) {
                    MINIGAME_RPS_score += 1; // You won the hand
                    return 1;
                }
                else {
                    MINIGAME_RPS_score += -1; // You have necessarily lost the hand
                    return -1;
                }
                break;
            case (MINIGAME_RPS_choiceScissors):
                if (MINIGAME_RPS_petChoice == MINIGAME_RPS_choicePaper) {
                    MINIGAME_RPS_score += 1; // You won the hand
                    return 1;
                }
                else {
                    MINIGAME_RPS_score += -1; // You have necessarily lost the hand
                    return -1;
                }
                break;
        }
    }
}

//Minimalistic evaluation function that displays the two choices made by the
//player and the pet. Future work will expand this to include a visual
//legend of which player is on which side.
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
    if (SCENE_FRAME % MINIGAME_RPS_EVALSCREENLENGTH == 0) { // In such an event, we need to "blank" the frame directives and lines
            for (row = 0; row<PIXELS_Y/FONT_SIZE_FLOOR_Y; row++){
                for (col=0; col<PIXELS_X/FONT_SIZE_FLOOR_X; col++){
                    DISPLAY_FRAME.frame[row].directives[col] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                    DISPLAY_FRAME.frame[row].line[col]=' ';
                }
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
    if (SCENE_FRAME >= (2 * MINIGAME_RPS_EVALSCREENLENGTH)){
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
            for (row = 0; row<PIXELS_Y/FONT_SIZE_FLOOR_Y; row++){
                for (col=0; col<PIXELS_X/FONT_SIZE_FLOOR_X; col++){
                    DISPLAY_FRAME.frame[row].directives[col] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
                    DISPLAY_FRAME.frame[row].line[col]=' ';
                }
            }
        if (MINIGAME_RPS_score >= 0) { //Players win on a draw. Replace with straight greater-than to make them lose.
            GAME_applyHungerFun(0, MINIGAME_RPS_prize);
            strcpy(DISPLAY_FRAME.frame[0].line, "You Win!      ");
        }
        else {
            strcpy(DISPLAY_FRAME.frame[0].line, "You lost...   ");
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
