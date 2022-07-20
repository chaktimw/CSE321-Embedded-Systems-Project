-------------------
About
-------------------
Project Description: All-in-one count-down alarm system that users can program.
Contributor List: Chaktim Wong

--------------------
Features
--------------------
- Programmable timer with keypad and LCD display
- Lit LEDs for each keypad input and when time is up

--------------------
Required Materials
--------------------
- Nucleo L4R5ZI
- LCD
- Keypad
- Solderless Breadboard
- Jumper wires
- LEDs

--------------------
Resources and References
--------------------
- Reference Manual: https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
- Time API: https://os.mbed.com/docs/mbed-os/v6.15/apis/time.html
- Datasheet for the Mouser LCD Model: https://learn-us-east-1-prod-fleet02-xythos.content.blackboardcdn.com/5e00ea752296c/10911180?X-Blackboard-Expiration=1635660000000&X-Blackboard-Signature=KASCrt%2FhdHjf1tiGizJpF5TgwEln6wCDgPYO7FqTq3s%3D&X-Blackboard-Client-Id=100310&response-cache-control=private%2C%20max-age%3D21600&response-content-disposition=inline%3B%20filename%2A%3DUTF-8%27%27JDH_1804_Datasheet.pdf&response-content-type=application%2Fpdf&X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Date=20211031T000000Z&X-Amz-SignedHeaders=host&X-Amz-Expires=21600&X-Amz-Credential=AKIAZH6WM4PL5SJBSTP6%2F20211031%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Signature=4d5ef6bbf416f1f2037ced29da4a121d97a1dc6b3fdf9e64f3a60ba042a28511
- 1802.cpp and 1802.h

--------------------
Getting Started
--------------------
1) Connect pin GND to negative side of breadboard.
2) Connect Port C pins 8, 9, 10, and 11 to breadboard with an LED, and resistor to ground. 
3) Connect in series with the positive side of the LEDs (step 2) to the keypad row pins. Make sure that row 1 is pin 11, row 2 is pin 10, etc.
4) Connect pins PC12, PD2, PF3, PF5 to the keypad colum pins. Make sure that col 4 is PC12, col 3 is PD2, etc.
6) Connect the LCD to the nucleo using chosen SDA pin, SCL pin, 5v pin, and any gnd pin. 
7) Connect the nucleo to your computer, and build the code in main.cpp.

--------------------
main.cpp:
--------------------
This file contains code that reads input from a keypad and outputs to an LCD. During runtime, the ports representing each row of the keypad is
temporarily set to output to check for button presses. Interrupts handle inputs from each column (if any) and calls the appropiate callback function
to determine which key was pressed. Button press "D" initaites the set timer loop by outputting a prompt to the user to input the timer, and loops until 
a timer has been set using the number buttons and button "A" is pressed. From there, the loop switches to the timer mode which decreases the timer by one 
for each second that passes and outputs the time remaining to the LCD. This mode continues until button "B" is pressed to stop/pause the timer, or the timer 
hits 0 mins and 0 secs at which point, "Times Up", is outputted to the LCD. The user at this point can set a new timer by pressing button "A". 

----------
Things Declared
----------
*all semicolons have been excluded*

// declared callback functions
- void isr_147x(void)
- void isr_2580(void)
- void isr_369y(void)
- void isr_ABCD(void)

// declared timer functions
- void create_timer()
- void start_timer()
- void pause_timer()
- void end_timer()

// declared keypress handler
- void handle_keypress(char input);

// wait constant (1 sec = 1,000,000 us)
- #define WAIT_TIME_US 500000

- int row = 0        // indicate which row of keypad to output to
- int timer_mode = 0 // indicates current timer mode
- char user_input [] = "000" // stores user input to set timer
- char user_display [] = "m:ss" // stores display format for user input
- int user_input_length = 0 // stores number of user inputs for timer
- bool prompted = false // indicates whether prompt has be displayed yet
- unsigned int time_set = 0 // indicates time set (from user input) in seconds
- unsigned int time_passed = 0  // indicates time passed (seconds) since timer started

// create interrupts for each column wire on keypad
- InterruptIn col_A(PF_5, PullDown)
- InterruptIn col_B(PF_3, PullDown)
- InterruptIn col_C(PD_2, PullDown)
- InterruptIn col_D(PC_12, PullDown)

// create LCD object (need to figure out SDA and SCL pins)
- CSE321_LCD display(16, 2, LCD_5x8DOTS, PB_9, PB_8)

----------
API and Built In Elements Used
----------
- CSE321_LCD
- InterruptIn
- time
//included
- mbed.h
- 1802.h
- cctype
- cstdlib
- cstring
- ctime

----------
Custom Functions
----------
void create_timer():

	This function starts a loop that adds any number button presses to user_input (up to 3), and displays it on the LCD.
	
	Inputs:
		None
	Outputs:
		LCD
	Globally referenced things used:
		display, user_display, user_input, user_input_length, timer_mode, time_passed, prompted

void start_timer():

	This function starts a loop that decreases the timer set by the user by 1 every second until paused or it hits 0. If the timer hits 0, end_timer() is called.
	
	Inputs:
		None
	Outputs:
		LCD
	Globally referenced things used:
		display, time, timer_mode, prompted, time_passed, time_set

void pause_timer():

	This function stops the countdown loop until resumed or a new timer is set by switching 
	timer_mode to 3.
	
	Inputs:
		None
	Outputs:
		LCD
	Globally referenced things used:
		time, timer_mode

void end_timer():

	This function is called when timer hits 0. Outputs "Time's Up" to LCD. LEDs are left on (no action).
	
	Inputs:
		None
	Outputs:
		LCD
	Globally referenced things used:
		display, timer_mode, prompted
	
void handle_keypress(char input):

	This function is called in each interrupt, and handles the keypress. Depending on the current timer_mode,
	the function will call the appropriate function (ex: start_timer for "A" input), or add number presses to
	user_input if timer_mode is set to create timer.
	
	Inputs:
		char input
	Outputs:
		none
	Globally referenced things used:
		create_timer(), start_timer(), pause_timer(), user_input, user_input_length, user_display
