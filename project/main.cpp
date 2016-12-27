#include "WProgram.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Sprite.h"
#include "sprites.h"
#include "Matrix.h"
//#include <Adafruit_NeoPixel.h>
//#include "functions.h"
#define ARM_MATH_CM4
#include <arm_math.h>
Matrix myLeds = Matrix(0, 2, 1, 4);
//TODO Figure out ISRs
//TODO create 8 bit pixel alphabet
//TODO create function with alphabet to print letters.
/*
   ISR(TIMER0_OVF_vect){
   digitalWriteFast(13, HIGH);
   delay(500);
//  Timer 0 overflow
}
 */
////////////////////////////////////////////////////////////////////////////////
// CONIFIGURATION 
// These values can be changed to alter the behavior of the spectrum display.
////////////////////////////////////////////////////////////////////////////////

int SAMPLE_RATE_HZ = 9000;             // Sample rate of the audio in hertz.
float SPECTRUM_MIN_DB = 30.0;          // Audio intensity (in decibels) that maps to low LED brightness.
float SPECTRUM_MAX_DB = 60.0;          // Audio intensity (in decibels) that maps to high LED brightness.
int LEDS_ENABLED = 1;                  // Control if the LED's should display the spectrum or not.  1 is true, 0 is false.
// Useful for turning the LED display on and off with commands from the serial port.
const int FFT_SIZE = 256;              // Size of the FFT.  Realistically can only be at most 256 
// without running out of memory for buffers and other state.
const int AUDIO_INPUT_PIN = 14;        // Input ADC pin for audio data.
const int ANALOG_READ_RESOLUTION = 10; // Bits of resolution for the ADC.
const int ANALOG_READ_AVERAGING = 16;  // Number of samples to average with each ADC reading.
const int POWER_LED_PIN = 13;          // Output pin for power LED (pin 13 to use Teensy 3.0's onboard LED).
const int NEO_PIXEL_PIN = 3;           // Output pin for neo pixels.
const int NEO_PIXEL_COUNT = 4;         // Number of neo pixels.  You should be able to increase this without
// any other changes to the program.
const int MAX_CHARS = 65;              // Max size of the input command buffer

////////////////////////////////////////////////////////////////////////////////
// INTERNAL STATE
// These shouldn't be modified unless you know what you're doing.
////////////////////////////////////////////////////////////////////////////////

IntervalTimer samplingTimer;
float samples[FFT_SIZE*2];
float magnitudes[FFT_SIZE];
int sampleCounter = 0;
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NEO_PIXEL_COUNT, NEO_PIXEL_PIN, NEO_GRB + NEO_KHZ800);
char commandBuffer[MAX_CHARS];
float frequencyWindow[NEO_PIXEL_COUNT+1];
float hues[NEO_PIXEL_COUNT];


////////////////////////////////////////////////////////////////////////////////
// SAMPLING FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void samplingCallback() {
	// Read from the ADC and store the sample data
	samples[sampleCounter] = (float32_t)analogRead(AUDIO_INPUT_PIN);
	// Complex FFT functions require a coefficient for the imaginary part of the input.
	// Since we only have real data, set this coefficient to zero.
	samples[sampleCounter+1] = 0.0;
	// Update sample buffer position and stop after the buffer is filled
	sampleCounter += 2;
	if (sampleCounter >= FFT_SIZE*2) {
		samplingTimer.end();
	}
}

void samplingBegin() {
	// Reset sample buffer position and start callback at necessary rate.
	sampleCounter = 0;
	samplingTimer.begin(samplingCallback, 1000000/SAMPLE_RATE_HZ);
}

boolean samplingIsDone() {
	return sampleCounter >= FFT_SIZE*2;
}


void spectrumSetup() {
	// Set the frequency window values by evenly dividing the possible frequency
	// spectrum across the number of neo pixels.
	float windowSize = (SAMPLE_RATE_HZ / 2.0) / float(NEO_PIXEL_COUNT);
	for (int i = 0; i < NEO_PIXEL_COUNT+1; ++i) {
		frequencyWindow[i] = i*windowSize;
	}
	// Evenly spread hues across all pixels.
	for (int i = 0; i < NEO_PIXEL_COUNT; ++i) {
		hues[i] = 360.0*(float(i)/float(NEO_PIXEL_COUNT-1));
	}
}

void setup() {
	//LED matrix crap
	myLeds.clear();
	myLeds.setBrightness(1); //value range 0-15 zero is still shows value

	//Microphone stuff
	Serial.begin(38400);

	// Set up ADC and audio input.
	pinMode(AUDIO_INPUT_PIN, INPUT);
	analogReadResolution(ANALOG_READ_RESOLUTION);
	analogReadAveraging(ANALOG_READ_AVERAGING);

	// Turn on the power indicator LED.
	pinMode(POWER_LED_PIN, OUTPUT);
	digitalWrite(POWER_LED_PIN, HIGH);

	// Initialize neo pixel library and turn off the LEDs
	//pixels.begin();
	//pixels.show(); 

	// Clear the input command buffer
	memset(commandBuffer, 0, sizeof(commandBuffer));

	// Initialize spectrum display
	spectrumSetup();

	// Begin sampling audio
	samplingBegin();

}
void daft(){
	myLeds.write(0, 0, letter_D);
	myLeds.write(9, 0, letter_A);
	myLeds.write(17, 0, letter_F);
	myLeds.write(25, 0, letter_T);
}
void punk(){
	myLeds.write(0, 0, letter_P);
	myLeds.write(9, 0, letter_U);
	myLeds.write(16, 0, letter_N);
	myLeds.write(24, 0, letter_K);
}
int main(void)
{
	setup();
	sei();
	pinMode(13, OUTPUT);
	while (1) {
		myLeds.clear(); // clear display
		daft();
		digitalWriteFast(13, HIGH);
		delay(1000);
		punk();
		digitalWriteFast(13, LOW);
		delay(1000);
	}


}
