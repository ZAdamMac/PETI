/*
 * audio.h
 *
 *  Created on: Jan. 18, 2022
 *      Author: patches
 */



#ifndef LIB_ALERTS_AUDIO_H_
#define LIB_ALERTS_AUDIO_H_

#define AUDIO_LONG_PULSE 0x4444
#define AUDIO_SHORT_PULSE 0x2222

void AUDIO_pulse(int pulse_length);

//Functional Boolean used to indicate whether or not it's time to stop the timers.
int AUDIO_noteEnd;

void AUDIO_stopNote(void);

#endif /* LIB_ALERTS_AUDIO_H_ */
