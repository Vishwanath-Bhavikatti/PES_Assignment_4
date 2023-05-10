/*
*********************************************************************************************************************
​ *
​ ​*​ ​@author​ :​Vishwanath Bhavikatti
 * @credits: KL25Z reference manual and PES lecture slides for GPIO Interrupt initialization
​ ​*​ ​@date​ :March 6, 2023
 * @file name : gpio.c
 * @file_description: This file does gpio interrupt initialization
 *
*************************************************************************************************************************
*/

#include "MKL25Z4.h"
#include <stdio.h>

#define MASK(x) (1UL << (x))  // Masking bits
#define GPIO_INT_PIN 3     //GPIO_Interrupt Pin

/*
 * @Function
 *     This function is GPIO PORTD_Initialization for GPIO Interrupt
 *     uses PORTD PIN 3 and falling edge mode ( Connect PORTD PIN 3 to GND to generate an Interrupt)
 * @Parametes: None
 * @Returns : None
 */

void gpio_init(void)
{
	//Enable clock to PORT D
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

    //select pin MUX mode to GPIO by setting the MUX field to PORTD_PCR3 to 001

    PORTD->PCR[GPIO_INT_PIN] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[GPIO_INT_PIN] |= PORT_PCR_MUX(1);

    // Configure Port D Pin 3 as GPIO input with pull-up resistor and interrupt enabled on falling edge (0a)
    PORTD->PCR[GPIO_INT_PIN] |= PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0a);


    //Define appropriate port bits to be Input.

    PTD->PDDR |= ~MASK(GPIO_INT_PIN);
    // Enable Port D interrupt in NVIC
    NVIC_EnableIRQ(PORTD_IRQn);


}


