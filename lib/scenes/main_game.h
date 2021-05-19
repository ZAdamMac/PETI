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

typedef struct MenuIcon {   // Much like with main game and metanimations we're going to have a structure of icons.
    char IconChar;          // this allows you to add a new menu item to an array by just defining its character from the 8x12 font
    unsigned int DropTo;    // and a SCENEADDR symbol (or its numerical value) which handles that function
}MenuIcon;


#endif /* LIB_SCENES_MAIN_GAME_H_ */
