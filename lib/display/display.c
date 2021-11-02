//***************************************************************************************
//  PETI FOSS Virtual Pet - Display Controller Code
//
//  Principal Code for TI MSP430FR5994 LaunchPad to communicate as desired with the LCD.
//  This code works with the TI BOOKSXL-SHARP128 booster pack.
//
//  Check the schematics folder for VCS-controlled hardware schematics
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  October 2020 - Updated: May 2021
//***************************************************************************************

#include "display.h"
#include "font.h"
#include "driverlib.h"
#include "splash.h"
#include <msp430.h>


// Initiates the eUSCI_B #1 module to act as the SPI controller.
// Used exclusively to drive the LCD but could be used as a general SPI controller as needed.
void Init_SPI(void) {
    EUSCI_B_SPI_disable(EUSCI_B1_BASE); // disable the EUSCI to be programme

    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5,GPIO_PIN0,GPIO_PRIMARY_MODULE_FUNCTION);
         // Select Port 5, Set Pin 0 to output Primary Module Function, (UCB1TXD/UCB1SIMO)

    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5,GPIO_PIN2,GPIO_PRIMARY_MODULE_FUNCTION);
         // Set port 5, Pin 2 to output Primary Module Function, UCB1CLK.

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5,GPIO_PIN1,GPIO_PRIMARY_MODULE_FUNCTION);
         // Select Port 5, Set Pin 1 to input Primary Module Function, ( UCB1RXD/UCB1SOMI)
         // We don't need this pin, technically, but it's included for completeness.

         // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings

    PMM_unlockLPM5();

         //Initialize UCB1 as SPI Controller

    EUSCI_B_SPI_initMasterParam param = {0};                           // param is struct data type

    param.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    param.clockSourceFrequency = 1000000;   // System might run higher, may need to adjust.
    param.desiredSpiClock = 1000000;  // Theoretical maximum for the target LCD module.
    param.msbFirst = EUSCI_B_SPI_LSB_FIRST; // LCD module is little-endian.
    param.clockPhase = EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT; //not certain correct
    param.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW;  // not certain correct
    param.spiMode = EUSCI_B_SPI_3PIN;

    EUSCI_B_SPI_initMaster(EUSCI_B1_BASE, &param);     //   ***  B SPI is initialized with struct param data

         //Wait for lines to stabilize prior to switching on the SPI
    __delay_cycles(100);
         //Enable SPI module
    EUSCI_B_SPI_enable(EUSCI_B1_BASE);   // clear the UCSWRST bit to enable the SPI

    EUSCI_B_SPI_clearInterrupt(EUSCI_B1_BASE,EUSCI_B_SPI_RECEIVE_INTERRUPT);

    // Disable USCI_B1 RX interrupt - we expect nothing back
    EUSCI_B_SPI_disableInterrupt(EUSCI_B1_BASE,EUSCI_B_SPI_RECEIVE_INTERRUPT);

    GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN3);     // Set P1.3 as output pin for CS of LCD
    GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN3);  // disable LCD Module
         //Wait for devices to initialize.
    __delay_cycles(100);

}

// Provides a shortcut method to calculate the correct clear-screen command for the mLCD module based on current VCOM state
// And send it.
void LCDClearDisplay(void){
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN3);
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, (MLCD_CM | VCOM));
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, 0);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN3);
}

// Provides the LCD controller power and LCD display power to the LCD in the order
// Specified by SHARP, and then sends the clear screen command, which is recommended.
void Init_LCD(void) {

    // We need a function to initialize the LCD, which is non-automatic.
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN2);// Provide power to the LCD's controller.
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN2); // Provide display power to the LCD.
    LCDClearDisplay();
}

// Reverse the bit order of a byte to resolve the mirroring issue. Likely used in all SPI sends.
// Full credit to Stack Overflow user STH for this handy function
// We're trying to remove reliance on it but it might be used in a mirrored print function to save
// Fontspace, and is currently use for the slpash image to not tune them.
unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

//Transfer the line buffer to the display via SPI
// Input is the line number where the buffer should be rendered.
void SPIWriteLine(unsigned char line)
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN3); // Take SCS low to talk to the display
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, (MLCD_WR | VCOM));// send the command to write lines.
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, line); // send the line address to the system

    unsigned char index_of_byte = 0; //loop to send line buffer byte by byte
    unsigned char this_byte;
    while (index_of_byte <= (PIXELS_X/8))
        {
            this_byte = bufferLine[index_of_byte++];
            EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, this_byte);
            while (UCB1STATW & UCBUSY);
        }

    // send 16 bits to latch buffers and end transfer
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, 0x00);
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, 0x00);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN3); // taks eSCS high
}

