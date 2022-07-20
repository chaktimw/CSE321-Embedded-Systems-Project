/**
 * Author: Chaktim Wong
 * ubit: chaktimw
 * person#: 50280143
 * 
 * Description: 
 *      A temperature and humidity alarm system that uses a DHT11 sensor with an LCD display  
 *      and a vibrating motor as the alarm. When the temperature or humidity near the sensor 
 *      exceeds the chosen thresholds, the alarm activates until the temperature or humidity
 *      falls back below the chosen thresholds. Alarm code will loop until shutdown by user.  
 *
 * Modules: 
 *      1802.cpp, 1802.h, mbed.h, DHT.h, DHT.cpp
 *
 * Assignment: Project 3
 *
 * Inputs: DHT11 sensor
 *
 * Outputs: LCD display, vibration motor
 *
 * Constraints: 
 •	-	Temperature can be in degrees Fahrenheit or degrees Celsius.
 *  -	The user can press BUTTON1 on the Nucleo to change the unit of measurement for temperature (Fahrenheit or Celsius).
 *  -	Humidity can go from zero to one hundred percent.
 *  -	The threshold at which the vibrating motor turns must be set in degrees Fahrenheit for temperature and a percentage for humidity.
 *
 •	Specifications
 *  -	The LCD will display “Temp(F): “ or “Temp(C): “ followed by the current temperature on the first line.
 *  -	The LCD will display “Humidity: ” followed by the current humidity on the second line.
 *  -	Whenever the surrounding temperature or humidity changes, the LCD is updated with the new information. 
 *  -	The vibrating motor turns on when the temperature or humidity exceeds the chosen threshold, and turns off otherwise.
 *  -	Must run “forever”.
 •	
 *
 * References: 
 *  - STM32L4+ Series User Manual: https://www.st.com/resource/en/reference_manual/dm00310109-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
 *  - Datasheet for the Mouser LCD Model: https://learn-us-east-1-prod-fleet02-xythos.content.blackboardcdn.com/5e00ea752296c/10911180?X-Blackboard-Expiration=1635660000000&X-Blackboard-Signature=KASCrt%2FhdHjf1tiGizJpF5TgwEln6wCDgPYO7FqTq3s%3D&X-Blackboard-Client-Id=100310&response-cache-control=private%2C%20max-age%3D21600&response-content-disposition=inline%3B%20filename%2A%3DUTF-8%27%27JDH_1804_Datasheet.pdf&response-content-type=application%2Fpdf&X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Date=20211031T000000Z&X-Amz-SignedHeaders=host&X-Amz-Expires=21600&X-Amz-Credential=AKIAZH6WM4PL5SJBSTP6%2F20211031%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Signature=4d5ef6bbf416f1f2037ced29da4a121d97a1dc6b3fdf9e64f3a60ba042a28511
 *  - Datasheet for DHT11 Temperature Sensor: https://components101.com/sites/default/files/component_datasheet/DHT11-Temperature-Sensor.pdf
 *  - Datasheet for vibration motor: http://www.learningaboutelectronics.com/Datasheets/Precision-Microdrives-Vibration-Motor-Datasheet.pdf
 *  - Watchdog reference: https://os.mbed.com/docs/mbed-os/v6.15/apis/watchdog.html
 *  - Eventqueue reference: https://os.mbed.com/docs/mbed-os/v6.15/apis/eventqueue.html
 *  - Thread reference: https://os.mbed.com/docs/mbed-os/v6.15/apis/thread.html
 */

#include "mbed.h"
#include "1802.h"
#include "DHT.h"
#include <stdio.h>

// maximum values that when exceeded will trigger alarm (vibration motor)
#define MAX_TEMP 72
#define MAX_HUMIDITY 60

// wait constant (1 sec = 1,000,000 us)
#define WAIT_TIME_US 1000000  

// declare callback functions
void isr_temp(void);

// declare event functions
void updateSensor();
void updateDisplay();
void checkAlarm();
void changeUnit();

// declare critical variables;
int tempUnit = 0;               // controls current unit of measurement (0=F, 1=C)
double tempF;                   // temp from sensor in Fahrenheit
double tempC;                   // temp from sensor in Celsius
int humidity;                   // humidity from sensor (%)

// Interrupt changes displayed temperature unit 
InterruptIn button(BUTTON1);    // attached to BUTTON1 on Nucleo

// create LCD object (SDA=PB_9 and SCL=PB_8)
CSE321_LCD display(16, 2, LCD_5x8DOTS, PB_9, PB_8);

// create DHT11 object
DHT11 sensor(PC_8); 

// Eventqueue
EventQueue e(32 * EVENTS_EVENT_SIZE);

// Threads
Thread thread1;
Thread thread2;
Thread thread3;

// declare thread function
void DHTsensor();
void displayUpdater();
void alarm();

