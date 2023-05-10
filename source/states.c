/*
*********************************************************************************************************************
​ *
​ ​*​ ​@author​ :​Vishwanath Bhavikatti
 * @credits: Hari (PES TA) for helping me understand state machines and TSI resistors
​ ​*​ ​@date​ :March 5, 2023
 * @file name : states.c
 * @file_description: This is a state machine function which has sufficient number of states
 * 					 to carry out the Traffic pattern states as directed by the assignment document.
 * 					 It includes MKL25Z4.h, core_cm0plus.h, tpm.h, timer.h, log.h,states.h whose
 * 					 function prototypes are being used in this file.
 *
*************************************************************************************************************************
*/
#include "MKL25Z4.h"
#include "core_cm0plus.h"
#include "fsl_debug_console.h"
#include <stdio.h>
#include "tpm.h"
#include"timer.h"
#include "log.h"
#include "states.h"

/*
 * ifdef for defining timings in debug and release (production) mode
 * For STOP and GO states Debug timings = 5 seconds and Release timing = 20 seconds
 * For Warning state Debug timing = 3 seconds and Release timing = 5 seconds
 */

#ifdef DEBUG
#  define TICK_TIMER           (80)   // 80 * 62.5 ms  = 5 second Timer
#else
#  define TICK_TIMER           (320)  // 320 * 62.5 ms  = 20 second Timer
#endif

#ifdef DEBUG
#  define TICK_TIMER_WNG       (48)   // 48 * 62.5 ms  = 3 second Timer
#else
#  define TICK_TIMER_WNG       (80)   // 80 * 62.5 ms  = 5 second Timer
#endif

// Defines

#define TOUCH_SENSE_VALUE     (160)  // offset value of my dev board touch pad
#define ONE_TICKTIME           (1)    // 62.5ms unit time
#define UNIT_TIME              (62.5) //uint time from systick timer
#define ZERO_MSEC              (0)    // zero ms
#define TWO_FIFTY_MSEC         (4)    // 62.5ms unit time multiplied by 4 gives 250ms
#define ONE_SEC                (16)   // 62.5ms unit time multiplied by 16 gives 1000ms
// represents FF to particular color in RGB patter _ _ _ _ _ _  FF0000 / 00FF00 /  0000FF
#define LOAD_CNV_ON            (48000) //100% duty cycle to turn ON completely LED
// represents 00 to particular color in RGB patter _ _ _ _ _ _ 000000
//(001030) colour pattern for cross walk : as 48000 maps to FF of each
//color in RGB pattern so 3000 maps to 10(Hex value) and 9000 maps to 30(Hex value)
#define LOAD_CNV_OFF           (0)    // Zero duty cycle to turn off leds
#define CROSS_WALK_GREEN_ON    (3000) //Load value to turn green content in cross walk
#define CROSS_WALK_BLUE_ON     (9000) //Load value to turn blue content in cross walk
#define FIVE_SEC_COUNT         (5)    // incrementing count till 5 counts from 0 to get 5 seconds delay
#define ONE_SEC_COUNT          (15)   // incrementing count till 16 counts from 0 to get 1 seconds delay
#define DIVIDE_BY_SIXTEEN      (4)    // >> 4 is basically divide by 4
// GPIO Interrupt defines
#define GPIO_INT_PIN           (3)     //GPIO_Interrupt Pin
#define MASK(x)                (1UL << (x))  // Masking bits

//Variables Initialization

int count = 0;  // counter for 1 second transition
int blink_count = 0; //counter for 250ms OFF and 750ms ON
uint32_t touch_flag = 0; //Flag for touch sense
uint32_t red_gradient_value = 0; //gradient red for storing value during transition touch
uint32_t green_gradient_value = 0;//gradient green for storing value during transition touch
int gpio_pin_status;  // gpio_interrupt_status

/*
 * @Function
 *     This function is GPIO PORTD_IRQ Handler
 *     Just sets the gpio pin status and clears the Interrupt status Flag Register.
 * @Parametes: None
 * @Returns : None
 */
void PORTD_IRQHandler()
{
	gpio_pin_status = 1; // setting status pin when switch interrupt comes
	PORTD->ISFR |= MASK(GPIO_INT_PIN); // clearing Interrupt status Flag Register in ISR
}