// write a string to display, truncated after PIXEL_X/8 characters
// input: text      0-terminated string
//        line      vertical position of text, 0-(PIXEL_Y-1)
void printTextSmall(const char* text, unsigned char line)
{
    unsigned char character, bitmap, indexText, indexLineBuffer, indexLine;
    //bool padded;
    //For simplicity, and because we are borrowing an existing font library, we print line-by-line
    indexLine = 0;
    while(indexLine < 8 && line < PIXELS_Y)             //loop for 8 char lines within display
    {
        indexText = 0;
        indexLineBuffer = 0;
        while(indexLineBuffer < (PIXELS_X/8) && (character = text[indexText]) != 0)  // We did not reach the end of the line or the string.
        {
            bitmap = font8x8[character][indexLine];  // Retrieves the byte defining one line of character.
            bufferLine[indexLineBuffer] = bitmap;
            indexLineBuffer++;
            indexText++;
        }

        while(indexLineBuffer < (PIXELS_X/8))  //Pad line for empty characters.
        {
            //padded = true;
            bufferLine[indexLineBuffer] = 0xFF;
            indexLineBuffer++;
        }

       // if(padded) // needed because <= might break things.
       // {
       //     bufferLine[indexLineBuffer] = 0x00;
       //     padded = false;
       // }

        SPIWriteLine(line++);
        indexLine++;
    }
}

// write a string to display, truncated after PIXEL_X/8 characters
// expects an 8x12 font
// input: text      0-terminated string
//        line      vertical position of text, 0-(PIXEL_Y-1)
void printTextMedium(const char* text, unsigned char line)
{
    unsigned char character, bitmap, indexText, indexLineBuffer, indexLine;
    //bool padded;
    //For simplicity, and because we are borrowing an existing font library, we print line-by-line
    indexLine = 0;
    while(indexLine < 12 && line < PIXELS_Y)             //loop for 12 char lines within display
    {
        indexText = 0;
        indexLineBuffer = 0;
        while(indexLineBuffer < (PIXELS_X/8) && (character = text[indexText]) != 0)  // We did not reach the end of the line or the string.
        {
            bitmap = font8x12[character][indexLine];  // Retrieves the byte defining one line of character.
            bufferLine[indexLineBuffer] = bitmap;
            indexLineBuffer++;
            indexText++;
        }

        while(indexLineBuffer < (PIXELS_X/8))  //Pad line for empty characters.
        {
            //padded = true;
            bufferLine[indexLineBuffer] = 0xFF;
            indexLineBuffer++;
        }

        //if(padded) // needed because <= might break things.
        //{
        //    bufferLine[indexLineBuffer] = 0x00;
        //    padded = false;
        //}

        SPIWriteLine(line++);
        indexLine++;
    }
}

// write a string to display, truncated after PIXEL_X/8 characters
// uses a simple directives string to control character inversion
// expects an 8x12 font
// input: text      0-terminated string
//        directives 2 bytes of data.
//        line      vertical position of text, 0-(PIXEL_Y-1)
void printTextMedium_enhanced(const char* text, unsigned char line, const char* directives)
{
    unsigned char character, bitmap, indexText, indexLineBuffer, indexLine;
    //bool padded;
    //For simplicity, and because we are borrowing an existing font library, we print line-by-line
    indexLine = 0;
    while(indexLine < 12 && line < PIXELS_Y)             //loop for 12 char lines within display
    {
        indexText = 0;
        indexLineBuffer = 0;
        while(indexLineBuffer < (PIXELS_X/8) && (character = text[indexText]) != 0)  // We did not reach the end of the line or the string.
        {
            bitmap = font8x12[character][indexLine];  // Retrieves the byte defining one line of character.
            switch (directives[indexText]){
                case '0': // If directive 0, display as normal
                    bitmap = bitmap;
                    break;
                case '1': // If directive 1, invert the character
                    bitmap = ~bitmap;
                    break;
            }
            bufferLine[indexLineBuffer] = bitmap;
            indexLineBuffer++;
            indexText++;
        }

        while(indexLineBuffer < (PIXELS_X/8))  //Pad line for empty characters.
        {
            //padded = true;
            bufferLine[indexLineBuffer] = 0xFF;
            indexLineBuffer++;
        }


        SPIWriteLine(line++);
        indexLine++;
    }
}


