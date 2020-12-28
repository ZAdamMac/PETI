//***************************************************************************************
//  PETI FOSS Virtual Pet - Main Program Loop
//
//  Principal Code for TI MSP430FR5994 LaunchPad to adjudicate a PETI game.
//  This code works with the TI BOOKSXL-SHARP128 booster pack. Output P3 is available as
//  an output array for printing arbitrary debug flags to LED; check with the Init_GPIO
//  function to determine which pin is in what state under what condition.
//
//  Check the schematics folder for VCS-controlled hardware schematics, including pinouts.
//
//
//  Zac Adam-MacEwen (Kensho Security Labs)
//  October 2020
//***************************************************************************************



#include "driverlib.h"
#include "lib/display/display.h"
#include <msp430.h>

#define VERSION " POC-2020-12-03"       //Max 16 characters.

volatile unsigned int timeMSec;         // clock milliseconds
volatile unsigned char timeSecond;      // clock seconds
volatile unsigned char timeMinute;      // clock minutes
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

//We need a simple function for handling the state of buttons_state as it
//Needs updating throughtout the ISR functions.
void Update_Button_States(void){
    if (buttons_state & button_a_pressed)
    {
        buttons_state &= ~button_a_pressed; // In this instance we know we can de-set this flag.
        buttons_state ^= button_a_toggle;  // And alternate this flag.
    }
    if (buttons_state & button_b_pressed)
    {
            buttons_state &= ~button_b_pressed; // In this instance we know we can de-set this flag.
            buttons_state ^= button_b_toggle;  // And alternate this flag.
        }
    if (buttons_state & button_c_pressed)
    {
            buttons_state &= ~button_c_pressed; // In this instance we know we can de-set this flag.
            buttons_state ^= button_c_toggle;  // And alternate this flag.
        }
    if (buttons_state & button_d_pressed)
    {
            buttons_state &= ~button_d_pressed; // In this instance we know we can de-set this flag.
            buttons_state ^= button_d_toggle;  // And alternate this flag.
        }
}

//We also need a simple function for updating the magic string:
void Update_Buttons_Bar(void){
    if ((buttons_state & button_a_toggle) == button_a_toggle)
    { // in each case we care only about the state of the toggle flag.
        buttonsBar[2] = 'a';               // because of how the font is designed, the bold/pressed version is lowercase
    }
    else
    {
        buttonsBar[2] = 'A';
    }
    if (buttons_state & button_b_toggle)
    { // in each case we care only about the state of the toggle flag.
        buttonsBar[3] = 'b';               // because of how the font is designed, the bold/pressed version is lowercase
    }
    else
    {
        buttonsBar[3] = 'B';
    }
    if ( buttons_state & button_c_toggle )
    { // in each case we care only about the state of the toggle flag.
        buttonsBar[4] = 'c';               // because of how the font is designed, the bold/pressed version is lowercase
    }
    else
    {
        buttonsBar[4] = 'C';
    }
    if (buttons_state & button_d_toggle)
    { // in each case we care only about the state of the toggle flag.
        buttonsBar[5] = 'd';               // because of how the font is designed, the bold/pressed version is lowercase
    }
    else
    {
        buttonsBar[5] = 'D';
    }
}

//We need a simple function to initialize some GPIO pins for driving
//button input as well as the status LEDs
void Init_GPIO(void) {
    //We have a couple GPIO pins that most make sense to set up individually.
    // Some are explicitly or implicitly set up during Init_SPI.
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); // P1.0 is an LED for indicating VCOM state.
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0); // Initial VCOM = 0
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN1); // P1.1 is a debugging LED; currently toggles with each keypress.
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN1);

    // Set up debugging panel GPIO
    #ifdef DEBUG
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0); //Current use: Whole byte of indexLine
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN1); //Current use:
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN2); //Current use:
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN3); //Current use:
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN3);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN4); //Current use:
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN5); //Current use:
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6); //Current use:
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN7); //Current use:
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
    #endif

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
}

