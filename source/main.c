
/*
*********************************************************************************************************************
​ *
​ ​*​ ​@author​ :​Vishwanath Bhavikatti
​ ​*​ ​@date​ :March 5, 2023
 * @file name : main.c
 * @file_description: This main file includes all the necessary supported header files and
 * 					  Implemented function headers. It calls all initialization functions for
 * 					  TPM,Timer(Systick), states and Touch.
 * 					  It calls a infinite while loop to continuously run the program.
 *
*************************************************************************************************************************
*/

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "core_cm0plus.h"
/* TODO: insert other include files here. */

#include "tpm.h"
#include "timer.h"
#include "log.h"
#include "states.h"
#include "touch.h"
#include "gpio.h"
/* TODO: insert other definitions and declarations here. */
#define PWM_PERIOD 48000  // 48MHz / 48K = 1000HZ ; 1000/prescalr of 2 = 500Hz
volatile uint32_t touch_value = 0 ; // initializing touch value to zero
/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    LOG("TRAFFIC LIGHT STATES MAIN LOOP STARTS HERE\n\r");
    //SysTick Initialization
    Init_SysTick();
    //TPM Initialization
    Init_Pwm();
    //Initial Pattern of STOP state
    init_pattern();
    //Initial GPIO Interrupt
    gpio_init();
    //Touch sense initialization
    Touch_Init();
    //timer rest before start of function
    reset_timer();
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {

    	touch_value = Touch_Scan_LH(); // Polling touch sense value every 62.5 ms
    	traffic_pattern(touch_value); // calling traffic states

    }
    return 0 ;
}
