# v 0.0.4 - Menu Generator A
- added generation code for the main_game.c scene:
 - controlled by arrays in lib/menus/main_game.h
 - code will automatically divide the available icons between the top and bottom
 - a matching array in same file controls the destination scenes.
- added metanimation for the baby state
- hatching egg now drops to the baby life stage.
 - The old demo mode is accessible through the bug-shaped icon (DEBUG).
 - This involved adding some hacks to game_manager.c that need to be removed when the evolution function is implemented.
- Refactored the game manager to make the egg stage's length more easily modified.
 - Temporarily lowered this to the debugging value of 1 minute, which is the minimum non-zero value.
- Updated the default date to today (2021-10-29).
- Refactored several functions to use the printTextMedium_enhanced fuction, which supports directives for highlighting/cell inversion.
- Adds a scene (buttonproofer.c) that simply displays a message indicating the button you hit worked. Used for testing. Should be removed prior to a major release.
- improvements in commenting of code.

# v 0.0.5 - Pet Status Screen and Text-Oriented Menu Generator
- Added new characters to the font
 - includes altering the set button sprite slightly.
- Added scenes/status_menu, which is bounded to the status icon.
 - Displays basic health information about the pet:
  - Hunger
  - Fun
  - Weight
  - Age
  - Special Conditions
- Added new values to the enCA_strings locale files.
- Added a new scene, `SCENE_TextMenu()`, via `menu_generator.c`. This new scene allows for the generation of arbitrary text based menus via several provided arguments, which exit based on a provided array of function pointers.
- Added a new scene based on the above that provides for a debug menu. Right now, it's bare bones, but the `lib/menus/debug_menu` c source and h header files should provide a good working example of the correct definition of a menu until that process is properly documented.


# v 0.0.5r2 - Pre-Six Refactor
- Fixed Bug #11 - Prior to now it was possible to softlock the game by exiting the initial calendar setup menu improperly.
- Updated status_menu.c such that the leading zeros from the age display are now removed.
- Changed the definitions of SCENEADDR address values to more closely indicate affiliation by grouping.
- Changed the names of some Localization arrays to better match the style of the others.
- Deprecated printTextLarge and printTextMedium
- Refactored PTL and PTM to remove the _enhanced tag from the function name
- Deprecated and outright removed bcd_to_dec; use the builtins instead.

# v 0.0.6 - Display System Rewrite
- Restructured the DisplayFrame struct such that it holds the arrays needed rather than pointers.
 - Also is now an iterable array of DisplayFrameLine structs that have one property each for the text to be displayed, directive.
- Modified all scenes that used the old display frame to use the new one.
- Deprected the former Display_UpdatesOnly(), replacing wiht a new version that uses the new frame.
- Deprecated all the former printDeltas function to replace with printDeltas_universal
 - new functon uses a new struct called a SceneDefinition that includes the necessary data to iteratively print a screen based solely on a mode flag and the incoming display frame.
- Scenes no longer compute their own refresh data. The current frame is compared to the previous frame and the display code determines whether or not to reprint any one given line.
- Display modes are now defined as a struct, allowing one printout function to be shared among all display arrangements.
- Continued removal of magic numbers particularly as tied directly to screen resolution. Several new symbols were added to the defines in display.h to help with this.
- Closed a bug in scenes/main_game.c that would cause the following odd behaviours:
 - Pressing the D (Cancel) button would trigger the select (c) behaviour;
 - The select (c) behaviour would illegally set SceneAct if the cursor position was 0, locking the device in a dead state until reset
 
# v 0.0.7 - Feeding System
- Adds a struct Food for defining foods in terms of their iconset, hunger, and fun values.
- Adds string arrays to the locales file that control the names of foods as displayed.
- Adds a global array of Food objects that define the list of foods included in the food or snacks subsets.
- Adds a menu generator for reading food arrays instead of normal menu definition files.
- Adds an eating scene (`eating_animation.c`) that accepts a food as its input, generating the appropriate animation before returning to the main game screen.
- Adds a function to the game manager to iteratively decay Hunger and Fun meters over time based on the rateHF for the current evolutionary stage.
- Sets a rate HF of `0xFF` to the Baby (`EVO_metaStruct[1]`).
- Adds a function to the eating animation scene that applies the appropriate status modifiers from the food (TODO: Weight still not implemented)
- Corrects a bug in status_menu.c that caused graphical issues due to a failure to reset the DIRECTIVES for a given line.

