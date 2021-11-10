/*
 * main_game.h
 *
 *  Created on: Oct. 26, 2021
 *      Author: patches
 */

#ifndef LIB_MENUS_MAIN_GAME_H_
#define LIB_MENUS_MAIN_GAME_H_

#pragma PERSISTENT(MG_count_menu_icons)
const int MG_count_menu_icons = 6;

#pragma PERSISTENT(MG_menu_icons)
const char MG_menu_icons[6] = {
                               0x09,
                               0x07,
                               0x08,
                               0x0A,
                               0x0B,
                               0x0C
};

#pragma PERSISTENT(MG_menu_scene_addresses)
const int MG_menu_scene_addresses[6] = {
                                      SCENEADDR_status_menu,
                                      SCENEADDR_proof_text,
                                      SCENEADDR_proof_text,
                                      SCENEADDR_proof_text,
                                      SCENEADDR_debug_menu,
                                      SCENEADDR_calendar_menu
};


#endif /* LIB_MENUS_MAIN_GAME_H_ */
