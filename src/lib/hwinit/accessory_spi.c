//******************************************************************************
//  PETI FOSS Virtual Pet - Accessory SPI Feature
//
//  Implements primitives and API used for the Accessory SPI Featureset.
//
//  Zac Adam-MacEwen (Arcana Labs)
//  March 2024
//******************************************************************************

#include <msp430.h>
#include "driverlib/MSP430FR5xx_6xx/driverlib.h"
#include "lib/hwinit/accessory_spi.h"

// A very simple function to handle sending a single byte somewhere.
// Please don't build serious functionality based on this; it is likely
// going to be replaced wth something much more serious and proper.
void ACCESSORY_spiSendByte(char byte){
    GPIO_toggleOutputOnPin(ACCESSORY_PIN_PORT, ACCESSORY_PIN_PIN);
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, byte);
    GPIO_toggleOutputOnPin(ACCESSORY_PIN_PORT, ACCESSORY_PIN_PIN);
}