# v 0.1.0 - Alert and RNG Primitives
- Adds the alert primatives allowing simplistic LED and audio alerts if the backplane is installed (finalized back plane design not complete)
- makes changes to GPIO init in order to suppert use of the buzzer and LED
- Enables a debug menu and all debug functionality if switch 2.2 is closed
  - This also bypasses the "egg" stage of life automatically.
- Yet another display refactor: the medium and large print functions now support 16 fonts each and have the potential to allow you to define up to 16 "display modes" per character. No new fonts or display modifiers were added.
- Adds the primitives for RNG functions based on reading the TLV and using a LCG-type non-secure Random Number Generator in order to provide "game-quality" entropy for future use in, say, a minigames mode.
- Adds a scene to the debug menu that allows you to babysit the RNG state.
- It's intended to "break" here to catch up with documentation and finish the development kit design, thus the 0.1.0 numbering.

# v 0.2.0 - Minigame Prototype
- Adds a basic "Rock Paper Scissors" Minigame to serve as a prototype for the minigames system and as a base minigame for testing purposes. Future work will refine this minigame as other relied-upon work is completed first.
- Corrections have been made to the RNG to make it more reliably unpredictable to the average human player.
- Species size magic numbers have been replaced with defined symbols in EVO_data as part of the overall work to eliminate magic numbers.
- GAME_applyHungerFun was added to game_manager to provide a convenience function for applying signed-integer changes to the HUNGER_FUN score without needing to redefine the guard rails and bitwise operations every time.
  - Some older functions were refactored to use this.
- The input handler was replaced with a more performant input queue system that will no longer drop inputs when multiple inputs are generated between calls to the "evaluate input" function in any given scene or scenes.
  - All existing scenes were refactored to support this new system. The change is especially noticable in menus.
- Localization strings were added to enCA_strings to support the changes above.
- The default date was moved to a date closer to the release of 0.2.0.

# v 0.3.0 - Power Management Updates
- StateMachine - and thus the user progress data, are now persisted to FRAM.
  - Saves will persist between loss of power or BOR.
  - On BOR, user is prompted to confirm whether to clear the data or not using a localizeable warning.
- Fixed a small logic bug that was increasing frame render times.
- The battery LED and alert LED can now blink at one of two rates. This uses less power than leaving them on continuously.
- Implemented DISPLAY_sleepLCD and DISPLAY_wakeLCD to turn on and off the display respectively. A new state integer is being used to determine if it is necessary to run the display update code. A full refresh is called when waking the display.
  - Implemented a debug callable to sleep the LCD.
  - Alerts currently trigger when the display is asleep and will wake the LCD.
- Adds a new function `HID_dumpQueue` which blindly empties the input queue without handling it. This is useful in a variety of cases.
  - Currently called when waking the screen from sleep mode.
- Adds Low Battery Detection near start of the main gameplay loop. Sets the global flag `BATTERY_LOW` from battery.h to a true-ish value if the battery has been detected to be low.
  - This exposed issue #35 in the hardware, which needs to be corrected.
  - When detected, the low battery will blink the LED.
  - Adds support for the LBO sensor to HWINIT
  - Adds crude support for showing the Low Battery Icon on the main game screen.
- Corrects a longstanding issue that was causing full screen refreshes instead of the promised by-line refreshes. Should help with display-driven power usage.

# v 0.4.0 - Sleep and Growth Update
- Adds functionality that allows the pet to fall asleep between wall-clock times defined by their age group through `game_manager.h`
  - Sleeping pets do not have the game state recompute while sleeping and accordingly do not experience stat depletion.
  - Hunger and Fun degradation now occur retroactively when the pet wakes.
  - The main game screen displays a status icon and special pet animations when the pet is sleeping.
    - `evo_data.h`'s `struct Stage` was ammended to add a `animationSleeping` character pointer that holds sleeping animations. This shares the limitation with all other animation data that font addressing is not currently implemented at this level.
    - Minor font changes for `font16x16` to correct the misalignment of zazenkuchi's sleeping head from his body.
  - The Food and Minigame menus are not accessible when the pet is sleeping, as is appropriate.
    - `menus/main_game.c/h` were refactored to use a function-derived model similar to menu_generator in order to achieve this.
- Rearranges the main game idle screen's menu to be more logically accessible to the player in terms of icon order.
- Adds an icon to the main game menu that allows the pet to be put to bed by turning the lights off.
  - A pet that has freshly woken up will turn the lights back on.
- Fixes unreported bug that caused odd-numbered main menu menus to render the cursor on the bottom row one square earlier than it should.