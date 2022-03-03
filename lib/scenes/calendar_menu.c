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
#include <string.h>
#include "scenes_manager.h"
#include "calendar_menu.h"
#include "main.h"
#include "lib/locales/enCA_strings.h"

#define NEXT_SCENE SCENEADDR_main_game  // Under normal use this is probably a different value than DEMOMODE
#define SCENE_TYPE MODE_MENU            // Sets the scene type to use. Done here instead of inline so that a scene template code-page can be developed.

unsigned int calendar_menu_year;                //Integer year, any value from 0 to 4096.
unsigned int calendar_menu_month;               //Integer month bounded 1-12
unsigned int calendar_menu_day;                 //Integer day 0-31. Logic is used to trim for dates.
unsigned int calendar_menu_dow;                 //Integer day 0-6. 0 is sunday, refer to lookup table daysOfWeek in calendar_menu.h
unsigned int calendar_menu_hours;               //Integer hours 0-23
unsigned int calendar_menu_minutes;             //Integer minutes 0-59.
int change_in_digit;                            //Signed int used by the input handler to change the value of the currently-selected object.
unsigned int calmenu_init;                      //Boolean store; used so that the calendar menu doesn't reset to default 1/sec.
unsigned int calmenu_exiting;                   //Boolean store; used so that we can indicate the SET command was issued and we can leave later.
unsigned int save_changes;                      //Boolean store; used to indicate we accepted the exit and the calendar should be updated.
unsigned int cursor_position = 0x00;            //Cursor position, used to track which digit is being manipulated
unsigned int cursor_position_set = 13;          //Fourteen possible positions indexing from 0

