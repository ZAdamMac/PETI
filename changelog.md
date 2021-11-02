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

# v 0.0.5 - Pet Status Screen
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
