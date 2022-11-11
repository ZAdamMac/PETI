/*
 * food_data.h
 *
 *  Created on: Dec 6, 2021
 *      Author: patches
 */

#ifndef LIB_GAME_FOOD_DATA_H_
#define LIB_GAME_FOOD_DATA_H_


typedef struct Food{
    unsigned int hunger_value;   // An integer value (must be between 0 and 15) that describes the increase to hunger for eating this food
    unsigned int fun_value;      // An integer value (must be between 0 and 15) that describes the increase to fun for eating this food
    unsigned int mod_weight;     // An integer value (must be between 0 and 64) that describes the increase to the weight bits of the health byte for eating this food.
    char animation[3];  // Three characters of animation that describe this food.
} Food;

extern Food FOOD_array_foods[];
extern Food FOOD_array_snacks[];

extern int FOOD_count_foods;
extern int FOOD_count_snacks;

#endif /* LIB_GAME_FOOD_DATA_H_ */
