//***************************************************************************************
//  PETI FOSS Virtual Pet - Audio Alerting Primitives Code
//
//  Principal Code for TI MSP430FR5994 LaunchPad to Provide Audio Tones
//
//  Check the schematics folder for VCS-controlled hardware schematics
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  Added January 2022
//***************************************************************************************

#include "driverlib.h"
#include <msp430.h>
#include "audio.h"


void AUDIO_pulse(int pulse_length){
    unsigned int hardware_audio_disable = GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN7);
    if (!hardware_audio_disable){
    Timer_B_initUpModeParam initContParam = {0};
    initContParam.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
    initContParam.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_64; // Should yield a freq where this counts at roughly 125kHz
    initContParam.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
    initContParam.timerPeriod = pulse_length;
    initContParam.timerClear = TIMER_B_DO_CLEAR;
    initContParam.captureCompareInterruptEnable_CCR0_CCIE = TIMER_B_CCIE_CCR0_INTERRUPT_DISABLE;
    initContParam.startTimer = false;
    Timer_B_initUpMode(TIMER_B0_BASE, &initContParam); // init TB in compare mode for TB0
    Timer_B_clearTimerInterrupt(TIMER_B0_BASE);
    Timer_B_setCompareValue(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_0, pulse_length); // THIS ALSO SETS THE INTERRUPT FREQUENCY!
    Timer_B_clearCaptureCompareInterrupt(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_0);
    Timer_B_enableInterrupt(TIMER_B0_BASE);
    Timer_B_enableCaptureCompareInterrupt(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_0);

    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN4); // set it up
    Timer_B_startCounter(TIMER_B0_BASE, TIMER_B_UP_MODE); // and walk away.
    }
}
