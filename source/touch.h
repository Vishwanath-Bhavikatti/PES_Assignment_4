/*
*********************************************************************************************************************
​ *
​ ​*​ ​@author​ :​Vishwanath Bhavikatti
 * @Credits : https://github.com/alexander-g-dean/ESF/blob/master/NXP/Misc/Touch%20Sense/TSI/src/main.c
​ ​*​ ​@date​ :March 5, 2023
 * @file name : touch.h
 * @file_description: This file includes Touch_init() and Touch_Scan_LH() function prototypes
 *
*************************************************************************************************************************
*/

#ifndef TOUCH_H_
#define TOUCH_H_

void Touch_Init(); // Initialization
int Touch_Scan_LH(void); // returns touch scan value

#endif /* TOUCH_H_ */
