# Documentation - Main.c
## Introduction
The main.c file is, by divine fiat, the home of the main() loop, and therefore the beating heart (if you like) of the entire PETI design. As such, changes to main.c should be made only with careful consideration.

Accordingly, between the time of writing (POC release) and the next feature release, it will likely be entirely rewritten. The current implementation of the main loop is clunky and can lead to race conditions if not handled in a more appropriate way. Because of this consideration, main() itself is not documented here beyond what is available in the code comments themselves. As well, several functions (those not documented here) will likely also be removed, as they are needed only for the POC and not for the game itself.

However, several other functions currently defined in main.c will be sticking around, so they are documented herein.

## Includes
There are currently three includes:
- `driverlib.h`, which is TI's MSP430 driverlib library of device drivers. This allows for functional programming of the device through an API rather than direct register manipulation for ease of code design. This is not included in the repo and must be obtained seperately from TI.
- `lib/display/display.h` which is the display driver header included in this repo.
- `msp430.h` which is the general header file for the msp430 family of devices, provided by TI as part of their Code Composer Studio IDE, which is what is being used for development at present. Allows simplistic addressing of individual registers within the device and provides some helper pointers like `BIT0`.

## Functions Defined

### Init_GPIO(void)
This function initiates the GPIO pins on the device for their various purposes and should be expanded to include all pins on all GPIO ports; this is necessary to avoid phantom interrupts, which is part of why we keep winding up in the ISR trap.

Currently, it initializes the following pins in the corresponding ways:
- P1.0 as an output LED, which is currently tied to the status of the VCOM bit (see display.md).
- P1.1 as an output LED, which is used for debugging and currently indicates if the number of times a button has been pressed is odd or even, with odd being the lit value.
- P6.0, P7.1, P5.7, and P8.3 as input pins with interrupts and pullup resistors enabled for handling the four buttons on the buttons daughterboard.
- A commented-out block sets all the pins on Port 3 as output pins for driving debugging LEDs. These can be used individually or to drop the full value of a byte into their output register, to display that value mechanically.

### Init_Timers(void)
This function initiates a few timer registers AND sets the system clock to run at 1 mHz. It is written in such a way that it also enables Timer A0 and sets it up to trigger every 1000 cycles, or roughly once per milisecond.

This logic is likely to change; the controller can run up to three times faster than the present setting and raising an interrupt once per milisecond is suboptimal.

## Major Variables
- `VCOM`, an unsigned char that controls the VCOM state (described in detail in `display.md`.
- `VERSION`, a char array/string that describes the version number. This is printed during boot.
- `bufferText` a 17-member character array used for printing text.
- `buttonsBar` a 9 member character array used for printing the button state specifically.
- `buttons_state`, an unsigned int treated as a byte of bitwise flags for treating the button state. Each button has two bits in the flag:
 - `button_X_pressed`, which indicates the ISR for the button was triggered since the last time the state was updated, and;
 - `button_X_toggle`, which is toggled each time a button is processed.

## Interrupts Defined
All interrupts should be defined in Main.C for clarity.

### VCOM_ISR
This ISR is raised by the `TIMER_A0_VECTOR` and therefore triggers at roughly 1 kHz. As it is raised, a series of integers are incremented, calculating the time passed since reset, up to a total of 59:59:59.999. This also toggles the VCOM byte between enabled or disabled. See `display.md` for an explanation of the use of the VCOM byte.

Doing this much logic inside and ISR is inefficient and likely part of the issue with screen refresh rate. As the timer refactor progresses, it's likely this ISR will be refactored as well.

### BUTTON_ISRs
There are four button ISRs, labelled A through D, which functionally work the same way: clearing the interrupt flag for their port, and toggling a bit in `buttons_state` to indicate that the button in question was pressed.
