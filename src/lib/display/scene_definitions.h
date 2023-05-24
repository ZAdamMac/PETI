/*
 * scene_definitions.h
 *
 *  Created on: Nov. 12, 2021
 *      Author: patches
 */

#ifndef LIB_DISPLAY_SCENE_DEFINITIONS_H_
#define LIB_DISPLAY_SCENE_DEFINITIONS_H_

#include "display.h"

// Enumerating some control values for text size.
typedef enum textSizeType { TEXT_SIZE_SMALL, TEXT_SIZE_MEDIUM, TEXT_SIZE_LARGE } textSizeType;

typedef struct SceneDefinitionLine {
    int line_address;                   // Starting line (y) of this set of text.
    textSizeType text_size;             // See above enum for meaning.
} SceneDefinitionLine;

typedef struct SceneDefinition {
    SceneDefinitionLine rows[PIXELS_X/FONT_SIZE_FLOOR_X];
    int lines_used;                                         // The count of SceneDefinitionLine objects actually used by this scene.
} SceneDefinition;


extern const SceneDefinition SCENEDEF_game_mode;

extern const SceneDefinition SCENEDEF_menu_mode;

extern const SceneDefinition SCENEDEF_minigames;

#endif /* LIB_DISPLAY_SCENE_DEFINITIONS_H_ */