/*
 * Stop_state                       --> Stop state with FF0000 LED
 * go_state                         --> Go state with 00FF00 LED
 * warning_state                    --> Warning state with FFFF00 LED
 * crosswalk_state                  --> Cross walk state with 001030 LED
 * stop_go_transition               --> Stop -> Go transition LED
 * stop_go_touch_transition         --> Stop -> Warning transition LED when touch sensed
 * stop_warning_transition          --> Stop -> Warning transition LED
 * go_warning_transition            --> Go -> Warning transition LED
 * warning_stop_transition          --> Warning -> Stop transition LED
 * warning_stop_touch_transition    --> Warning -> stop transition LED when touch sensed
 * cswk_go_transition               --> Cross walk -> Go transition LED
 * warning_cswk_transition          --> Warning -> Cross walk transition LED
 */

enum states{
	stop_state,
	go_state,
	warning_state,
	crosswalk_state,
	stop_go_transition,
	stop_go_touch_transition,
	stop_warning_transition,
	go_warning_transition,
	warning_stop_transition,
	warning_stop_touch_transition,
	cswk_go_transition,
	warning_cswk_transition,
}current_state;

/*
 * @Function
 *     This function initializes initial STOP state of the state machine model
 * @Parametes: None
 * @Returns : None
 */

void init_pattern()
{

	TPM2->CONTROLS[1].CnV = LOAD_CNV_OFF; //GREEN OFF
	TPM2->CONTROLS[0].CnV = LOAD_CNV_ON; //RED ON
	current_state = stop_state;
	LOG("Starting with STOP state -- System Time: %d msec\n\r",now());
}

/*
 * @Function
 *     This function include all the states in switch case which performs individual task.
 * @Parametes: touch sense value
 * @Returns : None
 */

