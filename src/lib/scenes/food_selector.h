/*
 * menu_generator.h
 *
 *  Created on: Nov. 8, 2021
 *      Author: patches
 */


#include "lib/game/food_data.h"
#ifndef LIB_SCENES_FOOD_MENU_H_
#define LIB_SCENESFOOD_MENU_H_

Food FOODSEL_active_food;

void SCENE_FoodMenu(char * target_LSTRING_HEADER, char ** target_LARRAY, Food * target_FARRAY, int target_count_opts);


#endif /* LIB_SCENES_FOOD_MENU_H_ */
