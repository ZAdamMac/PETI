//***************************************************************************************
//  PETI FOSS Virtual Pet - LED Alerting Primitives Code
//
//  Principal Code for TI MSP430FR5994 LaunchPad to Provide Alert LED Functionality
//
//  Check the hardware_design folder for VCS-controlled hardware schematics
//
//
//  Zac Adam-MacEwen (Arcana Labs)
//  Added January 2022
//***************************************************************************************

#include "driverlib.h"
#include <msp430.h>
#include "blinkenlights.h"


void BLINKENLIGHTS_raise(void){
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN7);
}
void BLINKENLIGHTS_lower(void){
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN7);
}
