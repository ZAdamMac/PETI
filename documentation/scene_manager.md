# Scenes System and scene_manager.c/h Documentation
PETI is fundamentally built around a utility called `scene_manager`, defined in the source and header files bearing that name under `lib/scenes`. While there are other functions that are run on each operating cycle prior to the code that handles scenes, it is scene_manager and the other scene types that determine what is presented to the user on the device display. It is helped in this task by all the other files in the `lib/scenes` directory, known properly as Scenes.

## Intended Operation Flow of the Scene System
Once per operating cycle of the main hardware loop, the function `SCENE_updateDisplay` is called. This function reads a global variable called `SCENE_ACT` in order to determine the appropriate function to call in order to draw the next frame of animation from the scene.

Fundamentally, there are two "kinds" of scenes - scenes proper, and textual menus. Textual menus are a special-case scene type that uses the SCENE_TextMenu() function provided by `lib/scenes/menu_generator.c`. All other kinds of scenes, including menus that require special handling, qualify as ordinary scenes.

Scenes are constructed such that they (mostly) have the following commonalities:
 - A call to `DISPLAY_updatesOnly_enhanced()` passing in the `DISPLAY_FRAME` pointer and an appropriate display mode, which draws a frame of animation upon the screen.
 - A way to track which frame they are on as a matter of state.
 - One or more exit conditions that change the value of SCENE_ACT and (if needed) reset any cursor positions or frame counts to 0.
 - If needed, input handler code that handles the `buttons_state` object and uses the information contained within it to apply any needed state changes relevant entirely to that screen, such as incrementing cursor values or applying an exit condition.
 
Apart from these conditions, a scene can fundamentally be anything. `main_game.c` provides a scene that plays the main idle animation of the pet and renders the main game menu dynamically, as well as allowing you to interact with that menu. `calendar_menu.c` is a special purpose menu that allows you an interface to set the on-chip RTC.

### Adding a Scene to the Game
There are fundamentally only a few steps needed to add a scene to the game.
0. Write your scene and ensure that it has a primary runtime function, usually `SCENE_mynewScene()`.
1. Include the header file for the scene into scene_manager.c
2. In scene_manager.h, add a `#DEFINE` statement to define an address for your new SCENE.
3. In scene_manager.c, add a new case to the switch statement in SCENE_updateDisplay() such that the new scene address calls the new function set up in step 0.
4. Elsewhere in the game, set up soem condition that would cause SCENE_ACT to be set to your new scene address. In most cases, this means adding your scene address as the target in a menu.

### Adding Textual Menus to the Game
The menu generator provided in `menu_generator.c` is a convenience function allowing us to much more easily define text-based menus than we normally would if each needed to be a unique scene. It provides a function, `SCENE_TextMenu`, that accepts arguments from the switch statement in `SCENE_updateDisplay`. This makes adding new text-based menus much simpler, though steps 1, 2, 3, and 4 from the above step must still be completed afterward.

Step 0 is instead replaced with the following directives, which can be performed in any order:
- add new c and h files to `/lib/menus` for your new menu. They should define (and expose) the following variables:
 - functions for each option in the menu.
 - a constant which specifies the length (in items) of the menu, and
 - an array of voidFuncPointer objects that point to those functions in the order they will be displayed.
- Access your locale file (and, at minimum, enCA), and add an `LARRAY` variable listing strings for the names of each menu option, in the order they were provided to func pointer.

When performing step 3, make sure to include the appropriate functions as called to the `SCENE_TextMenu` function.

These terms are targeted more directly in the documentation for menu_generator.c

## Functions
### SCENE_updateDisplay(void)
This function reads the value of the global variable SCENE_ACT and performs the following actions:
- Determines if a `FORCE_REFRESH` flag needs to be set, and sets it;
- SETS the current `PREVIOUS_SCENE` to the same value as `SCENE_ACT`, and;
- Calls an appropriate SCENE function to compute, render, and display one frame of animation from the appropriate SCENE given the current value.

Since calling this function automatically causes a write to the display this should only be used in the main game loop and should not be called by subordinate pieces of the game code.

### SCENE_setTransitionTimeCondition(unsigned int delay_in_seconds);
Set a delay (up to an implemented limit of 60 seconds) to be used later to determine if a transition can occur. Currently this is used by game manager as part of a check to determine if we can hath the egg.

Mechanically, this works by setting some global vars, `SCENE_TRANS_SECONDS` and `SCENE_TRANS_MINUTES`. These can later be interrogated together with the RTC to determine if we've passed the transition time.

### SCENE_checkTransitionTimeCondition()
Interrogates the transition time condition variables `SCENE_TRANS_SECONDS` and `SCENE_TRANS_MINUTES`. If the current RTC time is equal to or later than those two values (ignoring the state of the hour), this function returns true. Else, it returns false.
