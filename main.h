/*
 * display.h
 *
 *  Created on: May 10, 2021
 *      Author: patches
 */

#define VERSION " POC-2021-05-11"       //Max 16 characters.

volatile unsigned char VCOM;            // current VCOM state
char bufferText[17];                    // General placeholder used by all the printext functions.

char buttonsBar[9];                     // string that prints button status bar. Might not survive in prod.
unsigned int buttons_state;            // We need to hold a whole byte to keep track of the flag state
#define button_a_pressed BIT7           // And hit state of each button, along with the bits to control each.
#define button_b_pressed BIT6
#define button_c_pressed BIT5
#define button_d_pressed BIT4
#define button_a_toggle BIT3
#define button_b_toggle BIT2
#define button_c_toggle BIT1
#define button_d_toggle BIT0

unsigned char bcd_to_dec(unsigned char bcd);

unsigned int interacted_flag;
