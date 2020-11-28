/*
 * display.h
 *
 *  Created on: Oct 9, 2020
 *      Author: patches
 */

#ifndef LIB_DISPLAY_DISPLAY_H_
#define LIB_DISPLAY_DISPLAY_H_


#endif /* LIB_DISPLAY_DISPLAY_H_ */


#define MLCD_WR 0x01                    // MLCD write line command
#define MLCD_CM 0x04                    // MLCD clear memory command
#define MLCD_SM 0x00                    // MLCD static mode command
#define MLCD_VCOM 0x02                  // MLCD VCOM bit

#define PIXELS_X 128                    // display is 128x128
#define PIXELS_Y 128                    // display is 128x128

volatile unsigned char VCOM;             // State of VCOM (either 0x00 or 0x02)

unsigned char bufferLine[PIXELS_X/8];   // Textmode Line Buffer

void Init_LCD(void);
void printTextSmall(const char* text, unsigned char line);
void printTextMedium(const char* text, unsigned char line);
void printTextLarge(const char* text, unsigned char line);
void Init_SPI(void);
void DisplaySplash(void);
void ToggleVCOM(void);
void LCDClearDisplay(void);
