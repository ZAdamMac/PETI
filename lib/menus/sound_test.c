/*
 * sound_test.c
 *
 *  Created on: Jan 31, 2022
 *      Author: patches
 */

#include "lib/scenes/scenes_manager.h"
#include "lib/scenes/menu_generator.h"
#include "lib/alerts/audio.h"


void MENU_SOUNDCHECK_short_pulse(void){
    AUDIO_pulse(AUDIO_SHORT_PULSE);
}

void MENU_SOUNDCHECK_long_pulse(void){
    AUDIO_pulse(AUDIO_LONG_PULSE);
}

#pragma PERSISTENT(MENU_SOUNDCHECK_count_options);
const int MENU_SOUNDCHECK_count_options = 2;

#pragma PERSISTENT(MENU_SOUNDCHECK_count_options);
voidFuncPointer MENU_SOUNDCHECK_functions[2] = {
                                      MENU_SOUNDCHECK_short_pulse,
                                      MENU_SOUNDCHECK_long_pulse
};