// write a string to display, truncated after PIXEL_X/16 characters
// expects a 16x16 font
// input: text      0-terminated string
//        line      vertical position of text, 0-(PIXEL_Y-1)
void printTextLarge(const char* text, unsigned char line)
{
    unsigned char character, bitmap, indexText, indexLineBuffer, indexLine, indexOfOffset;
    //bool padded;
    //For simplicity, and because we are borrowing an existing font library, we print line-by-line
    indexLine = 0;
    indexOfOffset = 0;
    while(indexLine < 16 && line < PIXELS_Y)             //loop for 16 char lines within display
    {
        indexText = 0;
        indexLineBuffer = 0;
        while(indexLineBuffer < (PIXELS_X/8) && (character = text[indexText]) != 0)  // We did not reach the end of the line or the string.
        {
            bitmap = font16x16[character][indexLine+indexOfOffset];  // Retrieves the byte defining the left side of the character.
            bufferLine[indexLineBuffer] = bitmap;
            indexLineBuffer++;
            bitmap = font16x16[character][indexLine+indexOfOffset+1];  // Retrieves the byte defining the right side of the character.
            bufferLine[indexLineBuffer] = bitmap;
            indexLineBuffer++;
            indexText++;
        }

        while(indexLineBuffer < (PIXELS_X/8))  //Pad line for empty characters.
        {
            //padded = true;
            bufferLine[indexLineBuffer] = 0xFF;
            indexLineBuffer++;
        }

        //if(padded) // needed because <= might break things.
        //{
        //    bufferLine[(PIXELS_X/8)-1] = 0x00;  // This is the line that breaks things.
        //    padded = false;
        //}

        SPIWriteLine(line++);
        indexLine++;
        indexOfOffset++;
    }
}


// write a string to display, truncated after PIXEL_X/16 characters
// expects a 16x16 font
// input: text      0-terminated string
//        line      vertical position of text, 0-(PIXEL_Y-1)
void printTextLarge_enhanced(const char* text, unsigned char line, const char* directives)
{
    unsigned char character, bitmap_left, bitmap_right, indexText, indexLineBuffer, indexLine, indexOfOffset, bitmap_left_out, bitmap_right_out;
    //bool padded;
    //For simplicity, and because we are borrowing an existing font library, we print line-by-line
    indexLine = 0;
    indexOfOffset = 0;
    while(indexLine < 16 && line < PIXELS_Y)             //loop for 16 char lines within display
    {
        indexText = 0;
        indexLineBuffer = 0;
        while(indexLineBuffer < (PIXELS_X/8) && (character = text[indexText]) != 0)  // We did not reach the end of the line or the string.
        {
            bitmap_right = font16x16[character][indexLine+indexOfOffset];  // Retrieves the byte defining the left side of the character.
            bitmap_left = font16x16[character][indexLine+indexOfOffset+1];  // Retrieves the byte defining the right side of the character.
            switch (directives[indexText]){
                case '0' : // Print as-is
                    bitmap_right_out = bitmap_right;
                    bitmap_left_out = bitmap_left;
                    break;
                case '1' : // Reverse Character Orientation
                    bitmap_left_out = reverse(bitmap_right); //The orientation of the two segments must also reverse, not just the bits themselves.
                    bitmap_right_out = reverse(bitmap_left);
                    break;
                case '2': // Invert the character
                    bitmap_left_out = ~bitmap_left;
                    bitmap_right_out = ~bitmap_right;
                    break;
                case '3': //Reverse and Invert. Don't see a use, but I'll want it if I don't create it.
                    bitmap_left_out = ~reverse(bitmap_right);
                    bitmap_right_out = ~reverse(bitmap_left);
                    break;
            }
            bufferLine[indexLineBuffer] = bitmap_right_out;
            indexLineBuffer++;
            bufferLine[indexLineBuffer] = bitmap_left_out;
            indexLineBuffer++;
            indexText++;
        }

        while(indexLineBuffer < (PIXELS_X/8))  //Pad line for empty characters.
        {
            //padded = true;
            bufferLine[indexLineBuffer] = 0xFF;
            indexLineBuffer++;
        }

        //if(padded) // needed because <= might break things.
        //{
        //    bufferLine[(PIXELS_X/8)-1] = 0x00;  // This is the line that breaks things.
        //    padded = false;
        //}

        SPIWriteLine(line++);
        indexLine++;
        indexOfOffset++;
    }
}


