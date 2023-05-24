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
#include "lib/game/entropy.h"
#include "lib/display/display.h"
#include "lib/hwinit/hwinit.h"
#include "lib/scenes/scenes_manager.h"
#include <msp430.h>
#include "main.h"
#include "lib/alerts/blinkenlights.h"
#include "lib/hwinit/human_input.h"
//#include "lib/game/evo_data.h"
//#include "lib/locales/enCA_strings.h"


int main(void) {
    Init_Watchdog();
    PMM_unlockLPM5();
    VCOM = MLCD_VCOM; // Set the initial state of the VCOM bit.
    FORCE_REFRESH = 0x00; // Force a refresh of all lines for the first scene ever called
    interacted_flag = 0x00;
    RNG_session_seed = RNG_getSeedWord(); // Done very early to be able to use the ADC without interfering with other functions.
    RNG_initialize(RNG_session_seed);
    Init_GPIO();
    Init_Timers();
    Init_RTC();
    Init_SPI();
    Init_LCD();
    GAME_initStateStruct();

    while (1){
        PMM_unlockLPM5();
        GAME_evaluateTimedEvents();
        if (interacted_flag){
            BLINKENLIGHTS_lower();
        }
        SCENE_updateDisplay();
        ToggleVCOM();
        RNG_forceShuffle();
        interacted_flag = 0x00; // By this point any interaction has been handled
        __bis_SR_register(LPM0_bits | GIE);
    }}

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

//Stop audio when the timer has elapsed.
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMEOUT_ISR (void){
    Timer_B_clearCaptureCompareInterrupt(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_0);
    Timer_B_clearTimerInterrupt(TIMER_B0_BASE);
    Timer_B_stop(TIMER_B0_BASE);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN4);
    __bic_SR_register_on_exit(LPM0_bits);            // wake up main loop every second
}