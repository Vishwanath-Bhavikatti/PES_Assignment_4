[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-c66648af7eb3fe8bc4f294546bfd86ef473780cde1dea487d3c4ff354943c9ae.svg)](https://classroom.github.com/online_ide?assignment_repo_id=10386395&assignment_repo_type=AssignmentRepo)
# Course: Principle of Embedded Software - ECEN 5813 (Spring 2023)
# Assignment 4
## Author : Vishwanath Bhavikatti (vishwanathbhavikatti1310@gmail.com)
## Date : 3/7/2023

# Description: 
In this assignment I am doing bare-metal C executable program for Traffic light implementation using state machines that runs on the KL25Z. 

## Compiler Used:
`gcc (with -WALL and -WERROR Flags enabled)`
## IDE and Development board
<br> IDE: MCUXpresso IDE 11.6.1
<br> Development Board: MKL25Z

## Implementation Details:
<br>  When the traffic light is in the STOP state, Boulder residents use the color #FF0000 .
<br>  When the traffic light is in the GO state, Boulder residents use the color #00FF00
<br>  When the traffic light is in the WARNING state, Boulder residents use the color #FFFF00
<br>  When someone presses the button to cross the street (CROSSWALK state), the traffic light is blinking with the color #001030. 
<br>
<br>  In the CROSSWALK state, the light blinks 250 msec off, 750 msec on. In all other states, the light is solid (not blinking).
<br>  Except for the blinking in CROSSWALK mode, all other changes in traffic light color must transition gradually and linearly over a one-second period. In my case of implementation 62.5ms into transition.
	
### Timing Sequence
<br>  STOP: Debug - 5 Sec  Production - 20 Sec
<br>  One sec transition to GO state
<br>  GO: Debug - 5 Sec  Production - 20 Sec
<br>  One sec transition to WARNING state
<br>  WARNING: Debug - 3 Sec  Production - 5 Sec

<br> At any point in the above cycle, if a resident presses the button to cross the street (represented on our FRDM-KL25Z by a touch anywhere on the capacitive slider), the traffic light should do the following:
<br>  Transition, over a one-second period, to WARNING state and color(0xFFFF00). The color transition to WARNING state should follow the gradual change rule above.
<br>  Stay in WARNING state for 5 seconds(or 3 seconds in DEBUG mode). If the previous state was WARNING, the system will still follow the exact same rule of transition and then stay for 5 seconds (or 3 seconds in DEBUG mode) extra.
<br>  Transition, over a one-second period, from WARNING to the CROSSWALK color (#001030). This one-second transition should follow the approach given above for gradual color transition.
<br>  Stay in the CROSSWALK mode for 5 seconds, blinking 250 msec off, 750 msec on. (This duration is the same in both DEBUG and PRODUCTION modes.)
<br> Transition, over a one-second period, from the CROSSWALK state to the GO state.

<br>  Button presses are ignored if they arrive when the light is either transitioning into or is already in the CROSSWALK state. Otherwise, the transition into the CROSSWALK state must start within 100 msec of a button press occurring.

## Source Files Included:
<br>Logic Functions
<br> main.c
<br> states.c
<br> states.h
<br> touch.c
<br> touch.h
<br> tpm.c
<br> tpm.h
<br> timer.c
<br> timer.h
<br> log.h


### Inbuilt Functions
<br> mtb.c
<br> semihost_hardfault.c

### Special Instructions while testing on KL25Z dev board.
<br> For my development board I considered 160 as my "Touch_offset_value from touch sense.
<br> I am using 62.5 ms as unit time and SysTick timer with external clock source. 

### Extra credit part
<br> I used PORTD PIN 3 as my external GPIO Interrupt pin with pull up resistor. In addition to basic polling of touch sense from touch pad even GPIO interrupt can take care of transitioning to crosswalk state. 

## Reviewer:
Name of the reviewer : Aamir Suhail Burhan
## Review Date: 
Date reviewed: 3/7/2023
## Review Comments:
1. Comment: Floating point can be used to display time since startup by changing preprocessor settings.
    - Response: Somehow its not working properly in my board, confirmed with one of the TA to use %d which is also fine.
2. Comment: Add reference of Slides for log.h file.
    - Response: I did the aboe change. It was actually done by mistake.
3. Comment: Remove magic number '4' for dividing by 16.
    - Response: I agree and I changed it.
4. Comment:Change the macro "TOUCH_OFFSET_VALUE" which represent minimum value for a touch to be detected so that it is easier to understand.
    - Response: Make sense I did it.
5. Comment:While trnasitioning from Warning state to crosswalk state, transition to #000000 instead of #001030. (250msec off then 750msec on)
    - Response: Yeah I have changed that, actually I thought it should gradually transit to cross walk state.
6. Comment:Well defined states which include transitions as well.
    - Response: Thanks
7. Comment:Overall good flow of program.
    - Response: Thanks
