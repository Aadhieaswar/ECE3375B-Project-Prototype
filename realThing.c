#include <time.h>
	
/* globals */
#define BUF_SIZE 80000 // about 10 seconds of buffer (@ 8K samples/sec)
#define BUF_THRESHOLD 96 // 75% of 128 word buffer
	
	
/* function prototypes */
void check_KEYs(int *, int *, int *);
void spray(void);
void checkLock(void);
void checkPressure(void);
int getADC();
int setColour(void);
int power(int, int);
void delay(int);

// enum for a boolean type
typedef enum { True, False } boolean;

boolean locked = False;
int scale = 0;
int sitting = 0;

volatile int * HEX1_ptr = (int *) 0xFF200020; // pointer to the last 4 seven-segment displays
volatile int * HEX2_ptr = (int *) 0xFF200030; // pointer to the first 2 seven segment displays
volatile int * red_LED_ptr = (int *)0xFF200000;
const int * SW_ptr = (int *) 0xFF200040; // pointer to the flip switches
const int * KEY_ptr = (int *) 0xFF200050; // pointer to the push buttons

const unsigned char hexDisplay[10] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66,
    0x6d, 0x7d, 0x07, 0x7f, 0x6f
};

int main(void) {
	
	volatile int * audio_ptr = (int *)0xFF203040;
	volatile int * HEX1_ptr = (int *) 0xFF200020; // pointer to the last 4 seven-segment displays
	volatile int * HEX2_ptr = (int *) 0xFF200030; // pointer to the first 2 seven segment displays
	/* used for audio record/playback */
	int fifospace;
	int record = 0, play = 0, buffer_index = 0;
	int left_buffer[BUF_SIZE];
	int right_buffer[BUF_SIZE];
	/* read and echo audio data */
	record = 0;
	play = 0;
	
	while (1) {
		checkPressure();
		check_KEYs(&record, &play, &buffer_index);

		if (play) {
			*(red_LED_ptr) = 0x2; // turn on LEDR_1
			fifospace = *(audio_ptr + 1); // read the audio port fifospace register
			
			if ((fifospace & 0x00FF0000) > BUF_THRESHOLD) // check WSRC
			{
				// output data until the buffer is empty or the audio-out FIFO
				// is full
				while ((fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE)) {
					*(audio_ptr + 2) = left_buffer[buffer_index];
					*(audio_ptr + 3) = right_buffer[buffer_index];
					++buffer_index;
					
					if (buffer_index == BUF_SIZE) {
						// done playback
						play = 0;
						*(red_LED_ptr) = 0x0; // turn off LEDR
					}
					fifospace = *(audio_ptr +1); // read the audio port fifospace register
				}
			}
		}
	}
}

void check_KEYs(int * KEY0, int * KEY1, int * counter) {
	volatile int * KEY_ptr = (int *)0xFF200050;
	volatile int * audio_ptr = (int *)0xFF203040;
	int KEY_value;
	KEY_value = *(KEY_ptr); // read the pushbutton KEY values
	
	while (*KEY_ptr)
		; // wait for pushbutton KEY release
	if (KEY_value == 0x1){
			// reset counter to start playback
			*counter = 0;
			// clear audio-out FIFO
			*(audio_ptr) = 0x8;
			*(audio_ptr) = 0x0;
			*KEY1 = 1;
			spray();
	}
}

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
        *red_LED_ptr = 1;
		sitting = 1;
    } else {
        *red_LED_ptr = 0;
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
    int ms = 200 * seconds;

    // Storing start time
    clock_t start = clock();

    // looping till required time is not achieved
    while (clock() < start + ms);
}
	
	