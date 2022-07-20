/**
 * Author: Chaktim Wong
 * ubit: chaktimw
 * person#: 50280143
 * 
 * Description: 
 *      User programmable all-in-one count-down alarm system with an LCD display and a keypad with
 *      16 buttons: 0-9, A-D, '*', and '#'. Additionally, each value input will light up a connected LED.
 *      Alarm code will loop until shutdown by user. 
 *
 * Modules: 
 *      1802.cpp, 1802.h, mbed.h
 *
 * Assignment: Project 2 - Midpoint
 *
 * Inputs: Keypad buttons 0-9, a, b, and d
 *
 * Outputs: LCD, leds connected to each output and input pin
 *
 * Constraints: 
 •	Time is entered as m:ss (up to 9:59)
 •	User can press A to start the timer, B to stop/turn off, and D to input the time
 •	User must use a prompt to enter time
 •	Every time a value is entered, an LED lights up
 •	The LCD will display "Time Remaining: " followed by the current time
 •	When the specified time is reached the LCD will display Times Up and multiple LEDs will turn on 
 •	Must run “forever”
 *
 * References: 
 *  - STM32L4+ Series User Manual: https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
 *  - Datasheet for the Mouser LCD Model
 *           
 */

#include "mbed.h"
#include "1802.h"
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <ctime>

// declare callback functions
void isr_147x(void);
void isr_2580(void);
void isr_369y(void);
void isr_ABCD(void);

// declare timer functions
void create_timer();
void start_timer();
void pause_timer();
void end_timer();

// declare keypress handler
void handle_keypress(char input);

// wait constant (1 sec = 1,000,000 us)
#define WAIT_TIME_US 500000

int row = 0;        // indicate which row of keypad to output to
int timer_mode = 0; // indicates current timer mode
char user_input [] = "000"; // stores user input to set timer
char user_display [] = "m:ss"; // stores display format for user input
int user_input_length = 0; // stores number of user inputs for timer
bool prompted = false; // indicates whether prompt has be displayed yet
unsigned int time_set = 0; // indicates time set (from user input) in seconds
unsigned int time_passed = 0;  // indicates time passed (seconds) since timer started

// create interrupts for each column wire on keypad
InterruptIn col_A(PF_5, PullDown);
InterruptIn col_B(PF_3, PullDown);
InterruptIn col_C(PD_2, PullDown);
InterruptIn col_D(PC_12, PullDown);

// create LCD object (need to figure out SDA and SCL pins)
CSE321_LCD display(16, 2, LCD_5x8DOTS, PB_9, PB_8);

int main() {
    // initialize the display
    display.begin();
    display.print("Timer");             // output to display
    display.setCursor(0, 1);            // go to next row on display
    display.print("initialized.");      // output to display
    display.setCursor(0, 0);            // reset cursor

    // enable clock on port C
    RCC->AHB2ENR |= 0x4;
    // enable output for port D pins 8,9,10,11
    GPIOC->MODER |= 0x550000;
    GPIOC->MODER &= ~(0xAA0000);

    // assign callback functions for each interrupt on button rise
    col_A.rise(&isr_147x);
    col_B.rise(&isr_2580);
    col_C.rise(&isr_369y);
    col_D.rise(&isr_ABCD);

while (1) {
    // Loop through keypad rows 0-3 (Polling loop)
    row = row + 1;
    row %= 4;
    switch(row) {
        case 0:
            GPIOC->ODR&=~(0x700); // turn off all other pins
            GPIOC->ODR|=  0x800;  // output to PC11            
            break;
        case 1:
            GPIOC->ODR&=~(0xB00); // turn off all other pins
            GPIOC->ODR|=  0x400;  // output to PC10
            break;
        case 2:
            GPIOC->ODR&=~(0xD00); // turn off all other pins
            GPIOC->ODR|=  0x200;  // output to PC9
            break;
        case 3:
            GPIOC->ODR&=~(0xE00); // turn off all other pins
            GPIOC->ODR |= 0x100;  // output to PC8
            break;
    }
    wait_us(400);  // delay poll to prevent incorrect key presses

    // update LCD display
    switch(timer_mode) {
        // set timer mode
        case 1:
            if (!prompted) {
                prompted = true;
                // display prompt
                display.clear();
                display.print("Set timer:");    // display prompt
                display.setCursor(0, 1);        // switch to row 2   
                display.print("m:ss");          // display timer format
                display.setCursor(0, 1);        // reset cursor
            }
            display.setCursor(0, 1);
            display.print(user_display);
            break;

        // started timer mode
        case 2:
            if (!prompted) {
                prompted = true;
                // display prompt
                display.clear();
                display.print("Time Remaining:");    // display prompt
                display.setCursor(0, 1);        // switch to row 2   
            }

            // update display if a second has passed
            if (time(NULL) > time_passed) {
                time_passed = time(NULL);  // update time passed

                // calculate time remaining (seconds)
                unsigned int temp = time_set;
                unsigned int time_set_minutes = temp / 100;
                temp -= time_set_minutes * 100; 
                unsigned int time_set_seconds = (time_set_minutes * 60) + (temp / 10) * 10 + (temp - (temp / 10)); // combine time into seconds
                unsigned int time_remaining = time_set_seconds - time_passed;

                // end timer if time remaining hits 0
                if (time_remaining == 0) {
                    end_timer();
                }

                // update user_display with time remaining
                user_display[0] = '0' + time_remaining / 60;
                time_remaining -= (time_remaining / 60 * 60);
                user_display[2] = '0' + time_remaining / 10;
                time_remaining -= (time_remaining / 10 * 10);
                user_display[3] = '0' + time_remaining;
                
                // print user_display to display
                display.setCursor(0, 1);
                display.print(user_display);
            }
            break;

        // end timer
        case 4:
            if (!prompted) {
                prompted = true;
                // display prompt
                display.clear();
                display.print("Times Up:");    // display prompt
                display.setCursor(0, 1);        // switch to row 2   
                display.print("0:00");          // display timer format
                display.setCursor(0, 1);        // reset cursor
            }
            break;
    }
}
  return 0; }

