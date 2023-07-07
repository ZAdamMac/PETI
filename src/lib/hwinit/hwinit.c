//***************************************************************************************
//  PETI FOSS Virtual Pet - Hardware Init Code
//
//  Principal Code for TI MSP430FR5994 LaunchPad to initialize various builtin peripherals
//
//  Check the schematics folder for VCS-controlled hardware schematics
//
//
//  Zac Adam-MacEwen (Kensho Security Labs)
//  October 2020
//***************************************************************************************

#include "driverlib.h"
#include <msp430.h>

// Default values for the RTC must be provided.
// The following variables are BCD, so read as decimal, but provid `0x` as though they were hex
#define DEFAULT_DOW 0x05        //Convention is arbitrary 0 = sunday
#define DEFAULT_DAY 0x07       //The day of the month
#define DEFAULT_MONTH 0x07      //The current month
#define DEFAULT_YEAR 0x2023     //The current year.

//We need a simple function to initialize some GPIO pins for driving input and output.
//Special GPIO pins for (e.g. SPI) are called out in their own functions.
void Init_GPIO(void) {
    //We have a couple GPIO pins that most make sense to set up individually.
    // Some are explicitly or implicitly set up during Init_SPI.
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); // P1.0 is an LED for indicating VCOM state.
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0); // Initial VCOM = 0
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN1); // P1.1 is a debugging LED; currently toggles with each keypress.
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN1);


    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN5); // P1.0 is an LED for indicating the battery alert.
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN5);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN7); // P4.7 is the alert system visual LED.
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN7);

    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN4); //P3.4 is buzzer Out.
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN4);

    //Inputs Block
    //This sets up the buttons as indicated below.
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P6, GPIO_PIN0); //A = P6.0
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P7, GPIO_PIN1); //B = P7.1
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN7); //C = P5.7
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P8, GPIO_PIN3); //D = P8.3
    GPIO_enableInterrupt(GPIO_PORT_P6, GPIO_PIN0);
    GPIO_enableInterrupt(GPIO_PORT_P7, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN7);
    GPIO_enableInterrupt(GPIO_PORT_P8, GPIO_PIN3);
    GPIO_selectInterruptEdge(GPIO_PORT_P6,GPIO_PIN0, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_selectInterruptEdge(GPIO_PORT_P7,GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_selectInterruptEdge(GPIO_PORT_P5,GPIO_PIN7, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_selectInterruptEdge(GPIO_PORT_P8,GPIO_PIN3, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_clearInterrupt(GPIO_PORT_P6, GPIO_PIN0);
    GPIO_clearInterrupt(GPIO_PORT_P7, GPIO_PIN1);
    GPIO_clearInterrupt(GPIO_PORT_P5, GPIO_PIN7);
    GPIO_clearInterrupt(GPIO_PORT_P8, GPIO_PIN3);

    //DIP Switch Inputs Block
    //These are obviously not momentary
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN6); //SW2.2 = P3.6
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN7); //SW2.1 = P3.7
}


// There are some very important global timers that we need
// Currently we only initialize Timer_A which will raise a 1hz interrupt for wakefulness.
// Primary timekeeping is via RTC_C
void Init_Timers(void) {
    // Set master clock to 8MHz; at this rate TimerA would interrupt roughly 2/sec
    // sets the properties to init Timer_A
    // Running the timer any faster than this will /break/ the animation logic in a way requiring greater complexity to operate.
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_3); // Set DCO frequency 8 MHz
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); //SMCLK = 8 Mhz
    Timer_A_initUpModeParam initContParam = {0};
    initContParam.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    initContParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64; // Should yield a freq where this counts at roughly 125kHz
    initContParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    initContParam.timerPeriod = 0xFFFF; // Which SHOULD trigger the interrupt 2/second. This is a comfortable visual frequency.
    initContParam.timerClear = TIMER_A_DO_CLEAR;
    initContParam.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    initContParam.startTimer = false;
    Timer_A_initUpMode(TIMER_A0_BASE, &initContParam); // init TA in compare mode for TA2
    Timer_A_clearTimerInterrupt(TIMER_A0_BASE);
    Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0, 0xFFFF); // THIS ALSO SETS THE INTERRUPT FREQUENCY!
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_enableInterrupt(TIMER_A0_BASE);
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    // start the timer
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}

// The RTC_C module is used in BCD format to provide timekeeping for the rest of the program. Time-based operations should
// interrogate whatever figures they need from the calendar in order to drive their operations.
// This allows for granularity of action down to a second; anything faster will need a timer to drive it.
void Init_RTC(void){
    //We need a default calendar to use as a local epoch. The date just happens to be when we were setting up this function.
    Calendar defaultTime;
    defaultTime.Seconds = 0x00;
    defaultTime.Minutes = 0x00;
    defaultTime.Hours = 0x00;
    defaultTime.DayOfWeek = DEFAULT_DOW;  // No convention is specified, so here we treat sunday as 0.
    defaultTime.DayOfMonth = DEFAULT_DAY;
    defaultTime.Month = DEFAULT_MONTH;
    defaultTime.Year = DEFAULT_YEAR; // Accepts values up to 4096 (lel) so no unix epoch issue here.

    RTC_C_initCalendar(RTC_C_BASE, &defaultTime, RTC_C_FORMAT_BCD); // Hey struct, get into your registers.
    RTC_C_startClock(RTC_C_BASE); // Okay let's go!
}

// We are not currently using the standard watchdog timer, though this may change in the immediate future.
void Init_Watchdog(void){
    WDTCTL = WDTPW | WDTHOLD;
    // Clear P1 IFGs, more as a formality than anything.
    P1IFG = 0;
    // Clear the OFIFG because occasionally strange IFGs get set that we aren't handling.
    SFRIFG1 &= ~OFIFG;
}
