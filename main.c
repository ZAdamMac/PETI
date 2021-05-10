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
#include "lib/hwinit/hwinit.h"
#include <msp430.h>

#define VERSION " POC-2021-05-10"       //Max 16 characters.
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
    Init_Watchdog();
    PMM_unlockLPM5();
    VCOM = MLCD_VCOM; // Set the initial state of the VCOM bit.
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
