/*
 * scene_definitions.c
 *
 *  Created on: Nov. 12, 2021
 *      Author: patches
 */

#include "scene_definitions.h"

#pragma PERSISTENT(SCENEDEF_game_mode)
const SceneDefinition SCENEDEF_game_mode = {
     .rows = {
        {1, TEXT_SIZE_MEDIUM},
        {13, TEXT_SIZE_LARGE},
        {29, TEXT_SIZE_LARGE},
        {45, TEXT_SIZE_LARGE},
        {61, TEXT_SIZE_LARGE},
        {77, TEXT_SIZE_LARGE},
        {93, TEXT_SIZE_LARGE},
        {109, TEXT_SIZE_SMALL},
        {117, TEXT_SIZE_MEDIUM}
     },
     .lines_used = 9
};

#pragma PERSISTENT(SCENEDEF_menu_mode)
const SceneDefinition SCENEDEF_menu_mode = {
    .rows = {
             {4, TEXT_SIZE_MEDIUM},
             {16, TEXT_SIZE_MEDIUM},
             {28, TEXT_SIZE_MEDIUM},
             {40, TEXT_SIZE_MEDIUM},
             {52, TEXT_SIZE_MEDIUM},
             {64, TEXT_SIZE_MEDIUM},
             {76, TEXT_SIZE_MEDIUM},
             {88, TEXT_SIZE_MEDIUM},
             {100, TEXT_SIZE_MEDIUM},
             {112, TEXT_SIZE_MEDIUM}
        },
    .lines_used = 10
};


#pragma PERSISTENT(SCENEDEF_minigames)
const SceneDefinition SCENEDEF_minigames = {
    .rows = {
             {1, TEXT_SIZE_LARGE},
             {17, TEXT_SIZE_LARGE},
             {33, TEXT_SIZE_LARGE},
             {49, TEXT_SIZE_LARGE},
             {65, TEXT_SIZE_LARGE},
             {81, TEXT_SIZE_LARGE},
             {97, TEXT_SIZE_LARGE},
             {113, TEXT_SIZE_LARGE}
        },
    .lines_used = 8
};
