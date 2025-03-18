/*
 * main_game.h
 *
 *  Created on: May 12, 2021
 *      Author: patches
 */

#ifndef LIB_SCENES_MAIN_GAME_H_
#define LIB_SCENES_MAIN_GAME_H_


#define MG_poop_stack_width 2           //Maximum width of the poop pile, when drawn.

#define MG_sleep_display_cycles 30      //Value which counts how many MPLs to wait before shutting off the display
                                        //when the lights are off and the pet is asleep

extern unsigned int MG_lights_on;

void SCENE_main_game(void);


#endif
