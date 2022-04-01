#include <time.h>
#include "address_map_arm.h"

// prototypes
void spray(void);
void checkLock(void);
void checkPressure(void);
void delay(int);

// enum
// enum for a boolean type
typedef enum { True, False } boolean;

// variables
volatile int * HEX1_ptr = (int *) HEX3_HEX0_BASE; // pointer to the last 4 seven-segment displays
volatile int * HEX2_ptr = (int *) HEX5_HEX4_BASE; // pointer to the first 2 seven segment displays

const int * SW_ptr = (int *) SW_BASE; // pointer to the flip switches
const int * KEY_ptr = (int *) KEY_BASE; // pointer to the push buttons

int * LED_ptr = (int *) LED_BASE; // pointer to the LEDs

boolean sitting = True;

// main method
int main(void) {
    // infinite loop to keep the program running
    while (1) {
        checkLock();

        if (sitting == False)
            checkPressure();
    }
}

// function that simulates the system spraying
void spray(void) {

    const int SPRAY_HEX2 = 0x6D73; // 'SP' display on the hex display
    const int SPRAY_HEX1 = 0x50776E00; // 'RAY' display on the hex display

    // set the display for the seven segment dispay
    *HEX1_ptr = SPRAY_HEX1;
    *HEX2_ptr = SPRAY_HEX2;

    delay(3);

    sitting = False;
}

// function to simulate manual locking an unlocking of the door
void checkLock(void) {
    int SW1 = *SW_ptr & 0x01; // mask to ingore all the flip switches except the first one

    // turn on the LED based on the state of the flip switch
    if (SW1 == 1)
        *LED_ptr = 1;
    else
        *LED_ptr = 0;
}

// function to check the push button to simulate the pressure sensor
void checkPressure(void) {
    int KEY1 = *KEY_ptr & 0x01; // mask to ingore all the push buttons except the first one

    if (KEY1 == 1) {
        *LED_ptr = 1;
    } else {
        *LED_ptr = 0;

        if (sitting == True)
            spray();
    }
}

// function to delay the program
void delay(int seconds) {
    // Converting time into milli_seconds
    int ms = 1000 * seconds;

    // Storing start time
    clock_t start = clock();

    // looping till required time is not achieved
    while (clock() < start + ms)
        ;
}
