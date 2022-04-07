#include <time.h>


// prototypes
void spray(void);
void checkLock(void);
void checkPressure(void);
void delay(int);

// enum
// enum for a boolean type
typedef enum { True, False } boolean;

// variables
volatile int * HEX1_ptr = (int *) 0xFF200020; // pointer to the last 4 seven-segment displays
volatile int * HEX2_ptr = (int *) 0xFF200030; // pointer to the first 2 seven segment displays

const int * SW_ptr = (int *) 0xFF200040; // pointer to the flip switches
const int * KEY_ptr = (int *) 0xFF200050; // pointer to the push buttons


int * LED_ptr = (int *) 0xFF200000; // pointer to the LEDs

boolean locked = False;
int sitting = 0;

// main method
int main(void) {
    // infinite loop to keep the program running
    while (1) {
        *HEX1_ptr = 0;
		*HEX2_ptr = 0;
		checkPressure();
    }
}

// function that simulates the system spraying
void spray(void) {

    volatile int SPRAY_HEX2 = 0x6D73; // 'SP' display on the hex display
    volatile int SPRAY_HEX1 = 0x50776E00; // 'RAY' display on the hex display

    // set the display for the seven segment dispay
    *HEX1_ptr = SPRAY_HEX1;
    *HEX2_ptr = SPRAY_HEX2;

    delay(1);
	
	SPRAY_HEX2 = 0; // 'SP' display on the hex display
    SPRAY_HEX1 = 0; // 'RAY' display on the hex display
	
	*HEX1_ptr = SPRAY_HEX1;
    *HEX2_ptr = SPRAY_HEX2;
    
}

// function to simulate manual locking an unlocking of the door
void checkLock(void) {
    int SW1 = *SW_ptr & 0x01; // mask to ingore all the flip switches except the first one

    // turn on the LED based on the state of the flip switch
    if (SW1 == 1)
	{
		locked = True;
	}   
    else
	{
		locked = False;
	}
        
}

// function to check the push button to simulate the pressure sensor
void checkPressure(void) {
    int KEY1 = *KEY_ptr & 0x01; // mask to ingore all the push buttons except the first one
	checkLock();

    if (KEY1 == 1 || locked == True) {
        *LED_ptr = 1;
		sitting = 1;
    } else {
        *LED_ptr = 0;
		if(sitting == 1)
		{
			spray();
			sitting = 0;
		}
		
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
	
	