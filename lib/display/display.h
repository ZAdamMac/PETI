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

#define PIXELS_X 128                    // display is 128x128
#define PIXELS_Y 128                    // display is 128x128

#define MODE_DEMO 0x00                // Used for the DISPLAY_updatesOnly if the three-font demonstrator mode is being used
#define MODE_GAME 0x01                  // Used for the DISPLAY_updatesOnly if the multiline game display mode is active
#define MODE_MENU 0x02                  // Used to display the 12-high menu mode in DISPLAY_UpdatesOnly

volatile unsigned char VCOM;             // State of VCOM (either 0x00 or 0x02)
volatile int FORCE_REFRESH;

unsigned char bufferLine[PIXELS_X/8];   // Textmode Line Buffer

typedef struct DisplayFrame{
    char *line0;
    int refresh_L0;
    char *line1;
    int refresh_L1;
    char *line2;
    int refresh_L2;
    char *line3;
    int refresh_L3;
    char *line4;
    int refresh_L4;
    char *line5;
    int refresh_L5;
    char *line6;
    int refresh_L6;
    char *line7;
    int refresh_L7;
    char *line8;
    int refresh_L8;
    char *line9;
    int refresh_L9;
    char *lineA;
    int refresh_LA;
    char *lineB;
    int refresh_LB;
    char *lineC;
    int refresh_LC;
    char *lineD;
    int refresh_LD;
    char *lineE;
    int refresh_LE;
    char *lineF;
    int refresh_LF;
} DisplayFrame;

void Init_LCD(void);
void printTextSmall(const char* text, unsigned char line);
void printTextMedium(const char* text, unsigned char line);
void printTextLarge(const char* text, unsigned char line);
void Init_SPI(void);
void DisplaySplash(void);
void ToggleVCOM(void);
void LCDClearDisplay(void);
void DISPLAY_updatesOnly(DisplayFrame incoming_frame, unsigned int mode);



#endif /* LIB_DISPLAY_DISPLAY_H_ */
