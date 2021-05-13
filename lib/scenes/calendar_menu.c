//***************************************************************************************
//  PETI FOSS Virtual Pet - Calendar Setting Menu Code
//
//  A PETI scene which allows the user an interactive menu by which to set the RTC_C
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  March 2021
//***************************************************************************************

#include "lib/display/display.h"
#include "lib/hwinit/hwinit.h"
#include "driverlib.h"
#include <msp430.h>
#include "scenes_manager.h"
#include "calendar_menu.h"
#include "main.h"
#include "lib/locales/enCA_strings.h"

#define NEXT_SCENE SCENEADDR_main_game  // Under normal use this is probably a different value than DEMOMODE
#define SCENE_TYPE MODE_MENU            // Sets the scene type to use. Done here instead of inline so that a scene template code-page can be developed.

unsigned int calendar_menu_year;                //Integer year, any value from 0 to 4096.
unsigned int calendar_menu_month;               //Integer month bounded 1-12
unsigned int calendar_menu_day;                 //Integer day 0-31. Logic is used to trim for dates.
unsigned int calendar_menu_dow;                      //Integer day 0-6. 0 is sunday, refer to lookup table daysOfWeek in calendar_menu.h
unsigned int calendar_menu_hours;               //Integer hours 0-23
unsigned int calendar_menu_minutes;             //Integer minutes 0-59.
int change_in_digit;                            //Signed int used by the input handler to change the value of the currently-selected object.
unsigned int calmenu_init;                      //Boolean store; used so that the calendar menu doesn't reset to default 1/sec.
unsigned int calmenu_exiting;                   //Boolean store; used so that we can indicate the SET command was issued and we can leave later.
unsigned int save_changes;                      //Boolean store; used to indicate we accepted the exit and the calendar should be updated.
unsigned int cursor_position = 0x00;            //Cursor position, used to track which digit is being manipulated
unsigned int cursor_position_set = 13;          //Fourteen possible positions indexing from 0
DisplayFrame CALMENU_Frame;
char top_alterable[17];
char bottom_alterable[17];

int lengthOfMonths[12] = {
                                   31,
                                   28,
                                   31,
                                   30,
                                   31,
                                   30,
                                   31,
                                   31,
                                   30,
                                   31,
                                   30,
                                   31
};

void CALMENU_initCalendarStruct(){
    Calendar currentTime = RTC_C_getCalendarTime(RTC_C_BASE);
    calendar_menu_year = RTC_C_convertBCDToBinary(RTC_C_BASE, currentTime.Year);
    calendar_menu_month = RTC_C_convertBCDToBinary(RTC_C_BASE, currentTime.Month);
    calendar_menu_day = RTC_C_convertBCDToBinary(RTC_C_BASE, currentTime.DayOfMonth);
    calendar_menu_dow = RTC_C_convertBCDToBinary(RTC_C_BASE, currentTime.DayOfWeek);
    calendar_menu_hours = RTC_C_convertBCDToBinary(RTC_C_BASE, currentTime.Hours);
    calendar_menu_minutes = RTC_C_convertBCDToBinary(RTC_C_BASE, currentTime.Minutes);
    cursor_position = 0x00;

}

void CALMENU_handleInputs(void){
    if (buttons_state & button_a_toggle){  //A increases the value in this position by one.
        change_in_digit = 1;
        buttons_state ^= button_a_toggle; // unset the toggle for convenience. We've actioned this since we saw it last.
    }
    if (buttons_state & button_b_toggle){  //B decreases the value in this position by one.
        change_in_digit = -1;
        buttons_state ^= button_b_toggle;
    }
    if (buttons_state & button_c_toggle){  //C accepts this value by moving onto the next frame, OR hits the set requirement if we're on that index.
        if (cursor_position == cursor_position_set){
            calmenu_exiting = true;
            save_changes = true;
            buttons_state ^= button_c_toggle;
        }
        else {
            cursor_position++;
            buttons_state ^= button_c_toggle;
        }
    }
    if (buttons_state & button_d_toggle){  //D moves back a value to allow for corrections or, if at the thousands of the year, cancels without saving.
        if (cursor_position == 0){
            calmenu_exiting = true;
            save_changes = false;
            buttons_state ^= button_d_toggle;
        }
        else {
            cursor_position--;
            buttons_state ^= button_d_toggle;
        }
    }
}

