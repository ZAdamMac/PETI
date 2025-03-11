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
#include "lib/display/scene_definitions.h"
#include "lib/display/font.h"
#include "driverlib.h"
#include "splash.h"
#include <msp430.h>
#include <string.h>


// Initiates the eUSCI_B #1 module to act as the SPI controller.
// Used exclusively to drive the LCD but could be used as a general SPI controller as needed.
// FUTURE: Move to hwinit, it better belongs there.
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

    GPIO_setAsOutputPin(GPIO_PORT_P8,GPIO_PIN0);     // Set P8.0 as output pin for CS of EXP
    GPIO_setOutputLowOnPin(GPIO_PORT_P8,GPIO_PIN0);  // disable LCD Module

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
    FORCE_REFRESH = 0x01; // If coming back from sleep, this is needed. If first boot, harmless.
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
    DISPLAY_STATUS = 0x01;
}


// Provides the LCD controller power and LCD display power to the LCD in the order
// Specified by SHARP, and then sends the clear screen command, which is recommended.
// Differs from Init_LCD in that we assume the control registers for GPIO are already
// set up.
void DISPLAY_wakeLCD(void) {
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN2);// Provide power to the LCD's controller.
    GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN2); // Provide display power to the LCD.
    LCDClearDisplay();
    DISPLAY_STATUS = 0x01;
}


