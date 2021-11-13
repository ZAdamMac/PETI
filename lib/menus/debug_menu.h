/*
 * debug_menu.h
 *
 *  Created on: Nov. 8, 2021
 *      Author: patches
 */

#ifndef LIB_MENUS_DEBUG_MENU_H_
#define LIB_MENUS_DEBUG_MENU_H_

#pragma PERSISTENT(MENU_DEBUG_count_options);
extern const int MENU_DEBUG_count_options;

#pragma PERSISTENT(MENU_DEBUG_functions);
extern voidFuncPointer * MENU_DEBUG_functions[2];

#endif /* LIB_MENUS_DEMUG_MENU_H_ */
