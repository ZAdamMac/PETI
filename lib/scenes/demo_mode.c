//***************************************************************************************
//  PETI FOSS Virtual Pet - Boot Splash Scene
//
//  Sends the boot splash screen to the display and waits until it is time to exit.
//
//
//
//  Zac Adam-MacEwen (Kensho Security Labs)
//  May 2021
//***************************************************************************************

#include <msp430.h>
#include "driverlib.h"
#include "scenes_manager.h"
#include "demo_mode.h"
#include "lib/display/display.h"
#include "main.h"

char* Print_CurrentTime(void){
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
    return bufferText;
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



void SCENE_demo_mode(void){
    volatile DisplayFrame current_demo_Frame;
    current_demo_Frame.line0 = "  HELLO, WORLD!";
    current_demo_Frame.line1 = "      PETI";
    current_demo_Frame.line2 = "      SAYS";
    current_demo_Frame.line3 = "       HI";
    current_demo_Frame.line4 = "1234567890123456";
    current_demo_Frame.line5 = "  The Time Is:";
    current_demo_Frame.line6 = Print_CurrentTime();
    current_demo_Frame.refresh_L6 = true;
    Update_Buttons_Bar();
    current_demo_Frame.line7 = buttonsBar;
    current_demo_Frame.refresh_L7 = interacted_flag;
    interacted_flag = false;
    DISPLAY_updatesOnly(current_demo_Frame, MODE_DEMO);
}