// watchdog timeout variable
const uint32_t TIMEOUT_MS = 5000;

// watchdog trigger (from watchdog API example)
void trigger()
{
    Watchdog::get_instance().kick();
}

int main() {
    printf("starting main...\n");

    // initialize the display
    display.begin();

    // enable clock on port C
    RCC->AHB2ENR |= 0x4;
    // enable output for port C pin 9
    GPIOC->MODER |= 0x40000;
    GPIOC->MODER &= ~(0xB0000);

    // assign callback function for BUTTON1
    button.rise(&isr_temp);

    // wake up the watchdog (code from watchdog API example code)
    Watchdog &watchdog = Watchdog::get_instance();
    watchdog.start(TIMEOUT_MS);
    uint32_t watchdog_timeout = watchdog.get_timeout();
    printf("Watchdog initialized to %u ms.\r\n", watchdog_timeout);
    // dog being fed in updateDisplay()

    // start threads
    thread1.start(DHTsensor);
    thread2.start(displayUpdater);
    thread3.start(alarm);

    // start eventqueue
    e.dispatch_forever();

    while (1) {
    }
  return 0; }

// button1 interrupt
void isr_temp(void) {
    e.call(changeUnit);
}

// DHTsensor thread function
void DHTsensor(){
    while(1){
        // add event to eventqueue
        e.call(updateSensor);

        // sleep
        ThisThread::sleep_for(1s);
    }
}

// display thread function
void displayUpdater(){
    while(1){
        // add event to eventqueue
        e.call(updateDisplay);

        // sleep
        ThisThread::sleep_for(1s);
    }
}

// alarm thread function
void alarm(){
    while(1){
        // add event to eventqueue
        e.call(checkAlarm);

        // sleep
        ThisThread::sleep_for(1s);
    }
}

/**
 *
 * void changeUnit()
 *
 * Paramters    : None
 * 
 * Return Value : None
 *
 * Description:
 *
 *      This function updates the temperature unit.
 *
 */
void changeUnit(){
    if (tempUnit == 0) {
        printf("Temperature unit is now Celsius.\n");
        tempUnit = 1;
    }else{
        printf("Temperature unit is now Fahrenheit.\n");
        tempUnit = 0;
    }
}

/**
 *
 * void updateSensor()
 *
 * Paramters    : None
 * 
 * Return Value : None
 *
 * Description:
 *
 *      This function updates the temperature and humidity data variables with data read from the DHT11 sensor.
 *
 */
void updateSensor(){
    // read humidity sensor
    sensor.read();                          
    tempF = sensor.getFahrenheit();  // get temp from sensor
    tempC = sensor.getCelsius();     // get temp from sensor
    humidity = sensor.getHumidity();    // get humidity from sensor
    // printf("T: %f, H: %d\r\n", tempF, humidity);
}

/**
 *
 * void updateDisplay()
 *
 * Paramters    : None
 * 
 * Return Value : None
 *
 * Description:
 *
 *      This function updates the LCD display with the temperature and humidity data variables.
 *
 */
void updateDisplay() {
    // printf("updating display...\n");

    // clear display
    display.clear();                
    
    // print temperature based on currently selected unit of measure
    char line1[10];
    if (tempUnit == 0) {
        // Fahrenheit
        sprintf(line1, "%f", tempF);    // convert to char* for display on LCD
        display.print("Temp(F):   ");
        display.print(line1);  
        printf("T(F): %f, H: %d\r\n", tempF, humidity);
    }else{
        // Celsius
        sprintf(line1, "%f", tempC);    // convert to char* for display on LCD
        display.print("Temp(C):   ");
        display.print(line1);
        printf("T(C): %f, H: %d\r\n", tempC, humidity);
    }
    
    // print humidity to display 
    char line2[10];       
    display.setCursor(0, 1);         // switch to row 2   
    sprintf(line2, "%d", humidity);  // convert to char* for display on LCD
    display.print("Humidity:   ");     
    display.print(line2);
    display.print("%");

    // feed the dog
        trigger();
}

/**
 *
 * void checkAlarm()
 *
 * Paramters    : None
 * 
 * Return Value : None
 *
 * Description:
 *
 *      This function checks if temp or humidity thresholds have been exceeded, 
 *      and if they have been, turns on the pin associated with the vibration motor
 *      otherwise, it turns off the associated pin
 *
 */
void checkAlarm(){
    // printf("checking alarm...\n");

    // check thresholds
    if (tempF > MAX_TEMP || humidity > MAX_HUMIDITY) {
        // turn on pin associated with vibration motor
        GPIOC->ODR |= 0x200;  // output to PC9
    }else{
        // turn off pin associated with vibration motor
        GPIOC->ODR &= ~(0x200);  // stop output to PC9
    }
}