int lengthOfMonths[12] = {                      //An array which holds the length of the months. February is included but not used for historical reasons.
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


// We simply require a function to update the various current local time integers with their initial values, to give the menu a place to start.
// This is done instead of static values as the menu will be accessible during the game, allowing the user to adjust the time settings on the fly.
void CALMENU_initCalendarStruct(){
    Calendar currentTime = RTC_C_getCalendarTime(RTC_C_BASE);  // We rely on these vendor-supplied functions to handle the conversions in-hardware for speed.
    calendar_menu_year = RTC_C_convertBCDToBinary(RTC_C_BASE, currentTime.Year);
    calendar_menu_month = RTC_C_convertBCDToBinary(RTC_C_BASE, currentTime.Month);
    calendar_menu_day = RTC_C_convertBCDToBinary(RTC_C_BASE, currentTime.DayOfMonth);
    calendar_menu_dow = RTC_C_convertBCDToBinary(RTC_C_BASE, currentTime.DayOfWeek);
    calendar_menu_hours = RTC_C_convertBCDToBinary(RTC_C_BASE, currentTime.Hours);
    calendar_menu_minutes = RTC_C_convertBCDToBinary(RTC_C_BASE, currentTime.Minutes);
    cursor_position = 0x00; // If not set early on in the menu process when arriving at the menu, it will be in the save position instead.

}

// We need our own scene-specific input handling, which will probably almost always be the case for scenes.
// In this case, we have a certain way to move through the menu that is highly specific to setting the clock.
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
    if (calendar_menu_month > 12){  // The lengths of months get a lot wierder because of Pope Gregory XIII.
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

//This function updates the calendar data integers specific to this scene; not those
//Tracked in or stored by the RTC. It also makes no effort to ensure those values are legal.
//A later function rectifies this data before allowing it to be displayed or written to RTC C.
void CALMENU_updateInternalCalendar(void){
    switch (cursor_position){
        case 0x00 : // Millenium is highlighted.
            calendar_menu_year += (change_in_digit * 1000);
            change_in_digit = 0;
            break;
        case 0x01 : // Century is highlighted.
            calendar_menu_year += (change_in_digit * 100);
            change_in_digit = 0;
            break;
        case 0x02 : // Decade is highlighted.
            calendar_menu_year += (change_in_digit * 10);
            change_in_digit = 0;
            break;
        case 0x03 : // Year is highlighted.
            calendar_menu_year += change_in_digit;
            change_in_digit = 0;
            break;
        case 0x04 : // Tens of the Month is highlighted.
            calendar_menu_month += (change_in_digit * 10);
            change_in_digit = 0;
            break;
        case 0x05 : // Ones of the Month is highlighted.
            calendar_menu_month += change_in_digit;
            change_in_digit = 0;
            break;
        case 0x06 : // Tens of the Date is highlighted
            calendar_menu_day += (change_in_digit * 10);
            change_in_digit = 0;
            break;
        case 0x07 : // Ones of the date is highlighted
            calendar_menu_day += change_in_digit;
            change_in_digit = 0;
            break;
        case 0x08 : // DAY OF WEEK is highlighted
            calendar_menu_dow += change_in_digit;
            change_in_digit = 0;
            break;
        case 0x09 : // Tens of Hours is Highlighted
            calendar_menu_hours += (change_in_digit * 10);
            change_in_digit = 0;
            break;
        case 0x0A : // Ones of Hours is highlighted
            calendar_menu_hours += change_in_digit;
            change_in_digit = 0;
            break;
        case 0x0B : // Tens of Mins is highlighted
            calendar_menu_minutes += (change_in_digit * 10);
            change_in_digit = 0;
            break;
        case 0x0C : // Ones of Mins is highlighted
            calendar_menu_minutes += change_in_digit;
            change_in_digit = 0;
            break;
        case 0x0D : // set is highlighted
            break;
    }
    CALMENU_rectifyCalendar();
}

//This updates top_alterable by breaking out the displayable version of the calendar date and day of the week.
char * CALMENU_dateAndDow(void){
    WORK_STRING[0] = ' '; // Centering Manually for Lulz.
    WORK_STRING[1] = DISPLAY_nthDigit(3, calendar_menu_year); // A convenience function. It'd be nice to do this for other places where this logic is used like in the demo.
    WORK_STRING[2] = DISPLAY_nthDigit(2, calendar_menu_year);
    WORK_STRING[3] = DISPLAY_nthDigit(1, calendar_menu_year);
    WORK_STRING[4] = DISPLAY_nthDigit(0, calendar_menu_year);
    WORK_STRING[5] = '-';
    WORK_STRING[6] = DISPLAY_nthDigit(1, calendar_menu_month);
    WORK_STRING[7] = DISPLAY_nthDigit(0, calendar_menu_month);
    WORK_STRING[8] = '-';
    WORK_STRING[9] = DISPLAY_nthDigit(1, calendar_menu_day);
    WORK_STRING[10] = DISPLAY_nthDigit(0, calendar_menu_day); // TODO: Resume from here, this step is mysteriously broken
    WORK_STRING[11] = ' ';
    WORK_STRING[12] = LARRAY_DAYS_OF_WEEK[calendar_menu_dow][0];  // Strings are arrays of chars, so the dict needs to be called out one index at a time.
    WORK_STRING[13] = LARRAY_DAYS_OF_WEEK[calendar_menu_dow][1];
    WORK_STRING[14] = LARRAY_DAYS_OF_WEEK[calendar_menu_dow][2];
    WORK_STRING[15] = ' ';
    return &WORK_STRING;
}

char * CALMENU_dateAndDowDirective(void){
    unsigned int text_index;
    for (text_index = 0; text_index < 17; ++text_index){ // begin by setting the array to "0"s.
        WORK_STRING[text_index] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
    }
    switch(cursor_position){ // There is an offset for highlighting based on cursor position we need to account for.
            case 0 :
                WORK_STRING[1] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
                break;
            case 1 :
                WORK_STRING[2] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
                break;
            case 2 :
                WORK_STRING[3] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
                break;
            case 3 :
                WORK_STRING[4] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
                break;
            case 4 :
                WORK_STRING[6] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
                break;
            case 5 :
                WORK_STRING[7] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
                break;
            case 6 :
                WORK_STRING[9] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
                break;
            case 7 :
                WORK_STRING[10] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
                break;
            case 8 : // This selection is multiwide
                WORK_STRING[12] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
                WORK_STRING[13] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
                WORK_STRING[14] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
                break;
        }
    return &WORK_STRING;
}

// This updates the WORK_STRING by breaking out the current 24h time.
char * CALMENU_printTime(void){
    WORK_STRING[0] = ' '; // Centering Manually for Lulz.
    WORK_STRING[1] = ' ';
    WORK_STRING[2] = ' ';
    WORK_STRING[3] = ' ';
    WORK_STRING[4] = ' ';
    WORK_STRING[5] = ' ';
    WORK_STRING[6] = DISPLAY_nthDigit(1, calendar_menu_hours);
    WORK_STRING[7] = DISPLAY_nthDigit(0, calendar_menu_hours);
    WORK_STRING[8] = ':';
    WORK_STRING[9] = DISPLAY_nthDigit(1, calendar_menu_minutes);
    WORK_STRING[10] = DISPLAY_nthDigit(0, calendar_menu_minutes);
    WORK_STRING[11] = ' ';
    WORK_STRING[12] = ' ';
    WORK_STRING[13] = ' ';
    WORK_STRING[14] = ' ';
    WORK_STRING[15] = ' ';
    return &WORK_STRING;
}

char* CALMENU_timeDirective(void){
    unsigned int text_index;
    for (text_index = 0; text_index < 17; ++text_index){ // begin by setting the array to "0"s.
        WORK_STRING[text_index] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
    }
    switch(cursor_position){ // There is an offset for highlighting based on cursor position we need to account for.
        case 9 :
            WORK_STRING[6] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
            break;
        case 10 :
            WORK_STRING[7] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
            break;
        case 11 :
            WORK_STRING[9] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
            break;
        case 12 :
            WORK_STRING[10] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
            break;
    }

   return &WORK_STRING;
}

// This function updates all the values in CALMENU frame by interrogating the various scene-specific
// integers of the scene-specific calendar via CALMENU_dateAndDow and CALMENU_printTime, above.
// Some logic is also applied to determine which rows need to be updated based on where the cursor is.
// This technically means we write too often but it's proven to be sufficiently performant not to matter.
void CALMENU_computeNextFrame(void){
    int index, col;
    for (index=0; index<PIXELS_Y/FONT_SIZE_FLOOR_Y; index++){ //It is computationally easier to blank the whole frame then just go back and update it.
        strcpy(DISPLAY_FRAME.frame[index].line, "");
        for (col=0; col<PIXELS_X/FONT_SIZE_FLOOR_X; col++){
            DISPLAY_FRAME.frame[index].directives[col] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
        }
    }
    // There are some static lines that need to be set for icons and suchlike.
    strcpy(DISPLAY_FRAME.frame[0].line, LSTRING_CALMENU_HEADER);
    strcpy(DISPLAY_FRAME.frame[1].line, " \x06\x06\x06\x06 \x06\x06 \x06\x06  \x06 "); //0x06 in this font is an up-arrow
    strcpy(DISPLAY_FRAME.frame[3].line, " \x01\x01\x01\x01 \x01\x01 \x01\x01  \x01 "); //0x01 in this font is a down-arrow
    strcpy(DISPLAY_FRAME.frame[4].line, "      \x06\x06 \x06\x06      "); //0x06 in this font is an up-arrow
    strcpy(DISPLAY_FRAME.frame[6].line, "      \x01\x01 \x01\x01      "); //0x01 in this font is a down-arrow
    strcpy(DISPLAY_FRAME.frame[8].line, "      \x02\x03\x04\x05      "); // SET flag magic string.

    //Some other strings are set by various subfunctions
    strncpy(DISPLAY_FRAME.frame[2].line, CALMENU_dateAndDow(), (PIXELS_X/FONT_SIZE_FLOOR_X));
    strncpy(DISPLAY_FRAME.frame[2].directives, CALMENU_dateAndDowDirective(), (PIXELS_X/FONT_SIZE_FLOOR_X));
    strncpy(DISPLAY_FRAME.frame[5].line, CALMENU_printTime(), (PIXELS_X/FONT_SIZE_FLOOR_X));
    strncpy(DISPLAY_FRAME.frame[5].directives, CALMENU_timeDirective(), (PIXELS_X/FONT_SIZE_FLOOR_X));

    //Finally, check for the set directive:
    if (cursor_position == cursor_position_set){ // SET selected
        for (index=6; index<10; index++){
                DISPLAY_FRAME.frame[8].directives[index] = FONT_ADDR_0 + DIRECTIVE_NEGATIVE;
        }
    }
}

// This function takes all of the scene-specific date state that we just collected and passes it back to the RTC.
void CALMENU_setGlobalCalendar(void){
    RTC_C_holdClock(RTC_C_BASE); // ALWAYS stop a clock before you set anything, ain't you know that?

    Calendar updatingTime;
    updatingTime.Seconds = 0x00;  // We just force back down to 0 seconds.
    updatingTime.Minutes = RTC_C_convertBinaryToBCD(RTC_C_BASE, calendar_menu_minutes); // Doing the conversion in this way makes it faster as it uses the RTC_C's own registers.
    updatingTime.Hours = RTC_C_convertBinaryToBCD(RTC_C_BASE, calendar_menu_hours);
    updatingTime.DayOfWeek = RTC_C_convertBinaryToBCD(RTC_C_BASE, calendar_menu_dow);
    updatingTime.DayOfMonth = RTC_C_convertBinaryToBCD(RTC_C_BASE, calendar_menu_day);
    updatingTime.Month = RTC_C_convertBinaryToBCD(RTC_C_BASE, calendar_menu_month);
    updatingTime.Year = RTC_C_convertBinaryToBCD(RTC_C_BASE, calendar_menu_year);

    RTC_C_initCalendar(RTC_C_BASE, &updatingTime, RTC_C_FORMAT_BCD); // Hey struct, get into your registers.
    RTC_C_startClock(RTC_C_BASE); // Okay let's go!
}

// This is the function actually called by the scene manager when the scene is active. It handles the logic flow
// to determine if we need to stay within the function as well as whether or not the function has recently been run.
void SCENE_CalendarMenu(void){
    if (!calmenu_init){ // We didn't run this yet so we need to update the CALMENU struct to get it relatively recently.
        CALMENU_initCalendarStruct();
        FORCE_REFRESH = true; // Write every line, the scene just changed and that's how we got here.
        calmenu_init = true;  // Okay, we've localized the right variable.
    }
    CALMENU_handleInputs(); // What did the user just do?
    CALMENU_updateInternalCalendar();  // What did what they just did change about the internal state of this scene?
    CALMENU_computeNextFrame();  // How do we show them that change?
    DISPLAY_updatesOnly_enhanced(&DISPLAY_FRAME, MODE_MENU); // Updating the LCD is slow, please update just the parts that matter, and use the MENU layout.
    if (calmenu_exiting){ // The user has asked to leave.
        if (save_changes){ // They have asked to leave by accepting the "SET" button.
            CALMENU_setGlobalCalendar(); // RTC, this is the current time.
        }
        SCENE_ACT = NEXT_SCENE; // We need to go back to wherever this leads, usually to the main game screen.
        calendar_initial_setup_completed = true; // Globally, we have now set the calendar at LEAST once.
        calmenu_init = false; // It is sane to set this back to false for the next time we get here.
        calmenu_exiting = false; // The player can come back to this menu, so we need to reset this.
    }

}