// prints whatever image is currenlty defined as splash_bitmap in splash.h.
// This expects an image of 128x128 pixels expressed in a BIGENDIAN bit-order, thus reverse.
// TODO/FUTURE: Refactor splash-bitmap and remove the reverse function for better performance.
void DisplaySplash(void){
    volatile unsigned char bitmap, indexLineBuffer, indexLine;
    volatile unsigned int line, indexByte, this_line, sent_line;
    line = 0;
    indexLine = 0;
    indexByte = 0; // The whole bitmap is one byte array.
    while (indexLine <= PIXELS_Y){
        indexLineBuffer = 0;
        while (indexLineBuffer < PIXELS_X/8){
            bitmap = reverse(splash_bitmap[indexByte]);
            bufferLine[indexLineBuffer] = bitmap;
            indexLineBuffer++;
            indexByte++;
        };
        SPIWriteLine(line++);
        indexLine++;
    }
}

//This call literally does what it says - tells the module to switch the VCOM state.
void ToggleVCOM(void){
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN3);
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, (MLCD_SM | VCOM));
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, 0);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN3);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

// The follwing printDeltas functions are all preferred to calling the printText functions if a direct call is needed, however
// there is an even better method to update the screen provided by DISPLAY_updatesOnly(); refer to the wiki for information
// on usage.

// Determine the changed lines of text between the previous and incoming frame, and send only those lines of text through the print process.
// This is specifically for scenes using the MODE_DEMO and the DemoFrame struct.
void printDeltas_demo(DisplayFrame incoming_frame){
    if (incoming_frame.refresh_L0 || FORCE_REFRESH ){
        printTextMedium(incoming_frame.line0, 1);
        incoming_frame.refresh_L0 = false;
    }
    if (incoming_frame.refresh_L1 || FORCE_REFRESH  ){
        printTextSmall(incoming_frame.line1, 16);
        incoming_frame.refresh_L1 = false;
    }
    if (incoming_frame.refresh_L2 || FORCE_REFRESH ){
        printTextSmall(incoming_frame.line2, 24);
        incoming_frame.refresh_L2 = false;
    }
    if (incoming_frame.refresh_L3 || FORCE_REFRESH  ){
        printTextSmall(incoming_frame.line3, 32);
        incoming_frame.refresh_L3 = false;
    }
    if (incoming_frame.refresh_L4 || FORCE_REFRESH ){
        printTextSmall(incoming_frame.line4, 56);
        incoming_frame.refresh_L4 = false;
    }
    if (incoming_frame.refresh_L5 || FORCE_REFRESH  ){
        printTextSmall(incoming_frame.line5, 72);
        incoming_frame.refresh_L5 = false;
    }
    if (incoming_frame.refresh_L6 || FORCE_REFRESH  ){
        printTextSmall(incoming_frame.line6, 88);
        incoming_frame.refresh_L6 = false;
    }
    if (incoming_frame.refresh_L7 || FORCE_REFRESH ){
        printTextLarge(incoming_frame.line7, 100);
        incoming_frame.refresh_L7 = false;
    }
}

// 10 rows of the 8x12 font centered vertically on the screen, based on the incoming frame
// to determine if a row should be reprinted.
void printDeltas_menu(DisplayFrame incoming_frame){
    if (incoming_frame.refresh_L0 || FORCE_REFRESH ){
        printTextMedium_enhanced(incoming_frame.line0, 4, incoming_frame.directive_L0);
        incoming_frame.refresh_L0 = false;
    }
    if (incoming_frame.refresh_L1 || FORCE_REFRESH  ){
        printTextMedium_enhanced(incoming_frame.line1, 16, incoming_frame.directive_L1);
        incoming_frame.refresh_L1 = false;
    }
    if (incoming_frame.refresh_L2 || FORCE_REFRESH ){
        printTextMedium_enhanced(incoming_frame.line2, 28, incoming_frame.directive_L2);
        incoming_frame.refresh_L2 = false;
    }
    if (incoming_frame.refresh_L3 || FORCE_REFRESH  ){
        printTextMedium_enhanced(incoming_frame.line3, 40, incoming_frame.directive_L3);
        incoming_frame.refresh_L3 = false;
    }
    if (incoming_frame.refresh_L4 || FORCE_REFRESH ){
        printTextMedium_enhanced(incoming_frame.line4, 52, incoming_frame.directive_L4);
        incoming_frame.refresh_L4 = false;
    }
    if (incoming_frame.refresh_L5 || FORCE_REFRESH  ){
        printTextMedium_enhanced(incoming_frame.line5, 64, incoming_frame.directive_L5);
        incoming_frame.refresh_L5 = false;
    }
    if (incoming_frame.refresh_L6 || FORCE_REFRESH  ){
        printTextMedium_enhanced(incoming_frame.line6, 76, incoming_frame.directive_L6);
        incoming_frame.refresh_L6 = false;
    }
    if (incoming_frame.refresh_L7 || FORCE_REFRESH ){
        printTextMedium_enhanced(incoming_frame.line7, 88, incoming_frame.directive_L7);
        incoming_frame.refresh_L7 = false;
    }
    if (incoming_frame.refresh_L8 || FORCE_REFRESH ){
        printTextMedium_enhanced(incoming_frame.line8, 100, incoming_frame.directive_L8);
        incoming_frame.refresh_L8 = false;
    }
    if (incoming_frame.refresh_L9 || FORCE_REFRESH ){
        printTextMedium_enhanced(incoming_frame.line9, 112, incoming_frame.directive_L9);
        incoming_frame.refresh_L9 = false;
    }
}

