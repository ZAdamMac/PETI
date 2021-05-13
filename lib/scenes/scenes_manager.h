/*
 * scenes_manager.h
 *
 *  Created on: May 10, 2021
 *      Author: patches
 */

#ifndef LIB_SCENES_SCENES_MANAGER_H_
#define LIB_SCENES_SCENES_MANAGER_H_

volatile unsigned int SCENE_ACT;            // The current active scene

// The following defines are all addresses for scenes defined in other files in the scene system.
// If you are adding a scene to this header make sure to also add its include to the corresponding c file and update SCENE_updateDisplay's case statement!

#define SCENEADDR_boot_splash 0x00
#define SCENEADDR_demo_mode 0x01
#define SCENEADDR_main_game 0x02
#define SCENEADDR_calendar_menu 0x10

void SCENE_updateDisplay(void);
void SCENE_setTransitionTimeCondition(unsigned int delay_in_seconds);
bool SCENE_checkTransitionTimeCondition();

#endif /* LIB_SCENES_SCENES_MANAGER_H_ */
