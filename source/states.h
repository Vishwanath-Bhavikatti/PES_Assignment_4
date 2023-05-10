/*
*********************************************************************************************************************
​ *
​ ​*​ ​@author​ :​Vishwanath Bhavikatti
 * @credits: Hari (PES TA) for helping me understand state machines and TSI resistors
​ ​*​ ​@date​ :March 5, 2023
 * @file name : states.h
 * @file_description: This file includes function prototypes of initial_pattern and
 *                    traffic pattern.
 *
 *
*************************************************************************************************************************
*/

#ifndef STATES_H_
#define STATES_H_

#include "MKL25Z4.h"


void init_pattern();
void traffic_pattern(uint32_t touch_value);

#endif /* STATES_H_ */
