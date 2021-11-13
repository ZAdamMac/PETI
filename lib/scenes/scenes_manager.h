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
// This data may have to break out into another file depending on inheritance rules
#define SCENEADDR_boot_splash 0x00      // Address for the boot splash animation.
#define SCENEADDR_main_game 0x01        // Address to the main game standby screen
#define SCENEADDR_status_menu 0x02      // Address to the pet's health page.
#define SCENEADDR_calendar_menu 0x11    // Address to the calendar adjustment menu.
#define SCENEADDR_debug_menu 0xF0       // Address to the debug menu
#define SCENEADDR_demo_mode 0xF1        // Address for the demo screen
#define SCENEADDR_proof_text 0xFF       // Address to the "button worked" proof message scene

void SCENE_updateDisplay(void);         // Convenience function. Checks the value of SCENE_ACT globally to determine which scene's handler function to call.
void SCENE_setTransitionTimeCondition(unsigned int delay_in_seconds); // Set a time-based delay, evaluated by the function below.
int SCENE_checkTransitionTimeCondition(); // check RTC's current time against the delay set by the above function, returning true if that time is present or past, and false if future.
// TODO the line above is the culpret, gayrontee
#endif /* LIB_SCENES_SCENES_MANAGER_H_ */
