/*
 * hwinit.h
 *
 *  Created on: May 8, 2023
 *      Author: patches
 */

#ifndef LIB_HWINIT_HID_H_
#define LIB_HWINIT_HID_H_

// Predefine arbitrary human input event types
// Hypothetically you could simply expand this list if adding new inputs.
#define BUTTON_NO_PRESS 0xFF
#define BUTTON_A_PRESS 0x00
#define BUTTON_B_PRESS 0x01
#define BUTTON_C_PRESS 0x02
#define BUTTON_D_PRESS 0x03

extern unsigned int HID_input_events_queue[8];
extern unsigned int HID_input_events_queue_depth;
extern unsigned int HID_interacted_flag;

#endif /* LIB_HWINIT_HID_H_ */