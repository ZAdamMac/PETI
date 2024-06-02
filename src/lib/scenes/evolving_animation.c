#include "lib/display/display.h"
#include "lib/game/game_manager.h"
#include "lib/game/evo_data.h"
#include "evolving_animation.h"
#include "scenes_manager.h"

//Render one frame of the evolution animation.
//expects an index from EVO_metaStruct for drawn_specieis, and a valid DISPLAY directive symbol for directive.
void EVOLVE_render_frame(unsigned int drawn_species, unsigned int directive){
    //This is clumsy but it'll work for now.
    Stage active_species;
    unsigned int stepframe, row, col;
    active_species = EVO_metaStruct[drawn_species];
    stepframe = SCENE_FRAME %2; // Animations are only two frames long in the context of evo_data.

    switch (active_species.size){
        case EVO_size_small:
            DISPLAY_FRAME.frame[4].line[3] = active_species.animation[stepframe][0];
            break;
        case EVO_size_med:
            DISPLAY_FRAME.frame[3].line[3] = active_species.animation[stepframe][0];
            DISPLAY_FRAME.frame[3].line[4] = active_species.animation[stepframe][1];
            DISPLAY_FRAME.frame[4].line[3] = active_species.animation[stepframe][2];
            DISPLAY_FRAME.frame[4].line[4] = active_species.animation[stepframe][3];
            break;
        case EVO_size_large:
            DISPLAY_FRAME.frame[3].line[2] = active_species.animation[stepframe][0];
            DISPLAY_FRAME.frame[3].line[3] = active_species.animation[stepframe][1];
            DISPLAY_FRAME.frame[3].line[4] = active_species.animation[stepframe][2];
            DISPLAY_FRAME.frame[4].line[2] = active_species.animation[stepframe][3];
            DISPLAY_FRAME.frame[4].line[3] = active_species.animation[stepframe][4];
            DISPLAY_FRAME.frame[4].line[4] = active_species.animation[stepframe][5];
            DISPLAY_FRAME.frame[5].line[2] = active_species.animation[stepframe][6];
            DISPLAY_FRAME.frame[5].line[3] = active_species.animation[stepframe][7];
            DISPLAY_FRAME.frame[5].line[4] = active_species.animation[stepframe][8];
            break;
    }

    for (row=0; row<(PIXELS_Y/FONT_SIZE_FLOOR_Y); row++){
        for (col=0; col<(PIXELS_X/FONT_SIZE_FLOOR_X); col++){
            //What follows is an inconvenient hack that relies on the scene definition for MODE_GAME never changing
            if ( row > 0 && row < 7){ // We are in the "playing field".
                DISPLAY_FRAME.frame[row].directives[col] = active_species.font + directive;
            }
            else { // We are in a text field and need to use the text font.
                DISPLAY_FRAME.frame[row].directives[col] = FONT_ADDR_0 + directive;
            }
        }
    }
}

//Special wrapper on the evolution animation to handle detecting the threshold between the old and new
//stage IDs, as well as control which step of the directive is used.
void EVOLVE_add_flourish_directives(void){
    //This is ALSO clumsy, but workable for testing purposes
    unsigned int use_id, use_directive, row, col;

    if (SCENE_FRAME < 4){
        use_id = StateMachine.OLD_STAGE_ID;
    }
    else {  // We are at the point in the animation where we want to use the old id.
        use_id = StateMachine.STAGE_ID;
    }

    switch (SCENE_FRAME){
        case 0:
            use_directive = DIRECTIVE_SLATS_THIN;
            break;
        case 1:
            use_directive = DIRECTIVE_SLATS_MED;
            break;
        case 2:
            use_directive = DIRECTIVE_SLATS_THICK;
            break;
        case 3:
            use_directive = DIRECTIVE_BLACKOUT;
            break;
        case 4:
            use_directive = DIRECTIVE_SLATS_THICK;
            break;
        case 5:
            use_directive = DIRECTIVE_SLATS_MED;
            break;
        case 6:
            use_directive = DIRECTIVE_SLATS_THIN;
            SCENE_CURSOR_POS = 1;
            break;
    }
        EVOLVE_render_frame(use_id, use_directive);
}

//Core wrappwer for the entire scene. This is the only part that should be called from outside.
void SCENE_Evolving(void){
    //FUTURE: Integrate audio in here somewhere, when the audio system is firmer
    DISPLAY_blankFrame();
    switch(SCENE_CURRENT_PAGE){
        case EVOLVE_PAGE_IDLE:
            EVOLVE_render_frame(StateMachine.OLD_STAGE_ID, DIRECTIVE_NORMAL);
            if (SCENE_FRAME >= EVOLVE_IDLE_FRAMES){
                SCENE_CURRENT_PAGE = EVOLVE_PAGE_EVOLVING;
                SCENE_FRAME = -1;
            }
            break;
        case EVOLVE_PAGE_EVOLVING:
            EVOLVE_add_flourish_directives();
            if (SCENE_CURSOR_POS){  // We have actually evolved already once.
                SCENE_CURRENT_PAGE = EVOLVE_PAGE_IDLE2;
                SCENE_FRAME = -1;
            }
            break;
        case EVOLVE_PAGE_IDLE2:
            EVOLVE_render_frame(StateMachine.STAGE_ID, DIRECTIVE_NORMAL);
            if (SCENE_FRAME >= EVOLVE_IDLE_FRAMES){
                SCENE_EXIT_FLAG = 1;
            }
            break;
    }
    DISPLAY_updatesOnly_enhanced(&DISPLAY_FRAME, MODE_GAME);
    if (SCENE_EXIT_FLAG) {
        SCENE_ACT = SCENEADDR_main_game;
        SCENE_FRAME = 0;
    }
    else {
        SCENE_FRAME++;
    }

}