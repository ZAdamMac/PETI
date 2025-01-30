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

#include "driverlib/MSP430FR5xx_6xx/driverlib.h"
#include <msp430.h>
#include "audio.h"


// Given int pulse_length (which is a factor of the overall system clock speed),
// output a fixed tone in PWM representing roughly 600hz at roughly 85% duty cycle
// FUTURE versions of this function will support arbitrary freq and duty cycle.
void AUDIO_pulse(int pulse_length){
    //check if the mute signal is being passed to the microcontroller.
    unsigned int hardware_audio_disable = GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN7);
    //if not muted:
    if (!hardware_audio_disable){
        Timer_B_outputPWMParam param;
        Timer_A_initUpModeParam length_param;
        
        //These parameters govern the behaviour of the PWM signal.
        param.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
        param.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_32; // Adjusts resolution available for signal/duty
        param.timerPeriod = 207;  // Sets frequency CCR0;
        param.compareRegister = TIMER_B_CAPTURECOMPARE_REGISTER_3;
        param.compareOutputMode = TIMER_B_OUTPUTMODE_TOGGLE_RESET;
        param.dutyCycle = 30; // Relationship to CCR0 sets duty cycle. Lower value = Higher PWM Duty %

        //These set up an interrupt to stop the PWM signal after pulse_length
        //Timer A2 is used for this because the timerB CCR0 register will *most likely* be lower than pulse_length
        //And in such a case you will never reach the end-of-note interrupt

        length_param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
        length_param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64; // Should yield a freq where this counts at roughly 125kHz
        length_param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
        length_param.timerPeriod = pulse_length; 
        length_param.timerClear = TIMER_A_DO_CLEAR;
        length_param.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
        length_param.startTimer = false;
        Timer_A_initUpMode(TIMER_A2_BASE, &length_param); // init TA in compare mode for TA2
        Timer_A_setCompareValue(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0, pulse_length); 
        Timer_A_clearTimerInterrupt(TIMER_A2_BASE);
        Timer_A_enableInterrupt(TIMER_A2_BASE);
        Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
        Timer_A_enableCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);

        //Start the timer in all regards.
        Timer_B_outputPWM(TIMER_B0_BASE, &param);
        Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);

        AUDIO_noteEnd = false;
        GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
    }
}


// In a sense this is extravagent at the time, but in another sense this sort of loop will be profoundly required for
// dealing with the Tunes system later in development.
void AUDIO_stopNote(void){
    AUDIO_noteEnd = false;
    Timer_B_stop(TIMER_B0_BASE);
    Timer_A_stop(TIMER_A2_BASE);

    //If we stop the timer with the clock in the high state we just hold the buzzer open.
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN4);
}