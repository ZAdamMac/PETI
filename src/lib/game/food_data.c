/*
 * food.h
 *
 *  Created on: Dec 6, 2021
 *      Author: zadammac
 */

#include "food_data.h"

#pragma PERSISTENT(FOOD_array_food)
extern Food FOOD_array_foods[] = {
                          {
                           .hunger_value = 7,
                           .fun_value = 0,
                           .mod_weight = 0,
                           .animation = {201, 202, 203} // It is acceptable to use hex escapes or this.
                          }
};

#pragma PERSISTENT(FOOD_array_snacks)
extern Food FOOD_array_snacks[] = {
                          {
                           .hunger_value = 0,
                           .fun_value = 15,
                           .mod_weight = 1,
                           .animation = {204, 205, 206} // It is acceptable to use hex escapes or this.
                          }
};

extern int FOOD_count_foods = 1; // Len of FOOD_array_food
extern int FOOD_count_snacks = 1; //Len of FOOD_array_snacks
