/*
 * hwinit.h
 *
 *  Created on: May 10, 2021
 *      Author: patches
 */

#ifndef LIB_HWINIT_HWINIT_H_
#define LIB_HWINIT_HWINIT_H_

/*
 * The following lines define several symbols that are used to configure hardware pins.
 * //FUTURE: Standardize this practice.
*/

#define LED_ALERT_PORT GPIO_PORT_P4
#define LED_ALERT_PIN  GPIO_PIN7
#define LED_BATTERY_PORT GPIO_PORT_P1
#define LED_BATTERY_PIN GPIO_PIN5
#define SENSOR_BATTERY_PORT GPIO_PORT_P3
#define SENSOR_BATTERY_PIN  GPIO_PIN5

void Init_GPIO(void);
void Init_Timers(void);
void Init_RTC(void);
void Init_Watchdog(void);

#endif /* LIB_HWINIT_HWINIT_H_ */
