//***************************************************************************************
//  PETI FOSS Virtual Pet - Main Program Loop
//
//  Principal Code for TI MSP430FR5994 LaunchPad to adjudicate a PETI game.
//  This code works with the TI BOOKSXL-SHARP128 booster pack. Output P3 is available as
//  an output array for printing arbitrary debug flags to LED; check with the Init_GPIO
//  function to determine which pin is in what state under what condition.
//
//  Check the schematics folder for VCS-controlled hardware schematics, including pinouts.
//  Check the wiki at the github repo for high-level documentation.
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  October 2020 - Updated May 2021
//***************************************************************************************



#include "driverlib.h"
#include "lib/game/game_manager.h"
#include "lib/display/display.h"
#include "lib/hwinit/hwinit.h"
#include "lib/scenes/scenes_manager.h"
#include <msp430.h>
#include "main.h"
//#include "lib/game/evo_data.h"
//#include "lib/locales/enCA_strings.h"

//We need a simple function for handling the state of buttons_state as it
//is adjusted by the ISRs.
void Update_Button_States(void){
    if (buttons_state & button_a_pressed)
    {
        buttons_state &= ~button_a_pressed; // In this instance we know we can de-set this flag.
        buttons_state ^= button_a_toggle;  // And alternate this flag.
        interacted_flag = true;
    }
    if (buttons_state & button_b_pressed)
    {
            buttons_state &= ~button_b_pressed; // In this instance we know we can de-set this flag.
            buttons_state ^= button_b_toggle;  // And alternate this flag.
            interacted_flag = true;
        }
    if (buttons_state & button_c_pressed)
    {
            buttons_state &= ~button_c_pressed; // In this instance we know we can de-set this flag.
            buttons_state ^= button_c_toggle;  // And alternate this flag.
            interacted_flag = true;
        }
    if (buttons_state & button_d_pressed)
    {
            buttons_state &= ~button_d_pressed; // In this instance we know we can de-set this flag.
            buttons_state ^= button_d_toggle;  // And alternate this flag.
            interacted_flag = true;
        }
}

// This is chiefly only used by the demo mode (lib/scenes/demo_mode) and may be removed once more of the game is implemented, to save space.
void Init_Buttons(void){
    buttons_state = 0;
    buttonsBar[0] = ' ';
    buttonsBar[1] = ' ';
    buttonsBar[2] = 'A';
    buttonsBar[3] = 'B';
    buttonsBar[4] = 'C';
    buttonsBar[5] = 'D';
    buttonsBar[6] = ' ';
    buttonsBar[7] = ' ';
    buttonsBar[8] = 0;
}

int main(void) {
    Init_Watchdog();
    PMM_unlockLPM5();
    VCOM = MLCD_VCOM; // Set the initial state of the VCOM bit.
    FORCE_REFRESH = 0x00; // Force a refresh of all lines for the first scene ever called
    interacted_flag = 0x00;
    Init_GPIO();
    Init_Timers();
    Init_RTC();
    Init_SPI();
    Init_LCD();
    Init_Buttons();
    GAME_initStateStruct();

    while (1){
        PMM_unlockLPM5();
        GAME_evaluateTimedEvents();
        Update_Button_States();
        SCENE_updateDisplay();
        ToggleVCOM();
        __bis_SR_register(LPM0_bits | GIE);
    }
}

//BCD is a pain to work with... This function was added for convenience and is still used in odd places.
// There is a faster implementation, RTC_C_convertBCDtoBinary(), which is preferred for usage.
// The refactor to convert to that is not complete and so this function has to stay.
unsigned char bcd_to_dec(unsigned char bcd)
{
  return bcd - 3*(bcd >> 4);
}



// interrupt service routine to handle timer A.
// Based on current configuration as of may 2021 this should be raised once per second.
// Raising this alert flips the VCOM flag at the manufacturer-recommended rate of 1/sec
// And wakes up the main loop.
#pragma vector=TIMER0_A0_VECTOR
__interrupt void VCOM_ISR (void){
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_clearTimerInterrupt(TIMER_A0_BASE);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
    if(VCOM == 0x00)                                  // invert polarity bit every second
    {
        VCOM = MLCD_VCOM;
    }
    else
    {
        VCOM = 0x00;
    }
    __bic_SR_register_on_exit(LPM0_bits);            // wake up main loop every second
}

// The ISRs below handle interrupts raised by each of the input keys A through D.
// They update the buttons_state int to indicate that they have been used, then wake
// up the device to handle the input in main loop rather than via an ISR.

#pragma vector=PORT5_VECTOR
__interrupt void BUTTON_C_ISR (void){
    GPIO_clearInterrupt(GPIO_PORT_P5, GPIO_PIN7);
    buttons_state |= button_c_pressed;
    __bic_SR_register_on_exit(LPM0_bits);            // wake up main loop
}

#pragma vector=PORT6_VECTOR
__interrupt void BUTTON_A_ISR (void){
    GPIO_clearInterrupt(GPIO_PORT_P6, GPIO_PIN0);
    buttons_state |= button_a_pressed;
    __bic_SR_register_on_exit(LPM0_bits);            // wake up main loop
}

#pragma vector=PORT7_VECTOR
__interrupt void BUTTON_B_ISR (void){
    GPIO_clearInterrupt(GPIO_PORT_P7, GPIO_PIN1);
    buttons_state |= button_b_pressed;
    __bic_SR_register_on_exit(LPM0_bits);            // wake up main loop
}

#pragma vector=PORT8_VECTOR
__interrupt void BUTTON_D_ISR (void){
    GPIO_clearInterrupt(GPIO_PORT_P8, GPIO_PIN3);
    buttons_state |= button_d_pressed;
    __bic_SR_register_on_exit(LPM0_bits);            // wake up main loop
}
