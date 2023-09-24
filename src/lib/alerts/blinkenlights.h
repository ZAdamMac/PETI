/*
 * audio.h
 *
 *  Created on: Jan. 18, 2022
 *      Author: patches
 */



#ifndef LIB_ALERTS_BLINKENLIGHTS_H_
#define LIB_ALERTS_BLINKENLIGHTS_H_

#define BLINKENLIGHTS_SLOW_PULSES 0x4444
#define BLINKENLIGHTS_RAPID_PULSES 0x2222

extern unsigned int BLINKENLIGHTS_ALERT_LED_HOT;
extern unsigned int BLINKENLIGHTS_BATTERY_LED_HOT;

void BLINKENLIGHTS_blinkAlertLED(int pulse_length);
void BLINKENLIGHTS_stopAlertLED();
void BLINKENLIGHTS_blinkBatteryLED(int pulse_length);
void BLINKENLIGHTS_stopBatteryLED();

#endif /* LIB_ALERTS_BLINKENLIGHTS_H_ */
