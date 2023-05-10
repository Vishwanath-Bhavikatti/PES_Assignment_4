/*
*********************************************************************************************************************
​ *
​ ​*​ ​@author​ :​Vishwanath Bhavikatti
 * @Credits : https://github.com/alexander-g-dean/ESF/blob/master/NXP
​ ​*​ ​@date​ :March 5, 2023
 * @file name : tpm.c
 * @file_description: It consists of MKL25Z4.h a default header file of the KL25Z board.
 *                     Along with I have included core_cm0plus.h  whose function prototypes are being used in this file.
 * 					   This file includes PWM Initialization for RED, BLUE and GREEN colour
 *
*************************************************************************************************************************
*/
#include "MKL25Z4.h"
#include "core_cm0plus.h"
#include <stdio.h>

#define RED_LED_PIN 18 //RED LED Port Pin number
#define GREEN_LED_PIN 19		//GREEN LED Port Pin number
#define BLUE_LED_PIN 1		//BLUE LED Port Pin number
#define MASK(x) (1UL << (x))  // Masking bits

/*
 * @Function
 *     This function initializes TPM module
 * @credits: Alexander Dean Book
 * @Parametes: None
 * @Returns : None
 */

void Init_Pwm(void)
{
	//For Blue LED
	//enable clock to PORTD, PORTB, TPM0 and TPM2
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;

	//setting pins to FTM
	PORTB->PCR[RED_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED_PIN] |= PORT_PCR_MUX(3);

	PORTB->PCR[GREEN_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED_PIN] |= PORT_PCR_MUX(3);

	PORTD->PCR[BLUE_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED_PIN] |= PORT_PCR_MUX(4);

	//set clock source for TPM i.e. 48MHz
	SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1));

	//loading the counter and MOD
	TPM2->MOD = 48000-1;
	//set the TPM counter direction to up with the prescaler of 2
	TPM2->SC = TPM_SC_PS(1);
	//continue operation in DEBUG mode
	TPM2->CONF |= TPM_CONF_DBGMODE(3);
	//set channel 0 to edge aligned low-true PWM
	TPM2->CONTROLS[0].CnSC = TPM_CnSC_MSB_MASK| TPM_CnSC_ELSA_MASK;
	//set initial duty cycle as 0
	TPM2->CONTROLS[0].CnV = 0;
	//start TPM
	TPM2->SC |= TPM_SC_CMOD(1);

	// For RED LED

	//set channel 1 to edge aligned low-true PWM
	TPM2->CONTROLS[1].CnSC = TPM_CnSC_MSB_MASK| TPM_CnSC_ELSA_MASK;
	//set initial duty cycle as 0
	TPM2->CONTROLS[1].CnV = 0;
	//start TPM
	TPM2->SC |= TPM_SC_CMOD(1);


	// For GREEN LED

	TPM0->MOD = 48000-1;
	//set the TPM counter direction to up with the prescaler of 2
	TPM0->SC = TPM_SC_PS(1);
	//continue operation in DEBUG mode
	TPM0->CONF |= TPM_CONF_DBGMODE(3);
	//set channel 1 to edge aligned low-true PWM
	TPM0->CONTROLS[1].CnSC = TPM_CnSC_MSB_MASK| TPM_CnSC_ELSA_MASK;
	//set initial duty cycle as 0
	TPM0->CONTROLS[1].CnV = 0;
	//start TPM
	TPM0->SC |= TPM_SC_CMOD(1);
}



