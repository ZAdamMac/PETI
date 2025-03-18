/*
 * accessory_spi.h
 *
 *  Created on: March 11, 2025
 *      Author: patches
 */

 #ifndef LIB_HWINIT_ACCESSORY_SPI_H
 #define LIB_HWINIT_ACCESSORY_SPI_H
 
 
#define ACCESSORY_PIN_PORT GPIO_PORT_P8
#define ACCESSORY_PIN_PIN GPIO_PIN0
 
void ACCESSORY_spiSendByte(char byte);
 
 #endif /* LIB_HWINIT_ACCESSORY_SPI_H */
 