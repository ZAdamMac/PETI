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

#define VERSION " POC-2021-05-08"       //Max 16 characters.
#define SPLASH_DELAY 1000000            //Number of cycles to make the system idle at the splash page idle.

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
//Needs updating throughout the ISR functions.
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
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0); //Current use: Unused
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

    // Set master clock to 1MHz; at this rate TimerA would interrupt every millisecond.
    // This is not likely especially power performant.
    // sets the properties to init Timer_A
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_0); // Set DCO frequency 1 MHz (? - experimentation suggests this is not true)
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); //SMCLK = 1 Mhz
    Timer_A_initUpModeParam initContParam = {0};
    initContParam.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    initContParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_16; // Should yield a freq where this counts at roughly 65kHz
    initContParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    initContParam.timerPeriod = 0xFFFF; // Which SHOULD trigger the interrupt 1/second.
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

void Init_RTC(void){
    //We need a default calendar to use as a local epoch. The date just happens to be when we were setting up this function.
    Calendar defaultTime;
    defaultTime.Seconds = 0x00;
    defaultTime.Minutes = 0x00;
    defaultTime.Hours = 0x00;
    defaultTime.DayOfWeek = 0x06;  // No convention is specified, so here we treat sunday as 0.
    defaultTime.DayOfMonth = 0x08;
    defaultTime.Month = 0x05;
    defaultTime.Year = 0x2021; // Accepts values up to 4096 (lel) so no unix epoch issue here.

    RTC_C_initCalendar(RTC_C_BASE, &defaultTime, RTC_C_FORMAT_BCD); // Hey struct, get into your registers.
    RTC_C_startClock(RTC_C_BASE); // Okay let's go!
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

void Display_Splash_with_Delay(void){
    DisplaySplash();
    printTextSmall(VERSION, 118);
    // Normally stuff happens here, this is just as a demonstration to allow the page to remain a while
    __delay_cycles(SPLASH_DELAY);
    LCDClearDisplay();
}


void Print_CurrentTime(void){
    Calendar currentTime = RTC_C_getCalendarTime(RTC_C_BASE); // Returns the calendar time as a struct.
    bufferText[0] = ' '; // Centering Manually for Lulz.
    bufferText[1] = ' ';
    bufferText[2] = ' ';
    bufferText[3] = ' ';
    bufferText[4] = currentTime.Hours / 16 + '0'; //get the 10s of hours then add the appropriate character offset.
    bufferText[5] = currentTime.Hours % 16 + '0'; //16 is used as the divisor because the BCD format expresses the decimal value in a hex space, thus the need for math.
    bufferText[6] = ':';
    bufferText[7] = currentTime.Minutes / 16 + '0';
    bufferText[8] = currentTime.Minutes % 16 + '0';
    bufferText[9] = ':';
    bufferText[10] = currentTime.Seconds / 16 + '0';
    bufferText[11] = currentTime.Seconds % 16 + '0';
    bufferText[12] = ' ';
    bufferText[13] = ' ';
    bufferText[14] = ' ';
    bufferText[15] = ' ';
    printTextSmall(bufferText, 88); // Goes direct to the position for uptime in the old method
}

int main(void) {
    // Kosmo: could any of this init go into an init function?

    // Disable the watchdog timer. We might rely on this later, but not for now.
    WDTCTL = WDTPW | WDTHOLD;

    // Clear P1 IFGs, more as a formality than anything.
    P1IFG = 0;

    // Clear the OFIFG because occasionally strange IFGs get set that we aren't handling.
    SFRIFG1 &= ~OFIFG;

    // Without this output pins can be stuck at current state causing apparent freezes.
    PMM_unlockLPM5();

    Init_GPIO();
    Init_Timers();
    Init_RTC();
    Init_SPI();
    Init_LCD();
    Init_Buttons();

    Display_Splash_with_Delay();
    printTextMedium("  HELLO, WORLD! ", 1);
    printTextSmall("      PETI      ", 16);
    printTextSmall("      SAYS      ", 24);
    printTextSmall("       HI       ", 32);
    printTextSmall("1234567890123456", 56);
    printTextSmall("  The Time Is:  ", 72);

    while (1){
        PMM_unlockLPM5();
        Update_Button_States();
        Update_Buttons_Bar();
        Print_CurrentTime();
        printTextLarge(buttonsBar, 100);
        ToggleVCOM();
        __bis_SR_register(LPM0_bits | GIE);
    }
}


// interrupt service routine to handle timer A; drives VCOM and readable clock; mostly for demonstration
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
// In future they should not "do" what their key does, simply set a flag used in main() indicating the button was pressed.

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