// A function is needed within CALMENU to be able to look at the menu sensibly and adjust to legal-only values before they are written to the screen or passed to the RTC itself.
void CALMENU_rectifyCalendar(void){
    calendar_menu_year = calendar_menu_year % 4096; // the system will accept any value in the range so defined. Just in case 2000 years from now someone still wants one of these.
    calendar_menu_dow = calendar_menu_dow % 7;
    calendar_menu_hours = calendar_menu_hours % 24;
    calendar_menu_minutes = calendar_menu_minutes % 60;
    if (calendar_menu_month > 12){  // The lengths of months get a lot weirder because of Pope Gregory XIII.
        calendar_menu_month -= 12; // Faster than modular math.
    }
    else if (calendar_menu_month < 1){
        calendar_menu_month += 12;
    }
    if ((calendar_menu_month == 2) && ((calendar_menu_year % 4 == 0) && (calendar_menu_year % 100 != 0 || calendar_menu_year % 400 == 0 ))){
        //Yeah, leap years are weird. Thanks Greg.
        if (calendar_menu_day > 29){
            calendar_menu_day -= 29;
        }
        else if (calendar_menu_day < 1){
            calendar_menu_day += 29;
        }
    }
    else{
        if (calendar_menu_day > lengthOfMonths[(calendar_menu_month-1)]){
            calendar_menu_day -= lengthOfMonths[(calendar_menu_month-1)];
        }
        else if (calendar_menu_day < 1){
            calendar_menu_day += lengthOfMonths[(calendar_menu_month-1)];
        }
    }
}

void CALMENU_updateInternalCalendar(void){
    switch (cursor_position){
        case 0x00 : // Millenium is highlighted.
            calendar_menu_year += (change_in_digit * 1000);
            change_in_digit = 0;
            CALMENU_Frame.refresh_L2 = true;
            break;
        case 0x01 : // Century is highlighted.
            calendar_menu_year += (change_in_digit * 100);
            change_in_digit = 0;
            CALMENU_Frame.refresh_L2 = true;
            break;
        case 0x02 : // Decade is highlighted.
            calendar_menu_year += (change_in_digit * 10);
            change_in_digit = 0;
            CALMENU_Frame.refresh_L2 = true;
            break;
        case 0x03 : // Year is highlighted.
            calendar_menu_year += change_in_digit;
            change_in_digit = 0;
            CALMENU_Frame.refresh_L2 = true;
            break;
        case 0x04 : // Tens of the Month is highlighted.
            calendar_menu_month += (change_in_digit * 10);
            change_in_digit = 0;
            CALMENU_Frame.refresh_L2 = true;
            break;
        case 0x05 : // Ones of the Month is highlighted.
            calendar_menu_month += change_in_digit;
            change_in_digit = 0;
            CALMENU_Frame.refresh_L2 = true;
            break;
        case 0x06 : // Tens of the Date is highlighted
            calendar_menu_day += (change_in_digit * 10);
            change_in_digit = 0;
            CALMENU_Frame.refresh_L2 = true;
            break;
        case 0x07 : // Ones of the date is highlighted
            calendar_menu_day += change_in_digit;
            change_in_digit = 0;
            CALMENU_Frame.refresh_L2 = true;
            break;
        case 0x08 : // DAY OF WEEK is highlighted
            calendar_menu_dow += change_in_digit;
            change_in_digit = 0;
            CALMENU_Frame.refresh_L2 = true;
            break;
        case 0x09 : // Tens of Hours is Highlighted
            calendar_menu_hours += (change_in_digit * 10);
            change_in_digit = 0;
            CALMENU_Frame.refresh_L5 = true;
            break;
        case 0x0A : // Ones of Hours is highlighted
            calendar_menu_hours += change_in_digit;
            change_in_digit = 0;
            break;
        case 0x0B : // Tens of Mins is highlighted
            calendar_menu_minutes += (change_in_digit * 10);
            change_in_digit = 0;
            CALMENU_Frame.refresh_L5 = true;
            break;
        case 0x0C : // Ones of Mins is highlighted
            calendar_menu_minutes += change_in_digit;
            change_in_digit = 0;
            CALMENU_Frame.refresh_L5 = true;
            CALMENU_Frame.refresh_L8 = true;
            break;
        case 0x0D : // set is highlighted
            CALMENU_Frame.refresh_L8 = true;
            break;
    }
    CALMENU_rectifyCalendar();
}

