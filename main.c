//***************************************************************************************
//  PETI FOSS Virtual Pet - Main Program Loop
//
//  Principal Code for TI MSP430FR5994 LaunchPad to adjudicate a PETI game.
//  This code works with the TI BOOKSXL-SHARP128 booster pack. Output P3 is available as
//  an output array for printing arbitrary debug flags to LED; check with the Init_GPIO
//  function to determine which pin is in what state under what condition. P8.0 through
//  P8.3 are used as GPIO inputs for the input buttons.
//
//  Check the schematics folder for VCS-controlled hardware schematics
//
//
//  Zac Adam-MacEwen (Kensho Security Labs)
//  October 2020
//***************************************************************************************



#include "driverlib.h"
#include "lib/display/display.h"
#include <msp430.h>

#define VERSION " POC-2020-10-10"      //Max 16 characters.

volatile unsigned int timeMSec;         // clock milliseconds
volatile unsigned char timeSecond;      // clock seconds
volatile unsigned char timeMinute;      // clock minutes
volatile unsigned char VCOM;            // current VCOM state
char bufferText[17];

//We need a simple function to initialize eUSCI B1 to act as th
void Init_GPIO(void) {
    //We have a couple GPIO pins that most make sense to set up individually.
    // Some are explicitly or implicitly set up during Init_SPI.
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); // P1.0 is an LED for indicating VCOM state.
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0); // Initial VCOM = 0
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN1); // P1.1 is a debugging LED
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN1);

    //Uncomment the section below for the debugging panel.
    //GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0); //Current use: Whole byte of indexLine
    //GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN0);
    //GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN1); //Current use:
    //GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN1);
    //GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN2); //Current use:
    //GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN2);
    //GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN3); //Current use:
    //GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN3);
    //GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN4); //Current use:
    //GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN4);
    //GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN5); //Current use:
    //GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN5);
    //GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6); //Current use:
    //GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);
    //GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN7); //Current use:
    //GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
}

void Init_Timers(void) {
    // There are some very important global timers that we need
    VCOM = MLCD_VCOM;                                   // Starting value setup.
    timeMSec = 0;                                       // initialize variables used by "clock"
    timeSecond = 0;
    timeMinute = 0;
    // Set master clock to 1MHz; at this rate TimerA would interrupt every millisecond.
    // This is not likely especially power performant.
    // sets the properties to init Timer_A
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_0); // Set DCO frequency 1 MHz
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); //SMCLK = 1 Mhz
    Timer_A_initUpModeParam initContParam = {0};
    initContParam.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    initContParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    initContParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    initContParam.timerPeriod = 1000; // this should result in a roughly 1ms period.
    initContParam.timerClear = TIMER_A_DO_CLEAR;
    initContParam.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    initContParam.startTimer = false;
    Timer_A_initUpMode(TIMER_A0_BASE, &initContParam); // init TA in compare mode for TA2
    Timer_A_clearTimerInterrupt(TIMER_A0_BASE);
    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0, 1000);
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_enableInterrupt(TIMER_A0_BASE);
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    // start the timer
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}


int main(void) {

    WDTCTL = WDTPW | WDTHOLD; // Disable the watchdog timer. We might rely on this later, but not for now.
        P1IFG = 0;
        // Disable the GPIO power-on default high-impedance mode
        // to activate previously configured port settings
        SFRIFG1 &= ~OFIFG; // Clear the OFIFG because that's some annoying bugs.
        PMM_unlockLPM5();
        Init_GPIO();
        Init_Timers();
        Init_SPI();
        Init_LCD();
        DisplaySplash();
        printText(VERSION, 118);
        __delay_cycles(1000000);  //Normally stuff happens here, this is just as a demonstration
        //GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN1);
        LCDClearDisplay();
        printText("HELLO, WORLD!", 1);
        printText("PETI", 16);
        printText("SAYS", 24);
        printText("HI", 32);
        printText("1234567890123456", 56);
        printText("UPTIME:",72);

        while (1)
        {
        PMM_unlockLPM5();
        //__delay_cycles(1000);
        // write clock to display by forming a string literal representing the current time
        bufferText[0] = ' ';
        bufferText[1] = timeMinute / 10 + '0';
        bufferText[2] = timeMinute % 10 + '0';
        bufferText[3] = ':';
        bufferText[4] = timeSecond / 10 + '0';
        bufferText[5] = timeSecond % 10 + '0';
        bufferText[6] = 0;
        printText(bufferText,88); // There is still a bug here, this line doesn't fully display.
        ToggleVCOM(); //<- Removing this KILLS the ISRtrap bug.
        __bis_SR_register(LPM0_bits | GIE);
        };
    }


// interrupt service routine to handle timer A; drives VCOM and readable clock
#pragma vector=TIMER0_A0_VECTOR
__interrupt void VCOM_ISR (void)
{
    timeMSec++;// count milliseconds
    Timer_A_clearTimerInterrupt(TIMER_A0_BASE);
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    if(timeMSec == 1000)                                // if we reached 1 second
    {
        timeMSec = 0;                                   // reset milliseconds
        timeSecond++;                                   // increase seconds
        if(timeSecond == 60)                            // if we reached 1 minute
        {
            timeSecond = 0;                             // reset seconds
            timeMinute++;                               // increase minutes
            if(timeMinute == 60)                        // if we reached 1 hour
            {
                timeMinute = 0;                         // reset minutes
            }
        }
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
}
