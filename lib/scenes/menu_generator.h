/*
 * menu_generator.h
 *
 *  Created on: Nov. 8, 2021
 *      Author: patches
 */

#ifndef LIB_SCENES_MENU_GENERATOR_H_
#define LIB_SCENES_MENU_GENERATOR_H_

typedef void (*voidFuncPointer)(void);


void SCENE_TextMenu(char * target_LSTRING_HEADER, char ** target_LARRAY, voidFuncPointer ** target_MARRAY, int target_count_opts);


#endif /* LIB_SCENES_MENU_GENERATOR_H_ */