char * CALMENU_dateAndDow(void){
    top_alterable[0] = ' '; // Centering Manually for Lulz.
    top_alterable[1] = DISPLAY_nthDigit(3, calendar_menu_year); // A convenience function. It'd be nice to do this for other places where this logic is used like in the demo.
    top_alterable[2] = DISPLAY_nthDigit(2, calendar_menu_year);
    top_alterable[3] = DISPLAY_nthDigit(1, calendar_menu_year);
    top_alterable[4] = DISPLAY_nthDigit(0, calendar_menu_year);
    top_alterable[5] = '-';
    top_alterable[6] = DISPLAY_nthDigit(1, calendar_menu_month);
    top_alterable[7] = DISPLAY_nthDigit(0, calendar_menu_month);
    top_alterable[8] = '-';
    top_alterable[9] = DISPLAY_nthDigit(1, calendar_menu_day);
    top_alterable[10] = DISPLAY_nthDigit(0, calendar_menu_day);
    top_alterable[11] = ' ';
    top_alterable[12] = daysOfWeek[calendar_menu_dow][0];  // Strings are arrays of chars, so the dict needs to be called out one index at a time.
    top_alterable[13] = daysOfWeek[calendar_menu_dow][1];
    top_alterable[14] = daysOfWeek[calendar_menu_dow][2];
    top_alterable[15] = ' ';
    switch(cursor_position){ // There is an offset for highlighting based on cursor position we need to account for.
        case 0 :
            top_alterable[1] += 80;
            break;
        case 1 :
            top_alterable[2] += 80;
            break;
        case 2 :
            top_alterable[3] += 80;
            break;
        case 3 :
            top_alterable[4] += 80;
            break;
        case 4 :
            top_alterable[6] += 80;
            break;
        case 5 :
            top_alterable[7] += 80;
            break;
        case 6 :
            top_alterable[9] += 80;
            break;
        case 7 :
            top_alterable[10] += 80;
            break;
        case 8 : // This selection is multiwide
            top_alterable[12] += 80;
            top_alterable[13] += 80;
            top_alterable[14] += 80;
            break;
    }
    return top_alterable;
}


char * CALMENU_printTime(void){
    bottom_alterable[0] = ' '; // Centering Manually for Lulz.
    bottom_alterable[1] = ' ';
    bottom_alterable[2] = ' ';
    bottom_alterable[3] = ' ';
    bottom_alterable[4] = ' ';
    bottom_alterable[5] = ' ';
    bottom_alterable[6] = DISPLAY_nthDigit(1, calendar_menu_hours);
    bottom_alterable[7] = DISPLAY_nthDigit(0, calendar_menu_hours);
    bottom_alterable[8] = ':';
    bottom_alterable[9] = DISPLAY_nthDigit(1, calendar_menu_minutes);
    bottom_alterable[10] = DISPLAY_nthDigit(0, calendar_menu_minutes);
    bottom_alterable[11] = ' ';
    bottom_alterable[12] = ' ';
    bottom_alterable[13] = ' ';
    bottom_alterable[14] = ' ';
    bottom_alterable[15] = ' ';
    switch(cursor_position){ // There is an offset for highlighting based on cursor position we need to account for.
        case 9 :
            bottom_alterable[6] += 80;
            break;
        case 10 :
            bottom_alterable[7] += 80;
            break;
        case 11 :
            bottom_alterable[9] += 80;
            break;
        case 12 :
            bottom_alterable[10] += 80;
            break;
    }
    return bottom_alterable;
}