void traffic_pattern(uint32_t touch_value)
{
	switch(current_state){
		//STOP
		case stop_state:
			// If touch sense triggers -> transitioning to WARNING
			if((touch_value > TOUCH_SENSE_VALUE) || (gpio_pin_status == 1))
			{
				LOG("Button Press Detected -- System Time: %d msec\n\r",now());
				current_state = stop_warning_transition;
				LOG("STOP -> WARNING Transition -- System Time: %d msec\n\r",now());
				touch_flag = 1;
				gpio_pin_status = 0; // clearing interrupt status pin
			}
			// Wait for 5sec (debug) / 20sec(release) in STOP mode
			else if((get_timer() >= TICK_TIMER))
			{
				reset_timer();
				current_state = stop_go_transition;
				LOG("STOP -> GO Transition -- System Time: %d msec\n\r",now());
				touch_flag = 0;
			}

			break;
		// GO
		case go_state:
			// If touch sense triggers -> transitioning to WARNING
			if((touch_value > TOUCH_SENSE_VALUE) || (gpio_pin_status == 1))
			{
				LOG("Button Press Detected -- System Time: %d msec\n\r",now());
				current_state = go_warning_transition;
				LOG("GO -> WARNING Transition -- System Time: %d msec\n\r",now());
				touch_flag = 1;
				gpio_pin_status = 0;
			}
			// Wait for 5sec (debug) / 20sec(release) in GO mode
			else if(get_timer() >= TICK_TIMER)
			{
				reset_timer();
				current_state = go_warning_transition;
				LOG("GO -> WARNING Transition -- System Time: %d msec\n\r",now());
				touch_flag = 0;
			}

	 		break;
	 	// Warning
		case warning_state:
			// If touch sense triggers -> stay in WARNING
			if((touch_value > TOUCH_SENSE_VALUE) || (gpio_pin_status == 1))
			{
				LOG("Button Press Detected -- System Time: %d msec\n\r",now());
				current_state = warning_state;
				LOG("Staying in WARNING state -- System Time: %d msec\n\r",now());
				touch_flag = 1;
				gpio_pin_status = 0;
				reset_timer(); // resetting timer to zero so that when touch interrupts comes
				//in warning state it will again wait for 5 seconds extra
			}
			// Wait for 3sec (debug) / 5sec(release) in WARNING mode
			else if(get_timer() >= TICK_TIMER_WNG)
			{
				if(touch_flag == 1)
				{
					reset_timer();
					current_state = warning_cswk_transition;
					LOG("WARNING -> CROSSWALK Transition -- System Time: %d msec\n\r",now());
					touch_flag = 0;
				}
				else
				{
					reset_timer();
					current_state = warning_stop_transition;
					LOG("WARNING -> STOP Transition -- System Time: %d msec\n\r",now());

				}

			}

			break;
		// STOP -> GO transition over 1 second
		case stop_go_transition:
			// Waits for Unit time of 62.5ms
			if(get_timer() >= ONE_TICKTIME)
			{
				// If touch sense triggers -> go to a transiion state which transits to WARNING state
				if((touch_value > TOUCH_SENSE_VALUE) || (gpio_pin_status == 1))
				{
					LOG("Button Press Detected -- System Time: %d msec\n\r",now());
					current_state = stop_go_touch_transition;

					touch_flag = 1;
					gpio_pin_status = 0;
					red_gradient_value = TPM2->CONTROLS[0].CnV;
					green_gradient_value = TPM2->CONTROLS[1].CnV;
					count = 0;
					reset_timer(); // resetting timer to zero so that when touch interrupts comes
					//in warning state it will again wait for 5 seconds extra

				}
				// transition to GO state
				else
				{
					TPM2->CONTROLS[0].CnV -= (LOAD_CNV_ON >> DIVIDE_BY_SIXTEEN); // 45000, 42000... RED decreasing intensity
					TPM2->CONTROLS[1].CnV += (LOAD_CNV_ON >> DIVIDE_BY_SIXTEEN); // 3000 , 6000 .... Green increase intensity
					TPM0->CONTROLS[1].CnV = LOAD_CNV_OFF; //Blue_OFF
					count++;
					if(count == ONE_SEC_COUNT)
					{
						current_state = go_state;
						count = 0;
						reset_timer();
					}
					else
					{
						current_state = stop_go_transition;
						reset_timer();

					}

				}
			}
			break;
		// STOP -> GO Transition in case of touch sense
		case stop_go_touch_transition:
			if(get_timer() >= ONE_TICKTIME)
			{
				TPM2->CONTROLS[0].CnV += ((LOAD_CNV_ON - red_gradient_value) >> DIVIDE_BY_SIXTEEN); // RED increasing intensity
				TPM2->CONTROLS[1].CnV += ((LOAD_CNV_ON- green_gradient_value) >> DIVIDE_BY_SIXTEEN); // Green increase intensity
				TPM0->CONTROLS[1].CnV = LOAD_CNV_OFF; //Blue_OFF
				count++;
				if(count == ONE_SEC_COUNT)
				{
					current_state = warning_state;
					LOG("STOP Transition -> WARNING -- System Time: %d msec\n\r",now());
					count = 0;
					reset_timer();
				}
				else
				{
					current_state = stop_go_touch_transition;
					reset_timer();

				}

			}
			break;
		// GO -> WARNING state transition
		case go_warning_transition:
			if(get_timer() >= ONE_TICKTIME)
			{
				if((touch_value > TOUCH_SENSE_VALUE) || (gpio_pin_status == 1))
				{
					LOG("Button Press Detected -- System Time: %d msec\n\r",now());
					current_state = go_warning_transition;
					touch_flag = 1;
					gpio_pin_status = 0;
				}
				TPM2->CONTROLS[0].CnV += (LOAD_CNV_ON >> DIVIDE_BY_SIXTEEN); // 3000 , 6000 .... RED increase intensity
				TPM0->CONTROLS[1].CnV = LOAD_CNV_OFF; //Blue_OFF
				count++;
				//printf("transition_state_internal :Count :%d \n\r",count);
				if(count == ONE_SEC_COUNT)
				{
					current_state = warning_state;
					count = 0;
					reset_timer();
				}
				else
				{
					current_state = go_warning_transition;
					reset_timer();

				}

			}
			break;
		// WARNING -> STOP Transition
		case warning_stop_transition:
			if(get_timer() >= ONE_TICKTIME)
			{
				if((touch_value > TOUCH_SENSE_VALUE) || (gpio_pin_status == 1))
				{
					LOG("Button Press Detected -- System Time: %d msec\n\r",now());
					current_state = warning_stop_touch_transition;

					touch_flag = 1;
					gpio_pin_status = 0;
					green_gradient_value = TPM2->CONTROLS[1].CnV;
					count = 0;
					reset_timer(); // resetting timer to zero so that when touch interrupts comes
					//in warning state it will again wait for 5 seconds extra
				}

				else
				{
					TPM2->CONTROLS[0].CnV = LOAD_CNV_ON; //RED_ON


					TPM2->CONTROLS[1].CnV -= (LOAD_CNV_ON >> DIVIDE_BY_SIXTEEN); // 45000, 42000... GREEN decreasing intensity
					TPM0->CONTROLS[1].CnV = LOAD_CNV_OFF; //Blue_OFF
					count++;
					if(count == ONE_SEC_COUNT)
					{
						current_state = stop_state;
						count = 0;
						reset_timer();
					}
					else
					{
						current_state = warning_stop_transition;
						reset_timer();

					}

				}
			}
			break;
		// WARNING -> STOP  Transition in case of touch
		case warning_stop_touch_transition:
			if(get_timer() >= ONE_TICKTIME)
			{
				TPM2->CONTROLS[1].CnV += ((LOAD_CNV_ON- green_gradient_value) >> DIVIDE_BY_SIXTEEN); // Green increase intensity
				TPM0->CONTROLS[1].CnV = LOAD_CNV_OFF; //Blue_OFF
				count++;
				if(count == ONE_SEC_COUNT)
				{
					current_state = warning_state;
					LOG(" STOP Transition -> WARNING Transition -- System Time: %d msec\n\r",now());
					count = 0;
					reset_timer();
				}
				else
				{
					current_state = warning_stop_touch_transition;
					reset_timer();

				}

			}
			break;
		// STOP -> WARNING transition
		case stop_warning_transition:
			if(get_timer() >= ONE_TICKTIME)
			{
				TPM2->CONTROLS[1].CnV += (LOAD_CNV_ON >> DIVIDE_BY_SIXTEEN); // 3000, 6000... GREEN increasing intensity
				TPM0->CONTROLS[1].CnV = LOAD_CNV_OFF; //Blue_OFF
				count++;
				if(count == ONE_SEC_COUNT)
				{
					current_state = warning_state;
					count = 0;
					reset_timer();
				}
				else
				{
					current_state = stop_warning_transition;
					reset_timer();

				}

			}
			break;
		// WARNING -> CROSSWALK Transition
		case warning_cswk_transition:
			if(get_timer() >= ONE_TICKTIME)
			{
				TPM2->CONTROLS[0].CnV -= (LOAD_CNV_ON >> DIVIDE_BY_SIXTEEN); // 45000, 42000... RED decreasing intensity
				TPM2->CONTROLS[1].CnV -= (LOAD_CNV_ON >> DIVIDE_BY_SIXTEEN); // 45000, 42000... GREEN decreasing intensity
				TPM0->CONTROLS[1].CnV = LOAD_CNV_OFF; //BLUE OFF
				count++;
				if(count == ONE_SEC_COUNT)
				{
					current_state = crosswalk_state;
					count = 0;
					reset_timer();
				}
				else
				{
					current_state = warning_cswk_transition;
					reset_timer();

				}

			}
			break;
		// CROSS WALK to GO Transition
		case cswk_go_transition:
			if(get_timer() >= ONE_TICKTIME)
			{
				TPM2->CONTROLS[1].CnV += ((LOAD_CNV_ON - CROSS_WALK_GREEN_ON) >> DIVIDE_BY_SIXTEEN); // 48000 - (45000/16), 45187.5 - (45000/16),... GREEN increases intensity to LOAD_CNV_ON
				TPM0->CONTROLS[1].CnV -= ((CROSS_WALK_BLUE_ON - LOAD_CNV_OFF) >> DIVIDE_BY_SIXTEEN); //Blue increases its intensity from 0 to CROSS_WALK_BLUE_ON
				TPM2->CONTROLS[0].CnV = LOAD_CNV_OFF; //RED_OFF
				count++;
				if(count == ONE_SEC_COUNT)
				{
					current_state = go_state;
					LOG(" CROSSWALK -> GO Transition -- System Time: %d msec\n\r",now());
					count = 0;
					gpio_pin_status = 0; // disabling gpio interrupt in the midst of cross walk till go
					reset_timer();
				}
				else
				{
					current_state = cswk_go_transition;
					reset_timer();

				}

			}
			break;
		// CROSS WALK
		case crosswalk_state:
			//250 ms OFF
			if((get_timer() > ZERO_MSEC) && (get_timer() <= TWO_FIFTY_MSEC))
			{
				TPM2->CONTROLS[1].CnV = LOAD_CNV_OFF; //Green content OFF
				TPM0->CONTROLS[1].CnV = LOAD_CNV_OFF; //Blue content OFF
				TPM2->CONTROLS[0].CnV = LOAD_CNV_OFF; //RED OFF

			}
			//750 ms ON
			else if((get_timer() > TWO_FIFTY_MSEC) && (get_timer() <= ONE_SEC))
			{
				TPM2->CONTROLS[0].CnV = LOAD_CNV_OFF; //RED OFF
				TPM2->CONTROLS[1].CnV = CROSS_WALK_GREEN_ON; //Green content ON
				TPM0->CONTROLS[1].CnV = CROSS_WALK_BLUE_ON; //Blue content ON
			}
			//for 5 seconds
			else if(get_timer() >= ONE_SEC)
			{
				reset_timer();
				blink_count++;
				if(blink_count == FIVE_SEC_COUNT)
				{
					current_state = cswk_go_transition;
					blink_count = 0;
				}
				else
					current_state = crosswalk_state;
			}
			break;

		default:
			break;
	}

}
