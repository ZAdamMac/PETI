/*
 * main.h
 *
 *  Created on: May 10, 2021
 *      Author: patches
 */

#define VERSION "     v0.1.0"           //Max 16 characters, and pad left.

volatile unsigned char VCOM;            // current VCOM state
char bufferText[17];                    // General placeholder used by all the printext functions.

unsigned int buttons_state;             // We need to hold a whole byte to keep track of the flag state
#define button_a_pressed BIT7           // And hit state of each button, along with the bits to control each.
#define button_b_pressed BIT6           // These upper bits indicate the button was pressed since the last time the
#define button_c_pressed BIT5           // Update_Button_States function was called.
#define button_d_pressed BIT4
#define button_a_toggle BIT3            // We need to track if the button is toggled on or off for certain functionality.
#define button_b_toggle BIT2            // These lower bits indicate that toggled state.
#define button_c_toggle BIT1            // Depending on the active scene, this may be reverted after it is read, or it may be left in its state
#define button_d_toggle BIT0


unsigned int interacted_flag;           // An int, however used effectively as a bool flag to determine if the device has been interacted with.
unsigned int calendar_initial_setup_completed; // As interacted_flag, used to determine if the RTC has been set at least once by lib/scenes/calendar_menu
