/**
 * Old Filename: p1_code_provided.cpp
 * Updated Filename: CSE321_project1_chaktimw_corrected_code
 * 
 * Author: N/A
 * Contributors: Chaktim Wong
 * Date: 10/4/2021
 * 
 * Summary of File:
 * 
 *   This file contains code that controls an LED.
 *   Functions will establish LED2 as output and make it blink 
 *   when the thread is activated. When the thread is activated, 
 *   the LED will repeatedly turn on for 2000 units 
 *   and turn off for at 500 units.
 *
 */
#include "mbed.h"

void led_thread();
void raise_flag(); 
void toggle_thread();

// create a thread to drive an LED 
Thread controller; 

// establish blue led as an output
DigitalOut led_output(LED2); 

// establish button as trigger
InterruptIn interrupt_trigger(BUTTON1); 

// indicator_values
int thread_state = 0; 
int flag = 0;

int main() {
  printf("----------------START----------------\n");
	printf("Starting state of thread: %d\n", controller.get_state());
  controller.start(led_thread); // start the allowed execution of the thread
	printf("State of thread right after start: %d\n", controller.get_state());
  interrupt_trigger.rise(raise_flag);  // NOTE: button is pushed down
	interrupt_trigger.fall(toggle_thread); // NOTE: button is let go
  return 0;
}

/**
 *
 * void led_thread()
 *
 * Paramters    : None
 * 
 * Return Value : None
 *
 * Description:
 *
 *    This function handles output to the LED based on the thread_state.
 *    While the thread_state is 0, the led is blinking. Otherwise, it
 *    is off.
 *
 */
void led_thread() {
  while (true) {
    if(thread_state == 0){
      led_output = !led_output;
      printf("LED on");
      thread_sleep_for(2000); //Thread_sleep is a time delay function, causes a 2000 unit delay
      led_output = !led_output;
      printf("LED off"); 
      thread_sleep_for(500); //Thread_sleep is a time delay function, causes a 500 unit delay
    }
  }
}

/**
 *
 * void raise_flag();
 *
 * Paramters    : None
 * 
 * Return Value : None
 *
 * Description:
 *
 *    This function raises the flag.
 *
 */
void raise_flag() {
	flag=1; // sets flag to 1
}

/**
 *
 * void toggle_thread();
 *
 * Paramters    : None
 * 
 * Return Value : None
 *
 * Description:
 *
 *    This function toggles the state of the thread
 *    and lowers the flag.
 *
 */
void toggle_thread() {
  if (flag==1){
    thread_state ++; 
	  thread_state %= 2; 						    
    flag=0;
  }
}