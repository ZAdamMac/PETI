//***************************************************************************************
//  PETI FOSS Virtual Pet - Display Controller Code
//
//  Principal Code for TI MSP430FR5994 LaunchPad to communicate as desired with the LCD.
//  This code works with the TI BOOKSXL-SHARP128 booster pack.
//
//  Check the schematics folder for VCS-controlled hardware schematics
//
//
//  Zac Adam-MacEwen (Kensho Security Labs)
//  October 2020
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
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN2);// Provide power to the LCD's controller.
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN2); // Provide display power to the LCD.
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN2);
    LCDClearDisplay();
}

// Reverse the bit order of a byte to resolve the mirroring issue. Likely used in all SPI sends.
// Full credit to Stack Overflow user STH for this handy function
// We're trying to remove reliance on it but it might be used /optionally/ in a mirrored print function to save
// Fontspace.
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
    while (index_of_byte <= (PIXELS_X/8))
        {
            EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, bufferLine[index_of_byte++]);
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
    bool padded;
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
    bool padded;
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


// write a string to display, truncated after PIXEL_X/16 characters
// expects a 16x16 font
// input: text      0-terminated string
//        line      vertical position of text, 0-(PIXEL_Y-1)
void printTextLarge(const char* text, unsigned char line)
{
    unsigned char character, bitmap, indexText, indexLineBuffer, indexLine, indexOfOffset;
    bool padded;
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
}