void isr_147x(void) {
    switch(row) {
        case 0:
            printf("found 1\n");
            handle_keypress('1');
            break;
        case 1:
            printf("found 4\n");
            handle_keypress('4');
            break;
        case 2:
            printf("found 7\n");
            handle_keypress('7');
            break;
        case 3:
            printf("found *\n");
            handle_keypress('*');
            break;
    }
    wait_us(WAIT_TIME_US);
}
void isr_2580(void) {
    switch(row) {
        case 0:
            printf("found 2\n");
            handle_keypress('2');
            break;
        case 1:
            printf("found 5\n");
            handle_keypress('5');
            break;
        case 2:
            printf("found 8\n");
            handle_keypress('8');
            break;
        case 3:
            printf("found 0\n");
            handle_keypress('0');
            break;
    }
    wait_us(WAIT_TIME_US);
}
void isr_369y(void) {
    switch(row) {
        case 0:
            printf("found 3\n");
            handle_keypress('3');
            break;
        case 1:
            printf("found 6\n");
            handle_keypress('6');
            break;
        case 2:
            printf("found 9\n");
            handle_keypress('9');
            break;
        case 3:
            printf("found #\n");
            handle_keypress('#');
            break;
    }
    wait_us(WAIT_TIME_US);
}
void isr_ABCD(void) {
    switch(row) {
        case 0:
            printf("found A\n");
            handle_keypress('A');
            break;
        case 1:
            printf("found B\n");
            handle_keypress('B');
            break;
        case 2:
            printf("found C\n");
            handle_keypress('C');
            break;
        case 3:
            display.print("test");
            // display.clear();
            printf("found D\n");
            handle_keypress('D');
            break;
    }
    wait_us(WAIT_TIME_US);
}

void create_timer(void) {
    timer_mode = 1;     // set mode to create timer mode
    prompted = false;   // main will display prompt
    time_passed = 0;    // reset time passed for start timer mode

    // reset user_input vars
    user_display[0] = 'm';
    user_display[2] = 's';
    user_display[3] = 's';
    user_input[0] = user_input[1] = user_input[2] = '0';
    user_input_length = 0;
}

void start_timer(void) {
    timer_mode = 2;     // set mode to start timer mode
    prompted = false;   // main will display prompt

    // initiate timer
    set_time(time_passed);  // creates a reference point for time passed
    time_set = strtoul(user_input,NULL,10); // convert user input to unsigned int
    // printf("Time set = %u\n", time_set);
}

void pause_timer(void) {
    timer_mode = 3; // set mode to pause timer mode
}

void end_timer(void){
    timer_mode = 4; // set mode to 4 to display "Times Up"
    prompted = false;   // main will display message

    // turn on all LEDs
    // reset to initial configuration
}

void handle_keypress(char input){
    switch(timer_mode){
        // default mode
        case 0:
            // check key press
            switch(input){
                // input timer
                case 'D':
                    printf("setting new timer...\n");
                    create_timer();
                    break;
            }
            break;
            
        // set timer mode
        case 1:
            // check key press
            switch(input){
                // start timer
                case 'A':
                    printf("starting timer...\n");
                    start_timer();
                    break;
                // input timer (resets user_input)
                case 'D':
                    printf("setting new timer...\n");
                    create_timer();
                    break;
                // number inputs add to user_input var to set timer
                default:
                    if(isdigit(input) && user_input_length < 3) {
                        // add input to user_input var but limit to 9:59
                        user_input[user_input_length] = input;
                        if (user_input_length == 1 && int(input) > int('5')) {
                            user_input[user_input_length] = '5';
                        }
                        // increment user_input_length and update user_display
                        switch (user_input_length) {
                            case 0:
                                user_display[0] = user_input[0];
                                break;
                            case 1:
                                user_display[2] = user_input[1];
                                break;
                            case 2:
                                user_display[3] = user_input[2];
                                break;
                        }
                        user_input_length++;
                    }
            }
            break;

        // started timer mode (countdown mode)
        case 2:
            // check key press
            switch(input){
                // pause timer
                case 'B':
                    printf("pausing timer...\n");
                    pause_timer();
                    break;
            }
            break;

        // paused timer mode 
        case 3:
            // check key press
            switch(input){
                // start timer
                case 'A':
                    printf("starting timer...\n");
                    start_timer();
                    break;
                // input timer
                case 'D':
                    printf("setting new timer...\n");
                    create_timer();
                    break;
            }
            break;

        // paused timer mode 
        case 4:
            // check key press
            switch(input){
                // input timer
                case 'D':
                    printf("setting new timer...\n");
                    create_timer();
                    break;
            }
            break;   
    }
}
