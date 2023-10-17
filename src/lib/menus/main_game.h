/*
 * main_game.h
 *
 *  Created on: Oct. 26, 2021
 *      Author: patches
 */

#ifndef LIB_MENUS_MAIN_GAME_H_
#define LIB_MENUS_MAIN_GAME_H_

#pragma PERSISTENT(MG_count_menu_icons)
//FUTURE: Remove all these similar class of calls; replace with sizeof calls.
const int MG_count_menu_icons = 7; // length of the two arrays below (they must match!)

#pragma PERSISTENT(MG_menu_icons)
const char MG_menu_icons[7] = {
                               0x09, // Weight Scale Icon - Maps to the Status Menu
                               0x08, // Knife/Fork Icon - Maps to the Feeding Menu
                               0x0A, // Smile Icon - Maps to the Games Menu
                               0x17, // Lightbulb Menu - Maps to the lightswitch function
                               0x0C, // Calendar Menu - Maps to the Calendar Display
                               0x07, // Wrench Icon - Maps to the Settings Menu
                               0x0B // Beetle Icon - Maps to the Debug Menu
};

extern voidFuncPointer MG_menu_options_array[7];

#endif /* LIB_MENUS_MAIN_GAME_H_ */
