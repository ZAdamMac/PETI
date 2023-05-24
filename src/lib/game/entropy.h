/*
 * entropy.h
 *
 *  Created on: August 24, 2022
 *      Author: patches
 */

#ifndef LIB_GAME_ENTROPY_H_
#define LIB_GAME_ENTROPY_H_

<<<<<<< HEAD
unsigned int RNG_current_state;  // The current state of the RNG. Is moved every time the RNG is called.
int RNG_session_seed;   // The initial seed. Doesn't need to be preserved per se, but we hold onto it for debugging reasons.

unsigned int RNG_getSeedWord(void); // Get an initial seed float for the RNG by selecting a random word out of the TLV using the ADC.
void RNG_initialize(int initial_seed); // Set the argument initial_seed as the RNG_current_state after passing it through the RNG at least once.
=======
float RNG_current_state;  // The current state of the RNG. Is moved every time the RNG is called.
float RNG_session_seed;   // The initial seed. Doesn't need to be preserved per se, but we hold onto it for debugging reasons.

float RNG_getSeedWord(void); // Get an initial seed float for the RNG by selecting a random word out of the TLV using the ADC.
void RNG_initialize(float initial_seed); // Set the argument initial_seed as the RNG_current_state after passing it through the RNG at least once.
>>>>>>> d186b0b8c57b0138b7fc00e218d48372f5e69a84
void RNG_forceShuffle(void); // Increment the RNG without returning anything. Called at odd intervals to increase the unpredictability for the player.
float RNG_drawFloat(void); // Increment the RNG and return the result normalled into the range between 0 and 1.

#endif /* LIB_GAME_ENTROPY_H_ */