// Special Print mode Consisting Of:
// 1 row of 8x12 text, 16 characters wide (the "Upper Menu Bar")
// 6 rows of 16x16 text, 8 characters wide (collectively the "Playing Field")
// 1 row of 8x8 text, 16 characters wide (the "DEBUG" bar) <- considered safe to write to directly if needed.
//1 row of 8x12 text, 16 characters wide (the "lower menu bar")

void printDeltas_game(DisplayFrame incoming_frame){
    if (incoming_frame.refresh_L0 || FORCE_REFRESH ){
        printTextMedium_enhanced(incoming_frame.line0, 1, incoming_frame.directive_L0);
        incoming_frame.refresh_L0 = false;
    }
    if (incoming_frame.refresh_L1 || FORCE_REFRESH  ){
        printTextLarge_enhanced(incoming_frame.line1, 13, incoming_frame.directive_L1);
        incoming_frame.refresh_L1 = false;
    }
    if (incoming_frame.refresh_L2 || FORCE_REFRESH ){
        printTextLarge_enhanced(incoming_frame.line2, 29, incoming_frame.directive_L2);
        incoming_frame.refresh_L2 = false;
    }
    if (incoming_frame.refresh_L3 || FORCE_REFRESH  ){
        printTextLarge_enhanced(incoming_frame.line3, 45, incoming_frame.directive_L3);
        incoming_frame.refresh_L3 = false;
    }
    if (incoming_frame.refresh_L4 || FORCE_REFRESH ){
        printTextLarge_enhanced(incoming_frame.line4, 61, incoming_frame.directive_L4);
        incoming_frame.refresh_L4 = false;
    }
    if (incoming_frame.refresh_L5 || FORCE_REFRESH  ){
        printTextLarge_enhanced(incoming_frame.line5, 77, incoming_frame.directive_L5);
        incoming_frame.refresh_L5 = false;
    }
    if (incoming_frame.refresh_L6 || FORCE_REFRESH  ){
        printTextLarge_enhanced(incoming_frame.line6, 93, incoming_frame.directive_L6);
        incoming_frame.refresh_L6 = false;
    }
    if (incoming_frame.refresh_L8 || FORCE_REFRESH ){
        printTextSmall(incoming_frame.line8, 109);
        incoming_frame.refresh_L8 = false;
    }
    if (incoming_frame.refresh_L9 || FORCE_REFRESH ){
        printTextMedium_enhanced(incoming_frame.line9, 117, incoming_frame.directive_L9);
        incoming_frame.refresh_L9 = false;
    }
}

//Determine which mode of changes-only screen update to use, and drop the incoming frame of output to that function.
//Expects a displayframe object compatible with the outgoing mode (and the scene that constructed it) and calls the correct
//printDeltas_XXXXX function depending on the value of MODE. For convenience any new modes added should be given descriptive
//symbolic names in display.h
void DISPLAY_updatesOnly(DisplayFrame incoming_frame, unsigned int mode){
    switch (mode){
        case MODE_DEMO :
            printDeltas_demo(incoming_frame);
            break;
        case MODE_GAME :
            printDeltas_game(incoming_frame);
            break;
        case MODE_MENU :
            printDeltas_menu(incoming_frame);
            break;
    }
    FORCE_REFRESH = false;
}


//Convenience function for breaking out multidigit numbers for display. Automatically elevates to the correct character code (subtract '0' if you need the number):
//Anywhere this method is not being used should be replaced as noticed unless there is a special-handling reason not to do so.
char DISPLAY_nthDigit(int digit_index_from_least, int full_value){
    while (digit_index_from_least--)
        full_value /= 10;
    return full_value%10 + '0';
}
