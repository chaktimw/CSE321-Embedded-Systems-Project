-------------------
About
-------------------
Project Description: Pressing the button will toggle the blinking LED on and off.
Contributor List: Chaktim Wong

--------------------
Features
--------------------
Toggleable blinking LED

--------------------
Required Materials
--------------------
- Nucleo L4R5ZI

--------------------
Resources and References
--------------------
https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf

--------------------
Getting Started
--------------------
Connect the nucleo to your computer, and build the code in the provided file.
Upon completion, press the button a few times, and verify that each button press toggles the blinking led on and off.

--------------------
p1_code_provided.cpp:
--------------------
	This file contains code that controls an LED. Functions will establish LED2 as output and make it blink 
when the thread is activated. When the thread is activated, the LED will repeatedly turn on for 2000 units 
and turn off for at 500 units.

----------
Things Declared
----------
#include "mbed.h"

void led_thread();
void raise_flag(); 
void toggle_thread();

Thread controller; 

DigitalOut led_output(LED2); 
InterruptIn interrupt_trigger(BUTTON1); 

int thread_state = 0; 
int flag = 0;

----------
API and Built In Elements Used
----------
DigitalOut(LED2), InterruptIn(BUTTON1)

----------
Custom Functions
----------

void led_thread():
	Handles output to the LED based on the thread_state.
	Inputs:
		None
	Outputs:
		None
	Globally referenced things used:
		thread_state

void raise_flag():
	Sets flag to 1
	Inputs:
		None
	Outputs:
		None
	Globally referenced things used:
		flag

void toggle_thread():
	This function toggles the state of the thread and lowers the flag.
	Inputs:
		None
	Outputs:
		None
	Globally referenced things used:
		flag, thread_state
