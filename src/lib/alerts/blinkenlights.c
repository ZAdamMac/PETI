//***************************************************************************************
//  PETI FOSS Virtual Pet - LED Alerting Primitives Code
//
//  Principal Code for TI MSP430FR5994 LaunchPad to Provide Alert LED Functionality
//
//  Check the hardware_design folder for VCS-controlled hardware schematics
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  Added January 2022
//***************************************************************************************

#include "driverlib.h"
#include <msp430.h>
#include "blinkenlights.h"
#include "lib/hwinit/hwinit.h"

unsigned int BLINKENLIGHTS_ALERT_LED_HOT = 0;
unsigned int BLINKENLIGHTS_BATTERY_LED_HOT = 0;

void BLINKENLIGHTS_blinkAlertLED(int pulse_length){
    Timer_A_initUpModeParam initContParam = {0};
    initContParam.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    initContParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64; // Should yield a freq where this counts at roughly 125kHz
    initContParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    initContParam.timerPeriod = pulse_length; // Which SHOULD trigger the interrupt 2/second. This is a comfortable visual frequency.
    initContParam.timerClear = TIMER_A_DO_CLEAR;
    initContParam.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    initContParam.startTimer = false;
    Timer_A_initUpMode(TIMER_A1_BASE, &initContParam); // init TA in compare mode for TA2
    Timer_A_clearTimerInterrupt(TIMER_A1_BASE);
    Timer_A_setCompareValue(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0, pulse_length); // THIS ALSO SETS THE INTERRUPT FREQUENCY!
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_enableInterrupt(TIMER_A1_BASE);
    Timer_A_enableCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    // start the timer

    GPIO_setOutputHighOnPin(LED_ALERT_PORT, LED_ALERT_PIN);
    BLINKENLIGHTS_ALERT_LED_HOT = 1;
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
}

void BLINKENLIGHTS_stopAlertLED(void){
    Timer_A_stop(TIMER_A1_BASE);
    BLINKENLIGHTS_ALERT_LED_HOT = 0;
    GPIO_setOutputLowOnPin(LED_ALERT_PORT, LED_ALERT_PIN);
}

void BLINKENLIGHTS_blinkBatteryLED(int pulse_length){
    Timer_A_initUpModeParam initContParam = {0};
    initContParam.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    initContParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64; // Should yield a freq where this counts at roughly 125kHz
    initContParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    initContParam.timerPeriod = pulse_length; // Which SHOULD trigger the interrupt 2/second. This is a comfortable visual frequency.
    initContParam.timerClear = TIMER_A_DO_CLEAR;
    initContParam.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    initContParam.startTimer = false;
    Timer_A_initUpMode(TIMER_A4_BASE, &initContParam); // init TA in compare mode for TA2
    Timer_A_clearTimerInterrupt(TIMER_A4_BASE);
    Timer_A_setCompareValue(TIMER_A4_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0, pulse_length); // THIS ALSO SETS THE INTERRUPT FREQUENCY!
    Timer_A_clearCaptureCompareInterrupt(TIMER_A4_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_enableInterrupt(TIMER_A4_BASE);
    Timer_A_enableCaptureCompareInterrupt(TIMER_A4_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    // start the timer

    GPIO_setOutputHighOnPin(LED_BATTERY_PORT, LED_BATTERY_PIN);
    BLINKENLIGHTS_BATTERY_LED_HOT = 1;
    Timer_A_startCounter(TIMER_A4_BASE, TIMER_A_UP_MODE);
}

void BLINKENLIGHTS_stopBatteryLED(void){
    Timer_A_stop(TIMER_A4_BASE);
    BLINKENLIGHTS_BATTERY_LED_HOT = 0;
    GPIO_setOutputLowOnPin(LED_BATTERY_PORT, LED_BATTERY_PIN);
}
