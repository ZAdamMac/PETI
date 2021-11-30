# Display.c and Display.h Documentation
PETI relies on a broadly-custom driver file for the Sharp LS013B7DH03 Memory LCD, included in main via `display.h`. This driver handles both the low-level communication with the display via SPI, as well as the higher-level abstractions that the scenes themselves rely on to set up display state.

## Includes
`display.h` is included in any module that needs to communicate with the display in some way. It has a circular relationship with `display.c`, which both includes `display.h` and is included in the same file. This allows `display.h` to be used to control any necessary hardcoded values and expose functions to `main.c` while keeping the function definitions themselves in `display.c`. `display.h` specifies a variety of important variables. It should be included whenever access to the DisplayFrame object, FORCE_REFRESH flag, or scaling of the screen is needed.

Display.c further includes:
- `font.h`, a custom header file that includes all three fonts used in PETI, discussed in more detail in the `fonts.md` documentation file.
- `driverlib.h`, which is TI's MSP430 driver library and must be obtained from them directly.
- `msp430.h`, which is TI's MSP430 master header file, included in Code Composer Studio, and;
- `splash.h`, an H file containing the splash logo (see below) as a byte array.
- `scene_definitions.h`, which contains `SceneDefinition` structs that inform the composition of screens.

## Intended Usage

As a text-based system, Display exposes the individually-callable print functions, `printTextSmall`, `printTextMedium`, and `printTextLarge`, but it is not intended that any user-added scenes should call those functions directly. Instead, preference should be paid to the use of the globally-instantiated `DISPLAY_FRAME` object, an instance of the `DisplayFrameNew` struct.

This structure includes an array property, `frame`, which are each themselves `DisplayLine` structs consisting of two members, `line` and `directives`. By default, those members are `char` arrays of length 17, and `frame` has length 16. These values are proportional to the values `PIXELS_X` and `FONT_SIZE_FLOOR_X`, and `PIXELS_Y` and `FONT_SIZE_FLOOR_Y`, respectively. Note that the final char in line and directives should always be 0, and that when using large text they should instead be treated as though they have a length of 9, with a 0 in the ninth position.

The intended usage is that a scene should modify the *existing* DISPLAY_FRAME instance by use of `strcpy` to individual target members, defining the full screen it is required to use in this way.

Finally, a call should be made to `DISPLAY_updatesOnly_enhanced(&DISPLAY_FRAME, relevant_mode)`, with relevant_mode being the appropriate symbol for a display mode definition such as MODE_GAME or MODE_MENU.

The display system will then compute the appropriate print functions to send, based on the directive and line data for each DisplayFrameLine in `DISPLAY_FRAME->frame`, comparing to the *previous* frame, and sent printText$Size functions for only those lines which need to be redrawn on this frame.

***NOTE:*** A limited number of legacy scenes such as demo_mode.c use the older DisplayFrame struct together with `DISPLAY_updatesOnly()`. This method is extremely inefficient from both a coding and execution perspective and should be considered deprecated. They are likely to be removed as early as version 0.0.7.

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

### printTextMedium(const char* text, unsigned char line, const char* directives)
Calculates and prints up to one line of text, truncating or padding based on the screen width as divided into 8-pixel-wide character cells. Leverages font8x12 from the `font.h` header file. Requires a directives string of `'0'`s and `'1'`s, with 0 displaying the character in that position normally, and 1 inverting the colors displayed.

### printTextLarge(const char* text, unsigned char line, const char* directives)
Calculates and prints up to one line of text, truncating based on the screen width as divided into 16-pixel-wide character sells. Leverages font16x16 from the `font.h` header file. Requires a directives string using the following characters:
- `'0'` - Display as-is
- `'1'` - Mirror the character.
- `'2'` - Reverse the colors of the character
- `'3'` - Both mirror and invert the colors of the character.

### DisplaySplash(void)
Computes a full screen write based on the `splash.h` image (specifically, `splash_bitmap`). Currently relies on `reverse()` which is inefficient, and will need to be refactored.

