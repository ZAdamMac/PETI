//******************************************************************************
//  PETI FOSS Virtual Pet - Human Input Handler
//
//  Code to set up the interrupt vector handling and other primitives needed in 
//  order to track input events irrespective of the current scene.
//
//  Check the schematics folder for VCS-controlled hardware schematics
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  May 2023
//******************************************************************************

#include "driverlib.h"
#include <msp430.h>
#include "human_input.h"

unsigned int HID_input_events_queue[8] = {
    BUTTON_NO_PRESS,
    BUTTON_NO_PRESS,
    BUTTON_NO_PRESS,
    BUTTON_NO_PRESS,
    BUTTON_NO_PRESS,
    BUTTON_NO_PRESS,
    BUTTON_NO_PRESS,
    BUTTON_NO_PRESS,
};
unsigned int HID_input_events_queue_depth = 0;
unsigned int HID_interacted_flag = 0;

// The ISRs below handle interrupts raised by each of the input keys A through D.
// They update the buttons_state int to indicate that they have been used, then wake
// up the device to handle the input in main loop rather than via an ISR.

#pragma vector=PORT5_VECTOR
__interrupt void BUTTON_C_ISR (void){
    GPIO_clearInterrupt(GPIO_PORT_P5, GPIO_PIN7);
    HID_input_events_queue[HID_input_events_queue_depth] = BUTTON_C_PRESS;
    HID_input_events_queue_depth++;
    __bic_SR_register_on_exit(LPM0_bits);            // wake up main loop
}

#pragma vector=PORT6_VECTOR
__interrupt void BUTTON_A_ISR (void){
    GPIO_clearInterrupt(GPIO_PORT_P6, GPIO_PIN0);
    HID_input_events_queue[HID_input_events_queue_depth] = BUTTON_A_PRESS;
    HID_input_events_queue_depth++;
    __bic_SR_register_on_exit(LPM0_bits);            // wake up main loop
}

#pragma vector=PORT7_VECTOR
__interrupt void BUTTON_B_ISR (void){
    GPIO_clearInterrupt(GPIO_PORT_P7, GPIO_PIN1);
    HID_input_events_queue[HID_input_events_queue_depth] = BUTTON_B_PRESS;
    HID_input_events_queue_depth++;
    __bic_SR_register_on_exit(LPM0_bits);            // wake up main loop
}

#pragma vector=PORT8_VECTOR
__interrupt void BUTTON_D_ISR (void){
    GPIO_clearInterrupt(GPIO_PORT_P8, GPIO_PIN3);
    HID_input_events_queue[HID_input_events_queue_depth] = BUTTON_D_PRESS;
    HID_input_events_queue_depth++;
    __bic_SR_register_on_exit(LPM0_bits);            // wake up main loop
}
