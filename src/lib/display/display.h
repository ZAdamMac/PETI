/*
 * display.h
 *
 *  Created on: Oct 9, 2020
 *      Author: patches
 */

#ifndef LIB_DISPLAY_DISPLAY_H_
#define LIB_DISPLAY_DISPLAY_H_


#define MLCD_WR 0x01                    // MLCD write line command
#define MLCD_CM 0x04                    // MLCD clear memory command
#define MLCD_SM 0x00                    // MLCD static mode command
#define MLCD_VCOM 0x02                  // MLCD VCOM bit

#define PIXELS_X 128                    // display is 128x128; wherever possible refrence these symbols rather than doing direct calculation to set widths
#define PIXELS_Y 128                    // display is 128x128; or heights

#define FONT_SIZE_FLOOR_X 8             // X width of the minimum size font in pixels
#define FONT_SIZE_FLOOR_Y 8             // Y height of the minimum size font in pixels

#define MODE_DEMO 0x00                  // Used for the DISPLAY_updatesOnly if the three-font demonstrator mode is being used
#define MODE_GAME 0x01                  // Used for the DISPLAY_updatesOnly if the multiline game display mode is active
#define MODE_MENU 0x02                  // Used to display the 12-high menu mode in DISPLAY_UpdatesOnly
#define MODE_MINIGAME 0x03              // Used to display the 8-row Large mode in DISPLAY_UpdatesOnly

#define FONT_ADDR_0 0x00                // Addresses for various fonts. Each Display_print function will interpret these vaguely differently.
#define FONT_ADDR_1 0x10                // the naming convention for each font should provide _N after fontXxY, omitting _0.
#define FONT_ADDR_2 0x20
#define FONT_ADDR_3 0x30
#define FONT_ADDR_4 0x40
#define FONT_ADDR_5 0x50
#define FONT_ADDR_6 0x60
#define FONT_ADDR_7 0x70
#define FONT_ADDR_8 0x80
#define FONT_ADDR_9 0x90
#define FONT_ADDR_A 0xA0
#define FONT_ADDR_B 0xB0
#define FONT_ADDR_C 0xC0
#define FONT_ADDR_D 0xD0
#define FONT_ADDR_E 0xE0
#define FONT_ADDR_F 0xF0

#define DIRECTIVE_NORMAL 0x01           // Define symbols for font operation directives; these should be universal in all three print functions.
#define DIRECTIVE_REVERSED 0x02
#define DIRECTIVE_NEGATIVE 0x03
#define DIRECTIVE_REVERSED_NEGATIVE 0x04


volatile unsigned char VCOM;            // State of VCOM (either 0x00 or 0x02)
volatile int FORCE_REFRESH;             // Int treated as boolean to force a full refresh of the screen without the need to set each refresh bit for the DisplayFrame
                                        // Useful for example when moving from one screen to another.
volatile int DISPLAY_STATUS;            // Useful for tracking if the display is currently awake or asleep.

unsigned char bufferLine[PIXELS_X/8];   // Textmode Line Buffer

typedef struct DisplayFrame{            // A scene can initialize an instance of this strut and pass it through to the
    char *line0;                        // relevant DISPLAY_updatesOnly function to have it dunamically printed to the screen.
    int refresh_L0;                     // the usage of these char *lineX arrays will vary depending on the MODE value also provided.
    char *directive_L0;
    char *line1;                        // the refresh_LX values are used as an effective boolean to determine if the value in the corresponding
    int refresh_L1;                     // line should be printed on this go-around.
    char *directive_L1;
    char *line2;                        // directive is used to supply a directive value to the output for the enhanced printTextLarge output
    int refresh_L2;
    char *directive_L2;
    char *line3;
    int refresh_L3;
    char *directive_L3;
    char *line4;
    char *directive_L4;
    int refresh_L4;
    char *directive_L5;
    char *line5;
    int refresh_L5;
    char *directive_L6;
    char *line6;
    int refresh_L6;
    char *directive_L7;
    char *line7;
    int refresh_L7;
    char *directive_L8;
    char *line8;
    int refresh_L8;
    char *directive_L9;
    char *line9;
    char *directive_LA;
    int refresh_L9;
    char *lineA;
    int refresh_LA;
    char *directive_LB;
    char *lineB;
    int refresh_LB;
    char *directive_LC;
    char *lineC;
    int refresh_LC;
    char *directive_LD;
    char *lineD;
    int refresh_LD;
    char *directive_LE;
    char *lineE;
    int refresh_LE;
    char *directive_LF;
    char *lineF;
    int refresh_LF;
} DisplayFrame;

// v 0.0.6 and later use a new display frame type based around a structure of `DisplayLine` structures as defined below.

typedef struct DisplayLine{
    char line[PIXELS_X/FONT_SIZE_FLOOR_X+1];       // The text on this line to be displayed.
    char directives[PIXELS_X/FONT_SIZE_FLOOR_X+1]; // The directive to control directive-oriented print functions (i.e. highlighting)
} DisplayLine;

typedef struct DisplayFrameNew{
    DisplayLine frame[PIXELS_Y/FONT_SIZE_FLOOR_Y];  // The display Frame is simply an array of lines as defined above.
} DisplayFrameNew;

DisplayFrameNew DISPLAY_FRAME;      // The global instance of DisplayFrameNew. Anyone using this should use THIS one.
DisplayFrameNew PREVIOUS_FRAME;

char WORK_STRING[PIXELS_X/FONT_SIZE_FLOOR_X]; // A working string to use for all string construction operations.

// These functions are defined fully and commentated in display.c
void Init_LCD(void);
void printTextSmall(const char* text, unsigned char line);
void printTextMedium(const char* text, unsigned char line, const char* directives);
void printTextLarge(const char* text, unsigned char line, const char* directives);
void Init_SPI(void);
void DisplaySplash(void);
void ToggleVCOM(void);
void LCDClearDisplay(void);
void DISPLAY_updatesOnly(DisplayFrame incoming_frame, unsigned int mode);
void DISPLAY_updatesOnly_enhanced(DisplayFrameNew *incoming_frame, unsigned int mode);
char DISPLAY_nthDigit(int digit_index_from_least, int full_value);
void DISPLAY_wakeLCD(void);
void DISPLAY_sleepLCD(void);


#endif /* LIB_DISPLAY_DISPLAY_H_ */