### ToggleVCOM(void)
This convenience function reads the current state of VCOM and sends the Static Mode signal to the display along with the VCOM bit. This tells the display to switch between VCOM high and VCOM low states. **It is necessary to call this function at least once in thirty seconds, ideally roughly once per second** to avoid getting the display into a "stuck" condition, as the current hardware implementation does not allow for a clock-driven VCOM state.

### printDeltas_game, printDeltas_demo, and printDeltas_menu
These functions rely on the old DisplayFrame struct type and are considered deprecated. They will be removed in a near-future release.

### printDeltas_universal(DisplayFrameNew* incoming_frame, SceneDefinition* scene_rules)
A universal print_delta function, made possible by the new SceneDefinition struct type. This function expects a pointer to a DisplayFrameNew (usually the global `DISPLAY_FRAME`), and a relevant SceneDefinition struct. It will iterate over the SceneDefinition's `rows` array, using the defined length `lines_used` to further iterate over DISPLAY_FRAME, comparing it to `PREVIOUS_FRAME`. If a difference between the two frames is detected, `scene_rules` is consulted to determine the appropriate print function to use. Finally, the current DISPLAY_FRAME is assigned to PREVIOUS_FRAME to prepare for the next time the function is called.

In this way it is no longer necessary to have scenes manually calculate refreshes, nor is it necessary to define a new printDeltas function for each new screen layout.

### DISPLAY_updatesOnly
This function is deprecated and should not be called. It was effectively a switch statement to determine which of the deprecated printDeltas function was needed. After it was called, FORCE_REFRESH was set to false.

### DISPLAY_updatesOnly_enhanced(DisplayFrameNew *incoming_frame, unsigned int mode);
This function is a convenience wrapper for printDeltas_universal that translates between a MODE_* symbol and the appropriate SceneDefinition object by use of a switch statement, and handles setting FORCE_REFRESH to false as it exits.

### DISPLAY_nthDigit(int digit_index_from_least, int full value)
This is a convenience function for breaking out individual characters of a multi-digit integer, and translating them to the appropriate ascii character code. The current implementation relies on modular arithmetic and division and is somewhat slow.

## Major Variables and Globals
### defined in `display.h`
- `MLCD_WR` is a bitmask that if used appropriately corresponds to the "write line" command flag for the memory LCD.
- `MLCD_CM` is a bitmask that if used appropriately corresponds to the "clear memory" command flag for the memory LCD; this blanks the screen when executed by the display.
- `MLCD_SM` is a bitmask that if used appropriately corresponds to the "STATIC MODE" command flag for the memory LCD. This tells the display to enter static mode, which is a lower-power-consumption mode ideally used when the device is in standby. Can also be coupled with VCOM to set the VCOM state.
- `MLCD_VCOM` is a bitmask that if used appropriately corresponds to the "VCOM" command flag for the memory LCD. This controls the voltage orientation of the display. SHARP recommends toggling this flag at least once per second if VCOM will be controlled in software rather than hardware.
- `PIXELS_X` is the width of the display in pixels
- `PIXELS_Y` is the width of the display in pixels
- `FONT_SIZE_FLOOR_X` is the minimum width of all fonts in pixels.
- `FONT_SIZE_FLOOR_Y` is the minimum width of all fonts in pixels.
- `VCOM` is the current VCOM state and should be either `MLCD_VCOM` or `0x00` at all times.
- `bufferLine` a character array used to hold the bytes corresponding to each character for a given line on the display. Its size is tied to the value of `PIXELS_X`, expressed as an integer number of bytes.
- `DISPLAY_FRAME` is a globally-accessible instance of `DisplayFrameNew`, intended to be used by all modules that need to modify the screen output (such as scenes or menus) as a single shared object of screen state data.
- `WORK_STRING` is a `char` array of size PIXELS_X/FONT_SIZE_FLOOR_X`. This is necessarily one less than the length of the members of a `DisplayLine`, allowing easy use of `strcpy` without having to manage the terminating 0. It is globally available, with the intention being that it should be used by any function which will compute a line of text to be copied into a `line` or `directives` member of a `DisplayLine`. Added in v0.0.6 to avoid a stack poisoning condition in the calendar menu scene.