void Init_Timers(void) {
    // There are some very important global timers that we need
    // Currently we only initialize Timer_A which will raise a 1khz interrupt
    // Allowing us to tick off the milliseconds.
    // In future this will likely be altered for power mnagement and to use RTC for main
    // Time tracking.
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
    initContParam.timerPeriod = 1000; // this should result in a roughly 1ms period, assuming the clock speed is accurate
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

void Display_Splash_with_Delay(int cycles){
    DisplaySplash();
    printTextSmall(VERSION, 118);
    // Normally stuff happens here, this is just as a demonstration to allow the page to remain a while
    __delay_cycles(cycles);  
    LCDClearDisplay();
}

void Print_Uptime(void){
    // write clock to display by forming a string literal representing the current time
    bufferText[0] = ' ';
    bufferText[1] = timeMinute / 10 + '0';
    bufferText[2] = timeMinute % 10 + '0';
    bufferText[3] = ':';
    bufferText[4] = timeSecond / 10 + '0';
    bufferText[5] = timeSecond % 10 + '0';
    bufferText[6] = 0;
    printTextSmall(bufferText,88); // There is still a bug here, this line doesn't fully display.
}

int main(void) {
    // Kosmo: could any of this init go into an init function?

    // Disable the watchdog timer. We might rely on this later, but not for now.
    WDTCTL = WDTPW | WDTHOLD; 
    
    // Clear P1 IFGs, more as a formality than anything.
    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    P1IFG = 0;  
    
    // Clear the OFIFG because occasionally strange IFGs get set that we aren't handling.
    SFRIFG1 &= ~OFIFG;

    // Without this output pins can be stuck at current state causing apparent freezes. 
    PMM_unlockLPM5();  
    
    Init_GPIO();
    Init_Timers();
    Init_SPI();
    Init_LCD();
    Init_Buttons();

    Display_Splash_with_Delay(1000000);
    printTextMedium("  HELLO, WORLD! ", 1);
    printTextSmall("      PETI      ", 16);
    printTextSmall("      SAYS      ", 24);
    printTextSmall("       HI       ", 32);
    printTextSmall("1234567890123456", 56);
    printTextSmall("UPTIME:", 72);

    while (1){
        PMM_unlockLPM5();
        Update_Button_States();
        Update_Buttons_Bar();
        Print_Uptime();
        printTextLarge(buttonsBar, 100);
        // Removing below KILLS the ISRtrap bug.
        ToggleVCOM(); 
        __bis_SR_register(LPM0_bits | GIE);
    }
}


// interrupt service routine to handle timer A; drives VCOM and readable clock; mostly for demonstration
#pragma vector=TIMER0_A0_VECTOR
__interrupt void VCOM_ISR (void){
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

// The ISRs below handle interrupts raised by each of the input keys A through D.
// In future they should not "do" what their key does, simply set a flag used in main() indicating the button was pressed.

#pragma vector=PORT5_VECTOR
__interrupt void BUTTON_C_ISR (void){
    GPIO_clearInterrupt(GPIO_PORT_P5, GPIO_PIN7);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN1); //deprecate these.
    buttons_state |= button_c_pressed;
    __bic_SR_register_on_exit(LPM0_bits);            // wake up main loop every second
}

#pragma vector=PORT6_VECTOR
__interrupt void BUTTON_A_ISR (void){
    GPIO_clearInterrupt(GPIO_PORT_P6, GPIO_PIN0);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN1);
    buttons_state |= button_a_pressed;
    __bic_SR_register_on_exit(LPM0_bits);            // wake up main loop every second
}

#pragma vector=PORT7_VECTOR
__interrupt void BUTTON_B_ISR (void){
    GPIO_clearInterrupt(GPIO_PORT_P7, GPIO_PIN1);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN1);
    buttons_state |= button_b_pressed;
    __bic_SR_register_on_exit(LPM0_bits);            // wake up main loop every second
}

#pragma vector=PORT8_VECTOR
__interrupt void BUTTON_D_ISR (void){
    GPIO_clearInterrupt(GPIO_PORT_P8, GPIO_PIN3);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN1);
    buttons_state |= button_d_pressed;
    __bic_SR_register_on_exit(LPM0_bits);            // wake up main loop every second
}
