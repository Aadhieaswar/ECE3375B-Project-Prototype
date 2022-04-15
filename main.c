#include <time.h>

#define BUF_SIZE 80000 // about 10 seconds of buffer (@ 8K samples/sec)
#define BUF_THRESHOLD 96 // 75% of 128 word buffer

// prototypes
void spray(void);
void checkLock(void);
void checkPressure(void);
int getADC();
void setSprayIntensity(void);
void checkAudio();
void checkBtn(void);
void resetAudioBuffer(void);
int power(int, int);
void delay(int);

// enum
// enum for a boolean type
typedef enum { True, False } boolean;

// variables
volatile int * HEX1_ptr = (int *) 0xFF200020; // pointer to the last 4 seven-segment displays
volatile int * HEX2_ptr = (int *) 0xFF200030; // pointer to the first 2 seven segment displays

const volatile int * SW_ptr = (int *) 0xFF200040; // pointer to the flip switches
const volatile int * KEY_ptr = (int *) 0xFF200050; // pointer to the push buttons

int * LED_ptr = (int *) 0xFF200000; // pointer to the LEDs

volatile int * AUDIO_ptr = (int *) 0xFF203040; // pointer to the audio

// GPIO and units converter variables
int * ADC_BASE_ptr = (int *) 0xFF204000;
int * GPIO_BASE_ptr = (int *) 0xFF200060;

// variables for the logic of the code
boolean locked = False;
int sitting = 0;
int scale = 0;

// variables needed for audio I/O 
int fifospace;
int play_sound = 0, buffer_index = 0;
int left_buffer[BUF_SIZE];
int right_buffer[BUF_SIZE];
volatile int delay_count;
int DELAY_LENGTH = 10;
int soundCounter = 0;
double sounds[56] = { // used to store the sounds
	0,
	11088262851,
	22039774346,
	32719469680,
	42995636353,
	52741538578,
	61836980307,
	70169787615,
	77637192131,
	84147098481,
	89619220103,
	93986069415,
	97193790137,
	99202821505,
	99988386170,
	99540795776,
	97865570447,
	94983370710,
	90929742683,
	85754679693,
	79522005703,
	72308588174,
	64203390063,
	55306372638,
	45727262664,
	35584199141,
	25002276297,
	14112000806,
	3047682251,
	-8054223318,
	-19056796288,
	-29824342115,
	-40224064839,
	-50127704859,
	-59413120751,
	-67965795640,
	-75680249531,
	-82461340198,
	-88225436564,
	-92901450128,
	-96431711693,
	-98772682606,
	-99895491710,
	-99786291422,
	-98446428505,
	-95892427467,
	-92155786758,
	-87282590306,
	-81332939157,
	-74380210259,
	-66510151498,
	-57819824175,
	-48416405947,
	-38415869012,
	-27941549820,
	-17122627972,
};

// array with the hex for the seven segment display
const unsigned char hexDisplay[10] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66,
    0x6d, 0x7d, 0x07, 0x7f, 0x6f
};

// main method
int main(void) {
	// set the GPIO input and output pin config
	*(GPIO_BASE_ptr + 1) = 0x000003FF;

    // infinite loop to keep the program running
    while (1) {
		
		setSprayIntensity();

        *HEX1_ptr = 0;
		*HEX2_ptr = 0;

		checkPressure();
		
		checkAudio();
    }
}

// function that simulates the system spraying
void spray(void) {

    volatile int SPRAY_HEX2 = 0x6D73; // 'SP' display on the hex display
    volatile int SPRAY_HEX1 = 0x50776E00; // 'RAY' display on the hex display

	// add the intensity of the spray next to the text
	SPRAY_HEX1 += hexDisplay[scale];

    // set the display for the seven segment dispay
    *HEX1_ptr = SPRAY_HEX1;
    *HEX2_ptr = SPRAY_HEX2;

    delay(1);

	// clear the hex display
	*HEX1_ptr = 0x00;
    *HEX2_ptr = 0x00;
}

// function to simulate manual locking an unlocking of the door
void checkLock(void) {
    int SW1 = *SW_ptr & 0x01; // mask to ingore all the flip switches except the first one

    // turn on the LED based on the state of the flip switch
    if (SW1 == 1) {
		locked = True;
	} else {
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
		if(sitting == 1) {
			spray();
			sitting = 0;
		}
    }
}

int getADC() {
	// mask for the GPIO port (input of first 12 bits)
	int mask = 0x00000FFF;

	*(ADC_BASE_ptr) = 0; // write anything to channel 0 to update ADC

	// access the input from channel 0 using the mask
	volatile int channel0;
	channel0 = (*(ADC_BASE_ptr) & mask);

	return channel0;
}

// get the intensity of the febeeze spray (simulated)
void setSprayIntensity(void) {

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

	*(GPIO_BASE_ptr) = power(2, scale) - 1;

}

// plays the audio after it loads the audio buffer
void checkAudio() {
	checkBtn();
	
	if (play_sound) {
		fifospace = *(AUDIO_ptr + 1); // read the audio port fifospace register
		
		if ((fifospace & 0x00FF0000) > BUF_THRESHOLD) {
			// while loop to fill the sound
			while ((fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE)) {
				*(AUDIO_ptr + 2) = left_buffer[buffer_index];
				*(AUDIO_ptr + 3) = right_buffer[buffer_index];
				++buffer_index;
				
				if (buffer_index == BUF_SIZE) {
					// done playback
					play_sound = 0;	
				}
				
				fifospace = *(AUDIO_ptr + 1); // read the audio port fifospace register
			}
		}
	}
}

// method to check whether to play the sound or not
void checkBtn(void) {
	int key_switch = *KEY_ptr & 0x01;
	int sw = *SW_ptr & 0x01;
	
	while (*KEY_ptr || *SW_ptr)
	;
	
	if (key_switch == 1 || sw == 1) {
		
		resetAudioBuffer();
		
		buffer_index = 0;
		
		*(AUDIO_ptr) = 0x8;
		*(AUDIO_ptr) = 0x0;
		
		play_sound = 1;
	}
}

// method to fill the audio output buffer
void resetAudioBuffer(void) {
	buffer_index = 0;
	
	*(AUDIO_ptr) = 0x4;
	*(AUDIO_ptr) = 0x0;
	
	fifospace = *(AUDIO_ptr + 1); // read the audio port fifospace register
	if ((fifospace & 0x000000FF) > BUF_THRESHOLD)
	{
		// store audio into the buffer
		while ((fifospace & 0x000000FF) && (buffer_index < BUF_SIZE)) {
			left_buffer[buffer_index] = sounds[soundCounter];
			right_buffer[buffer_index] = sounds[soundCounter];

			++buffer_index;
			++soundCounter;
			
            for (delay_count = DELAY_LENGTH; delay_count != 0; --delay_count)
				; 

			// reset the sound counter when it reaches the end of the array
			if(soundCounter > 56){
				soundCounter = 0;
			}

			fifospace = *(AUDIO_ptr + 1); // read the audio port fifospace register
		}
	}
}

// function to get the power of a number
int power(int x, int y) {
    if(y == 0)
		return 1;
    return (x * power(x, y - 1) );
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
