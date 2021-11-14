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