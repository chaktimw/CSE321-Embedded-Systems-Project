-------------------
About
-------------------
Project Description: 
  A temperature and humidity alarm system that uses a DHT11 sensor with an LCD display and a vibrating motor as the alarm. When the temperature or humidity near the sensor 
 exceeds the chosen thresholds, the alarm activates until the temperature or humidity falls back below the chosen thresholds. Alarm code will loop until shutdown by user.
 
Contributor List: 
  Chaktim Wong

--------------------
Features
--------------------
- Alarm system using a vibration motor
- LCD displays the current temperature and humidity of the surrounding area
- Temperature can be displayed in either Fahrenheit or Celsius by the press of a button.

--------------------
Required Materials
--------------------
- DHT11 (Temperature and humidity sensor)
- Vibration motor (for the alarm)
- Solderless Breadboard (to form connections)
- Jumper Wires (at least 7 for convenience)
- USB a to Micro USB B cable (connects the Nucleo to the computer)
- 16x2 LCDs with I2C for Arduinos and Raspberry PI, 1602 or 1802 model with the I2C element soldered on

--------------------
Resources and References
--------------------
- STM32L4+ Series User Manual: https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
- Datasheet for the Mouser LCD Model: https://learn-us-east-1-prod-fleet02-xythos.content.blackboardcdn.com/5e00ea752296c/10911180?X-Blackboard-Expiration=1635660000000&X-Blackboard-Signature=KASCrt%2FhdHjf1tiGizJpF5TgwEln6wCDgPYO7FqTq3s%3D&X-Blackboard-Client-Id=100310&response-cache-control=private%2C%20max-age%3D21600&response-content-disposition=inline%3B%20filename%2A%3DUTF-8%27%27JDH_1804_Datasheet.pdf&response-content-type=application%2Fpdf&X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Date=20211031T000000Z&X-Amz-SignedHeaders=host&X-Amz-Expires=21600&X-Amz-Credential=AKIAZH6WM4PL5SJBSTP6%2F20211031%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Signature=4d5ef6bbf416f1f2037ced29da4a121d97a1dc6b3fdf9e64f3a60ba042a28511
- Datasheet for DHT11 Temperature Sensor: https://components101.com/sites/default/files/component_datasheet/DHT11-Temperature-Sensor.pdf
- Datasheet for vibration motor: http://www.learningaboutelectronics.com/Datasheets/Precision-Microdrives-Vibration-Motor-Datasheet.pdf
- Watchdog reference: https://os.mbed.com/docs/mbed-os/v6.15/apis/watchdog.html
- Eventqueue reference: https://os.mbed.com/docs/mbed-os/v6.15/apis/eventqueue.html
- Thread reference: https://os.mbed.com/docs/mbed-os/v6.15/apis/thread.html

--------------------
Getting Started
--------------------
1) Connect the LCD ports to PB_8 (SCL), PB_9 (SDA), 3V (VCC), and GND (GND) on the Nucleo.
2) Connect the DHT11 sensor ports to PC_8 (out), 5V (+), and GND (-) on the Nucleo.
3) Connect the breadboard’s negative column to a GND pin on the Nucleo.
4) Connect PC_9 on the Nucleo to a row on the breadboard.
5) Connect the positive pin of the vibration motor to a hole in the same row as PC_9 on the breadboard.
6) Connect the negative pin of the vibration motor to the negative column of the breadboard which is connected to a GND pin on the Nucleo.
7) Remember to secure the vibration motor using tape or stick the adhesive side of the motor to the breadboard so that the wires don’t pull out when it vibrates.

--------------------
main.cpp:
--------------------
  This file contains the code necessary for the temperature and humidity alarm system to function. The code runs using a main loop and three other threads each managing
a speparate pheripheral (DHT11 sensor, LCD display, and the vibration motor). The main thread handles the eventqueue and the other threads adds events to this queue.
The button on the Nucleo is programmed to interrupt these threads and add an event to the queue which edits one of the data variables. All the threads run while true loop
and sleep for the designated time after it adds an event to the eventqueue. A watchdog also handles any errors that may halt the system. 

----------
Things Declared
----------
*all semicolons have been excluded for readability*

// declared callback functions
- void isr_temp(void)

// declared event functions
- void updateSensor()
- void updateDisplay()
- void checkAlarm()
- void changeUnit()

// declared critical variables;
- int tempUnit = 0               // controls current unit of measurement (0=F, 1=C)
- double tempF                   // temp from sensor in Fahrenheit
- double tempC                   // temp from sensor in Celsius
- int humidity                   // humidity from sensor (%)

// wait constant (1 sec = 1,000,000 us)
- #define WAIT_TIME_US 1000000

// Interrupt 
- InterruptIn button(BUTTON1)    // attached to BUTTON1 on Nucleo

// LCD object (SDA=PB_9 and SCL=PB_8)
- CSE321_LCD display(16, 2, LCD_5x8DOTS, PB_9, PB_8)

// DHT11 object
- DHT11 sensor(PC_8)

// Eventqueue
- EventQueue e(32 * EVENTS_EVENT_SIZE)

// Threads
- Thread thread1
- Thread thread2
- Thread thread3

// declared thread functions
- void DHTsensor()
- void displayUpdater()
- void alarm()

// watchdog timeout variable
- const uint32_t TIMEOUT_MS = 5000

----------
API and Built In Elements Used
----------
- CSE321_LCD
- InterruptIn
- DHT11
- EventQueue
- Thread
- Watchdog

//included
- mbed.h
- 1802.h
- DHT.h
- <stdio.h>

----------
Custom Functions
----------
void changeUnit():

	This function updates the temperature unit.
	
	Inputs:
		None
	Outputs:
		None
	Globally referenced things used:
		tempUnit

void updateSensor():

	This function updates the temperature and humidity data variables with data read from the DHT11 sensor.
	
	Inputs:
		DHT11 sensor
	Outputs:
		None
	Globally referenced things used:
		sensor, tempF, tempC, humidity

void updateDisplay():

	This function updates the LCD display with the temperature and humidity data variables.
	
	Inputs:
		None
	Outputs:
		LCD
	Globally referenced things used:
		display, tempUnit, tempF, tempC, humidity, watchdog

void checkAlarm():

	This function checks if temp or humidity thresholds have been exceeded, 
 and if they have been, turns on the pin associated with the vibration motor
 otherwise, it turns off the associated pin
	
	Inputs:
		None
	Outputs:
		vibration motor
	Globally referenced things used:
		tempF, humidity, MAX_TEMP, MAX_HUMIDITY
