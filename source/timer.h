/*
*********************************************************************************************************************
​ *
​ ​*​ ​@author​ :​Vishwanath Bhavikatti
 * @Credits : Embedded System Fundamentals with ARM cortex Microcontrollers Book by Alexander Dean chapter 7
​ ​*​ ​@date​ :March 5, 2023
 * @file name : timer.h
 * @file_description: It defines function prototypes for Init_SysTick, now(), reset_timer()
 * 					  and get_timer().
 *
*************************************************************************************************************************
*/
#ifndef TIMER_H_
#define TIMER_H_

#include "MKL25Z4.h"

void Init_SysTick(void);
typedef uint32_t ticktime_t;  // time since boot, in sixteenths of a second

ticktime_t now();
void reset_timer();
ticktime_t get_timer();
#endif /* TIMER_H_ */
