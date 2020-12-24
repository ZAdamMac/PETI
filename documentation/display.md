# Display.c and Display.h Documentation
PETI relies on a broadly-custom driver file for the SHARP display, included in main via `display.h`. 

## Includes
`display.h` is included in `main.c`. It has a circular relationship with `display.c`, which both includes `display.h` and is included in the same file. This allows `display.h` to be used to control any necessary hardcoded values and expose functions to `main.c` while keeping the function definitions themselves in `display.c`.

Display.c further includes:
- `font.h`, a custom header file that includes all three fonts used in PETI, discussed in more detail in the `fonts.md` documentation file.
- `driverlib.h`, which is TI's MSP430 driver library and must be obtained from them directly.
- `msp430.h`, which is TI's MSP430 master header file, included in Code Composer Studio, and;
- `splash.h`, an H file containing the splash logo (see below) as a byte array.

## Functions

### Init_SPI(void)
Init_SPI is used to initiate both the GPIO pins that control SPI (P5.0, P5.1, and P5.2 - collectively as `EUSCI_B1`) as well as P1.3 that acts as the chip select signal for the LCD display. This function also sets up SPI at a speed of 1MHz.

### Init_LCD(void)
A convenience function to define the GPIO pins that provide power to the display itself as well as the LCD's controller: P6.2 and P1.2, respectively). This done, it also calls the function LCDClearDisplay.

### LCDClearDisplay(void)
A convenience function that calculates the correct message to completely blank the screen based on the current VCOM state, and sends it. Necessary on screen initialization but also convenient at other times.

### reverse(unsigned char byte)
Returns a byte after reversing the arrangement of its bits. Wherever possible reliance on this function should be removed as it is inefficient, but it is currenltly necessary to correctly display `splash.h`.

### SPIWriteLine(unsigned char line)
A convenience function to calculate the correct write-by-line command for the given VCOM state and then send a full line of display information to the display, byte-by-byte, before sending the termination signal. Relied upon by all higher-level display calls.

### printTextSmall(const char* text, unsigned char line)
Calculates and prints up to one line of text, truncating or padding based on the screen width as divided into 8-pixel-wide character cells. Leverages font8x8 from the `font.h` header file.

### printTextMedium(const char* text, unsigned char line)
Calculates and prints up to one line of text, truncating or padding based on the screen width as divided into 8-pixel-wide character sells. Leverages font8x12 from the `font.h` header file.

### printTextLarge(const char* text, unsigned char line)
Calculates and prints up to one line of text, truncating based on the screen width as divided into 16-pixel-wide character sells. Leverages font16x16 from the `font.h` header file.

### DisplaySlpash(void)
Computes a full screen write based on the `splash.h` image (specifically, `splash_bitmap`). Currently relies on `reverse()` which is inefficient, and will need to be refactored.

### ToggleVCOM(void)
This convenience function reads the current state of VCOM and sends the Static Mode signal to the display along with the VCOM bit. This tells the display to switch between VCOM high and VCOM low states. **It is necessary to call this function at least once in thirty seconds, ideally roughly once per second** to avoid getting the display into a "stuck" condition, as the current hardware implementation does not allow for a clock-driven VCOM state.

## Major Variables
### defined in `display.h`
- `MLCD_WR` is a bitmask that if used appropriately corresponds to the "write line" command flag for the memory LCD.
- `MLCD_CM` is a bitmask that if used appropriately corresponds to the "clear memory" command flag for the memory LCD; this blanks the screen when executed by the display.
- `MLCD_SM` is a bitmask that if used appropriately corresponds to the "STATIC MODE" command flag for the memory LCD. This tells the display to enter static mode, which is a lower-power-consumption mode ideally used when the device is in standby. Can also be coupled with VCOM to set the VCOM state.
- `MLCD_VCOM` is a bitmask that if used appropriately corresponds to the "VCOM" command flag for the memory LCD. This controls the voltage orientation of the display. SHARP recommends toggling this flag at least once per second if VCOM will be controlled in software rather than hardware.
- `PIXELS_X` is the width of the display in pixels
- `PIXELS_Y` is the width of the display in pixels
- `VCOM` is the current VCOM state and should be either `MLCD_VCOM` or `0x00` at all times.
- `bufferLine` a character array used to hold the bytes corresponding to each character for a given line on the display. Its size is tied to the value of `PIXELS_X`, expressed as an integer number of bytes.
