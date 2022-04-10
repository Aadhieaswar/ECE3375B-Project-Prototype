#include <time.h>
#include "address_map_arm.h"

// prototypes
void spray(void);
void checkLock(void);
void checkPressure(void);
int getADC();
int setColour(void);
int power(int, int);
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

// GPIO and units converter variables
int * ADC_BASE_ptr = (int *) ADC_BASE;
int * GPIO_BASE_ptr = (int *) JP1_BASE;

boolean locked = False;
int sitting = 0;
int scale = 0;

const unsigned char hexDisplay[10] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66,
    0x6d, 0x7d, 0x07, 0x7f, 0x6f
};

// main method
int main(void) {
	*(GPIO_BASE_ptr + 1) = 0x000003FF;
	
    // infinite loop to keep the program running
    while (1) {
		setColour();
		
        *HEX1_ptr = 0;
		*HEX2_ptr = 0;
		
		checkPressure();
    }
}

// function that simulates the system spraying
void spray(void) {

    volatile int SPRAY_HEX2 = 0x6D73; // 'SP' display on the hex display
    volatile int SPRAY_HEX1 = 0x50776E00; // 'RAY' display on the hex display
	
	SPRAY_HEX1 += hexDisplay[scale];

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

int getADC() {
	// mask for the GPIO port (input/output)
	int mask = 0x00000FFF;
	
	*(ADC_BASE_ptr) = 0; // write anything to channel 0 to update ADC
	*(ADC_BASE_ptr + 1) = 0;
	volatile int channel0, channel2;
	channel0 = (*(ADC_BASE_ptr) & mask);
	channel2 = (*(ADC_BASE_ptr +1) & mask);
	
	int key = *SW_ptr & 0x2;
	if (key == 0) {
		return channel0;
	}
	
	if (key == 2) {
		return channel2;
	}
}

// 
int setColour(void) {

	scale = 0;

	if (getADC() < 500 && getADC() > 0) {
		scale = 1;
	}
	if (getADC() > 500 && getADC() < 1000 ) {
		scale = 2;
	}
	if (getADC() > 1000 && getADC() < 1500 ) {
		scale = 3;
	}
	if (getADC() > 1500 && getADC() < 2000 ) {
		scale = 4;
	}
	if (getADC() > 2000 && getADC() < 2500 ) {
		scale = 5;
	}
	if (getADC() > 2500 && getADC() < 3000 ) {
		scale = 6;
	}
	if (getADC() > 3000 && getADC() < 3500 ) {
		scale = 7;
	}
	if (getADC() > 3500 && getADC() < 3750 ) {
		scale = 8;
	}
	if (getADC() > 3750 && getADC() < 4000 ) {
		scale = 9;
	}
	if (getADC() > 4000) {
		scale = 10;
	}
	
	*(GPIO_BASE_ptr) = power(2,scale)-1; //Making he LED's light up

}

//lights up the LED's, gets the scale from setColour
int power(int x, int y){
    if(y == 0)
		return 1;
    return (x * power(x,y-1) );
}
	
// function to delay the program
void delay(int seconds) {
    // Converting time into milli_seconds
    int ms = 200 * seconds;

    // Storing start time
    clock_t start = clock();

    // looping till required time is not achieved
    while (clock() < start + ms)
		;
}
	
	