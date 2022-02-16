/*
 * main_game.h
 *
 *  Created on: Oct. 26, 2021
 *      Author: patches
 */

#ifndef LIB_MENUS_MAIN_GAME_H_
#define LIB_MENUS_MAIN_GAME_H_

#pragma PERSISTENT(MG_count_menu_icons)
const int MG_count_menu_icons = 6; // length of the two arrays below (they must match!)

#pragma PERSISTENT(MG_menu_icons)
const char MG_menu_icons[6] = {
                               0x09, // Weight Scale Icon - Maps to the Status Menu
                               0x07, // Wrench Icon - Maps to the Settings Menu
                               0x08, // Knife/Fork Icon - Maps to the Feeding Menu
                               0x0A, // Smile Icon - Maps to the Games Menu
                               0x0C, // Calendar Menu - Maps to the Calendar Display
                               0x0B // Beetle Icon - Maps to the Debug Menu
};

#pragma PERSISTENT(MG_menu_scene_addresses)
const int MG_menu_scene_addresses[6] = {
                                      SCENEADDR_status_menu, // Scene Address for Status Menu
                                      SCENEADDR_proof_text,  // Placeholder; when implemented, goto settings menu
                                      SCENEADDR_feed_menu,  // Placeholder; when implemented, goto feeding menu
                                      SCENEADDR_proof_text,  // Placeholder; when implemented, goto feeding menu
                                      SCENEADDR_calendar_menu,// Placeholder; when implemented, goto clock screen
                                      SCENEADDR_debug_menu  // Scene Address for the Debug Menu
};



#endif /* LIB_MENUS_MAIN_GAME_H_ */
