# Documentation for the Hardware Initializer Library (hwinit.c)
Like any microcontroller project, we need to initialize hardware. Most of these functions are called exactly once before entering the main game loop in `main.c`, and can be thought of as subordinate functions of the bootloader. Through the functions of this library, we translate between abstracted game concepts (like "Button A") to precise physical realities (Pin P6.0).

This library file should really only be modified under one of the following two conditions:
- A port of the project from one microcontroller to another, or;
- The desire to update the default date for the system RTC, which should be done at least as often as every release which changes the version string. For convenience these are defined in hwint.c, near the top, begining with `DEFAULT DOW` on what is currently line 18.

This is not a full documentation of the hardware used in the PETI project. For that, refer to the documentation, schematics, and renders in the hardware_design directory.

## Functions
### Init_GPIO(void)
- sets P1.0 and P1.1 as outputs. These are LEDs built directly into the TI Launchpad.
- Sets the pins currently assigned to the A, B, C, and D buttons as input pins with internal pullup resistors, with interrupts enabled, triggered on the High-to-Low transition.

### Init_Timers(void)
Sets the system clock (SMCLK) to 8 Mhz and creates a recurring up-mode interrupt on TIMER_A0's CCR0 register. This timer is the "heartbeat" of the clock and the interrupt is handled in main.c; the resulting interrupt serves as a prompt to "kick" the device every half second (roughly) and render the next frame of animation.

### Init_RTC(void)
Sets the system RTC (RTC_C) to midnight on the date defined by the definition symbols DEFAULT_DOW, DEFAULT_DAY, DEFAULT_MONTH, and DEFAULT_YEAR, then sets the RTC running. This serves as a default time; there are user-initiated processes that can adjust the time at any point in the future.

### Init_Watchdog(void)
Disables the standard watchdog timer. In future this watchdog may be used in place of TIMER_A0CCR0's current heartbeat signal, or as a forced restart in the event of a system-hanging event, but it remains currently unused.