void CALMENU_computeNextFrame(void){
    CALMENU_Frame.line0 = "Enter the time:";
    CALMENU_Frame.line1 = " \x0A\x0A\x0A\x0A \x0A\x0A \x0A\x0A  \x0A "; //Ox0A in this font is an up-arrow
    CALMENU_Frame.line2 = CALMENU_dateAndDow();
    CALMENU_Frame.line3 = " \x01\x01\x01\x01 \x01\x01 \x01\x01  \x01 "; //0x01 in this font is a down-arrow
    CALMENU_Frame.line4 = "      \x0A\x0A \x0A\x0A      ";
    CALMENU_Frame.line5 = CALMENU_printTime();
    CALMENU_Frame.line6 = "      \x01\x01 \x01\x01     ";
    CALMENU_Frame.line7 = "";
    CALMENU_Frame.line9 = "";
    if (cursor_position == cursor_position_set){
        CALMENU_Frame.line8 = "      \x06\x07\x08\x09      ";  // SET selected
    }
    else{
        CALMENU_Frame.line8 = "      \x02\x03\x04\x05      ";  // SET unselected
    }
    if (cursor_position <= 9){
        CALMENU_Frame.refresh_L2 = true;
    }
    else if (9 <= cursor_position <= cursor_position_set){
        CALMENU_Frame.refresh_L5 = true;
    }
    else if ( 12 <= cursor_position == cursor_position_set){
        CALMENU_Frame.refresh_L8 = true;
    }

}

void CALMENU_setGlobalCalendar(void){
    RTC_C_holdClock(RTC_C_BASE); // ALWAYS stop a clock before you set anything, ain't you know that?

    Calendar updatingTime;
    updatingTime.Seconds = 0x00;  // We just force back down to 0 seconds.
    updatingTime.Minutes = RTC_C_convertBinaryToBCD(RTC_C_BASE, calendar_menu_minutes);
    updatingTime.Hours = RTC_C_convertBinaryToBCD(RTC_C_BASE, calendar_menu_hours);
    updatingTime.DayOfWeek = RTC_C_convertBinaryToBCD(RTC_C_BASE, calendar_menu_dow);
    updatingTime.DayOfMonth = RTC_C_convertBinaryToBCD(RTC_C_BASE, calendar_menu_day);
    updatingTime.Month = RTC_C_convertBinaryToBCD(RTC_C_BASE, calendar_menu_month);
    updatingTime.Year = RTC_C_convertBinaryToBCD(RTC_C_BASE, calendar_menu_year);

    RTC_C_initCalendar(RTC_C_BASE, &updatingTime, RTC_C_FORMAT_BCD); // Hey struct, get into your registers.
    RTC_C_startClock(RTC_C_BASE); // Okay let's go!
}

void SCENE_CalendarMenu(void){
    if (!calmenu_init){
        CALMENU_initCalendarStruct();
        FORCE_REFRESH = true;
        calmenu_init = true;
    }
    CALMENU_handleInputs();
    CALMENU_updateInternalCalendar();
    CALMENU_computeNextFrame();
    DISPLAY_updatesOnly(CALMENU_Frame, MODE_MENU);
    if (calmenu_exiting){
        if (save_changes){
            CALMENU_setGlobalCalendar();
            calendar_initial_setup_completed = true;
        }
        SCENE_ACT = NEXT_SCENE;
    }

}


