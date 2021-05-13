/*
 * main_game.h
 *
 *  Created on: May 12, 2021
 *      Author: patches
 */

#ifndef LIB_SCENES_MAIN_GAME_H_
#define LIB_SCENES_MAIN_GAME_H_


unsigned int mg_cursor_position_mod = 0x00;          //update link please with the length of the mg_menu array.
void SCENE_main_game(void);

typedef struct MenuIcon {
    char IconChar;
    unsigned int DropTo;
}MenuIcon;


#endif /* LIB_SCENES_MAIN_GAME_H_ */