// Provides the LCD controller power and LCD display power to the LCD in the order
// Specified by SHARP, and then sends the clear screen command, which is recommended.
void DISPLAY_sleepLCD(void) {
    // We need a function to initialize the LCD, which is non-automatic.
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN2);// Provide power to the LCD's controller.
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN2); // Provide display power to the LCD.
    DISPLAY_STATUS = 0x00;
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
void printTextSmall(const char* text, unsigned char line, const char* directives)
{
    unsigned char character, bitmap, indexText, indexLineBuffer, indexLine, operation;
    //bool padded;
    //For simplicity, and because we are borrowing an existing font library, we print line-by-line
    indexLine = 0;
    while(indexLine < 8 && line < PIXELS_Y)             //loop for 8 char lines within display
    {
        indexText = 0;
        indexLineBuffer = 0;
        while(indexLineBuffer < (PIXELS_X/8) && (character = text[indexText]) != 0)  // We did not reach the end of the line or the string.
        {
            operation = directives[indexText] & 0xF;
            bitmap = font8x8[character][indexLine];  // Retrieves the byte defining one line of character.
            switch (operation){
                case DIRECTIVE_NORMAL : // Print as-is
                    break;
                case DIRECTIVE_REVERSED : // Reverse Character Orientation
                    bitmap = reverse(bitmap);
                    break;
                case DIRECTIVE_NEGATIVE: // Invert the character
                    bitmap = ~bitmap;
                    break;
                case DIRECTIVE_REVERSED_NEGATIVE: //Reverse and Invert. Don't see a use, but I'll want it if I don't create it.
                    bitmap = ~reverse(bitmap);
                    break;
                case DIRECTIVE_SLATS_THIN:  //Make every 4th line a string of black pixels instead of the right data.
                    if (indexLine % 4 == 0){
                        bitmap = 0x00;
                    }
                    break;
                case DIRECTIVE_SLATS_MED:  //Make every 4th and 5th line a string of black pixels instead of the right data.
                    if ((indexLine % 4 == 0) || (indexLine % 4 == 1)){
                        bitmap = 0x00;
                    }
                    break;
                case DIRECTIVE_SLATS_THICK:  //Make every 4th, fifth, and sixth line a string of black pixels instead of the right data.
                    if (indexLine % 4 != 3){
                        bitmap = 0x00;
                    }
                    break;
                case DIRECTIVE_BLACKOUT: //Ignore the font data, use black text instead.
                    bitmap = 0x00;
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
// uses a simple directives string to control character inversion
// expects an 8x12 font
// input: text      0-terminated string
//        directives 2 bytes of data.
//        line      vertical position of text, 0-(PIXEL_Y-1)
void printTextMedium(const char* text, unsigned char line, const char* directives)
{
    unsigned char character, bitmap, indexText, indexLineBuffer, indexLine, fontaddr, operation;
    char **addressed_font;
    //bool padded;
    //For simplicity, and because we are borrowing an existing font library, we print line-by-line
    indexLine = 0;
    while(indexLine < 12 && line < PIXELS_Y)             //loop for 12 char lines within display
    {
        indexText = 0;
        indexLineBuffer = 0;
        while(indexLineBuffer < (PIXELS_X/8) && (character = text[indexText]) != 0)  // We did not reach the end of the line or the string.
        {
            fontaddr = directives[indexText] >> 4 & 0xF;
            operation = directives[indexText] & 0xF;
            switch(fontaddr){
                case CASE_FONT_ADDR_0 :
                    addressed_font = &font8x12;
                    break;
                case CASE_FONT_ADDR_1 :
                    addressed_font = &font8x12_1;
                    break;
                case CASE_FONT_ADDR_2 :
                    addressed_font = &font8x12_2;
                    break;
                case CASE_FONT_ADDR_3 :
                    addressed_font = &font8x12_3;
                    break;
                case CASE_FONT_ADDR_4 :
                    addressed_font = &font8x12_4;
                    break;
                case CASE_FONT_ADDR_5 :
                    addressed_font = &font8x12_5;
                    break;
                case CASE_FONT_ADDR_6 :
                    addressed_font = &font8x12_6;
                    break;
                case CASE_FONT_ADDR_7 :
                    addressed_font = &font8x12_7;
                    break;
                case CASE_FONT_ADDR_8 :
                    addressed_font = &font8x12_8;
                    break;
                case CASE_FONT_ADDR_9 :
                    addressed_font = &font8x12_9;
                    break;
                case CASE_FONT_ADDR_A :
                    addressed_font = &font8x12_A;
                    break;
                case CASE_FONT_ADDR_B :
                    addressed_font = &font8x12_B;
                    break;
                case CASE_FONT_ADDR_C :
                    addressed_font = &font8x12_C;
                    break;
                case CASE_FONT_ADDR_D :
                    addressed_font = &font8x12_D;
                    break;
                case CASE_FONT_ADDR_E :
                    addressed_font = &font8x12_E;
                    break;
                case CASE_FONT_ADDR_F :
                    addressed_font = &font8x12_F;
                    break;
                default :
                    addressed_font = &font8x12;
                    break;
            }
            bitmap = addressed_font[character][indexLine];  // Retrieves the byte defining one line of character.
            switch (operation){
                case DIRECTIVE_NORMAL : // Print as-is
                    break;
                case DIRECTIVE_REVERSED : // Reverse Character Orientation
                    bitmap = reverse(bitmap);
                    break;
                case DIRECTIVE_NEGATIVE: // Invert the character
                    bitmap = ~bitmap;
                    break;
                case DIRECTIVE_REVERSED_NEGATIVE: //Reverse and Invert. Don't see a use, but I'll want it if I don't create it.
                    bitmap = ~reverse(bitmap);
                    break;
                case DIRECTIVE_SLATS_THIN:  //Make every 4th line a string of black pixels instead of the right data.
                    if (indexLine % 4 == 0){
                        bitmap = 0x00;
                    }
                    break;
                case DIRECTIVE_SLATS_MED:  //Make every 4th and 5th line a string of black pixels instead of the right data.
                    if ((indexLine % 4 == 0) || (indexLine % 4 == 1)){
                        bitmap = 0x00;

                    }
                    break;
                case DIRECTIVE_SLATS_THICK:  //Make every 4th, fifth, and sixth line a string of black pixels instead of the right data.
                    if (indexLine % 4 != 3){
                        bitmap = 0x00;
                    }
                    break;
                case DIRECTIVE_BLACKOUT: //Ignore the font data, use black text instead.
                    bitmap = 0x00;
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
void printTextLarge(const char* text, unsigned char line, const char* directives)
{
    unsigned char character, bitmap_left, bitmap_right, indexText, indexLineBuffer, indexLine, indexOfOffset, bitmap_left_out, bitmap_right_out, fontaddr, operation;
    char **addressed_font;
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
            fontaddr = directives[indexText] >> 4 & 0xF;
            operation = directives[indexText] & 0xF;
            switch(fontaddr){
                case CASE_FONT_ADDR_0 :
                    addressed_font = &font16x16;
                    break;
                case CASE_FONT_ADDR_1 :
                    addressed_font = &font16x16_1;
                    break;
                case CASE_FONT_ADDR_2 :
                    addressed_font = &font16x16_2;
                    break;
                case CASE_FONT_ADDR_3 :
                    addressed_font = &font16x16_3;
                    break;
                case CASE_FONT_ADDR_4 :
                    addressed_font = &font16x16_4;
                    break;
                case CASE_FONT_ADDR_5 :
                    addressed_font = &font16x16_5;
                    break;
                case CASE_FONT_ADDR_6 :
                    addressed_font = &font16x16_6;
                    break;
                case CASE_FONT_ADDR_7 :
                    addressed_font = &font16x16_7;
                    break;
                case CASE_FONT_ADDR_8 :
                    addressed_font = &font16x16_8;
                    break;
                case CASE_FONT_ADDR_9 :
                    addressed_font = &font16x16_9;
                    break;
                case CASE_FONT_ADDR_A :
                    addressed_font = &font16x16_A;
                    break;
                case CASE_FONT_ADDR_B :
                    addressed_font = &font16x16_B;
                    break;
                case CASE_FONT_ADDR_C :
                    addressed_font = &font16x16_C;
                    break;
                case CASE_FONT_ADDR_D :
                    addressed_font = &font16x16_D;
                    break;
                case CASE_FONT_ADDR_E :
                    addressed_font = &font16x16_E;
                    break;
                case CASE_FONT_ADDR_F :
                    addressed_font = &font16x16_F;
                    break;
                default :
                    addressed_font = &font16x16; //TODO <- this is most likely the fuckery
                    break;
            }
            bitmap_right = addressed_font[character][indexLine+indexOfOffset];  // Retrieves the byte defining the left side of the character.
            bitmap_left = addressed_font[character][indexLine+indexOfOffset+1];  // Retrieves the byte defining the right side of the character.
            switch (operation){
                case DIRECTIVE_NORMAL : // Print as-is
                    bitmap_right_out = bitmap_right;
                    bitmap_left_out = bitmap_left;
                    break;
                case DIRECTIVE_REVERSED : // Reverse Character Orientation
                    bitmap_left_out = reverse(bitmap_right); //The orientation of the two segments must also reverse, not just the bits themselves.
                    bitmap_right_out = reverse(bitmap_left);
                    break;
                case DIRECTIVE_NEGATIVE: // Invert the character
                    bitmap_left_out = ~bitmap_left;
                    bitmap_right_out = ~bitmap_right;
                    break;
                case DIRECTIVE_REVERSED_NEGATIVE: //Reverse and Invert. Don't see a use, but I'll want it if I don't create it.
                    bitmap_left_out = ~reverse(bitmap_right);
                    bitmap_right_out = ~reverse(bitmap_left);
                    break;
                //TODO: Once the logic for the slats checks out, export to the other print funcs.
                //TODO resumehere: This is promotable
                case DIRECTIVE_SLATS_THIN:  //Make every 4th line a string of black pixels instead of the right data.
                    if (indexLine % 4 == 0){
                        bitmap_left_out = 0x00;
                        bitmap_right_out = 0x00;
                    }
                    else {
                        bitmap_right_out = bitmap_right;
                        bitmap_left_out = bitmap_left;
                    }
                    break;
                case DIRECTIVE_SLATS_MED:  //Make every 4th and 5th line a string of black pixels instead of the right data.
                    if ((indexLine % 4 == 0) || (indexLine % 4 == 1)){
                        bitmap_left_out = 0x00;
                        bitmap_right_out = 0x00;
                    }
                    else {
                        bitmap_right_out = bitmap_right;
                        bitmap_left_out = bitmap_left;
                    }
                    break;
                case DIRECTIVE_SLATS_THICK:  //Make every 4th, fifth, and sixth line a string of black pixels instead of the right data.
                    if (indexLine % 4 != 3){
                        bitmap_left_out = 0x00;
                        bitmap_right_out = 0x00;
                    }
                    else {
                        bitmap_right_out = bitmap_right;
                        bitmap_left_out = bitmap_left;
                    }
                    break;
                case DIRECTIVE_BLACKOUT: //Ignore the font data, use black text instead.
                    bitmap_left_out = 0x00;
                    bitmap_right_out = 0x00;
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
// FUTURE: Refactor splash-bitmap and remove the reverse function for better performance.
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

void printDeltas_universal(DisplayFrameNew* incoming_frame, SceneDefinition* scene_rules){
    int index; int line_changed; int directives_changed; 
    char current_string[PIXELS_X/FONT_SIZE_FLOOR_X]; int previous_string[PIXELS_X/FONT_SIZE_FLOOR_X];
    const int len = PIXELS_X/FONT_SIZE_FLOOR_X;
    for (index = 0; index < scene_rules->lines_used; index++){

        strcpy(current_string, incoming_frame->frame[index].line);
        strcpy(previous_string, PREVIOUS_FRAME.frame[index].line);
        line_changed = memcmp(current_string, previous_string, len);
        strcpy(current_string, incoming_frame->frame[index].directives);
        strcpy(previous_string, PREVIOUS_FRAME.frame[index].directives);
        directives_changed = memcmp(current_string, previous_string, len);

        if (line_changed || directives_changed || FORCE_REFRESH){
            switch (scene_rules->rows[index].text_size){
                case TEXT_SIZE_SMALL :
                    printTextSmall(incoming_frame->frame[index].line, scene_rules->rows[index].line_address, incoming_frame->frame[index].directives);
                    break;
                case TEXT_SIZE_MEDIUM :
                    printTextMedium(incoming_frame->frame[index].line, scene_rules->rows[index].line_address, incoming_frame->frame[index].directives);
                    break;
                case TEXT_SIZE_LARGE :
                    printTextLarge(incoming_frame->frame[index].line, scene_rules->rows[index].line_address, incoming_frame->frame[index].directives);
                    break;
            }
        }
    }
    PREVIOUS_FRAME = *(incoming_frame);
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


//Determine which mode of changes-only screen update to use, and drop the incoming frame of output to that function.
//Expects a pointer to a DisplayFrameNew compatible with the outgoing mode (and the scene that constructed it) and calls the correct
//printDeltas_XXXXX function depending on the value of MODE. For convenience any new modes added should be given descriptive
//symbolic names in display.h
void DISPLAY_updatesOnly_enhanced(DisplayFrameNew *incoming_frame, unsigned int mode){
    switch (mode){
        case MODE_GAME :
            printDeltas_universal(incoming_frame, &SCENEDEF_game_mode);
            break;
        case MODE_MENU :
            printDeltas_universal(incoming_frame, &SCENEDEF_menu_mode);
            break;
        case MODE_MINIGAME :
            printDeltas_universal(incoming_frame, &SCENEDEF_minigames);
            break;
    }
    FORCE_REFRESH = false;
}



//Convenience function for breaking out multidigit numbers for display. Automatically elevates to the correct character code (subtract '0' if you need the number):
//Anywhere this method is not being used should be replaced as noticed unless there is a special-handling reason not to do so.
char DISPLAY_nthDigit(int digit_index_from_least, int full_value){
    while (digit_index_from_least--){
        full_value /= 10;
    }
    return full_value%10 + '0';
}

//convenience function for simply turning all the lines in a scene blank
void DISPLAY_blankFrame(void){
    int row, col;
    for (row = 0; row<(PIXELS_Y/FONT_SIZE_FLOOR_Y); row++){
        for (col = 0; col<((PIXELS_X/FONT_SIZE_FLOOR_X)-1); col++){
            DISPLAY_FRAME.frame[row].line[col] = ' ';
            DISPLAY_FRAME.frame[row].directives[col] = FONT_ADDR_0 + DIRECTIVE_NORMAL;
        }
        DISPLAY_FRAME.frame[row].line[(PIXELS_X/FONT_SIZE_FLOOR_X)] = 0;
    }

}
