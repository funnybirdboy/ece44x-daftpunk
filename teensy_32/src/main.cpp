#include "WProgram.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Sprite.h"
#include "sprites.h"
#include "Matrix.h"
//TODO delete duplicate funtions and move to external file
//#include "functions.h"
#define ARM_MATH_CM4	//this is processor specific
#include "arm_math.h"

////////////////////////////////////////////////////////////////////////////////
// CONIFIGURATION
// These values can be changed to alter the behavior of the spectrum display.
////////////////////////////////////////////////////////////////////////////////

int SAMPLE_RATE_HZ = 9000;             // Sample rate of the audio in hertz.
float SPECTRUM_MIN_DB = 30.0;          // Audio intensity (in decibels) that maps to low LED brightness.
float SPECTRUM_MAX_DB = 60.0;          // Audio intensity (in decibels) that maps to high LED brightness.
int LEDS_ENABLED = 1;                  // Control if the LED's should display the spectrum or not.  1 is true, 0 is false.
int MODE = 0; 							// Sets the mode default is 0
// Useful for turning the LED display on and off with commands from the serial port.
const int FFT_SIZE = 256;              // Size of the FFT.  Realistically can only be at most 256
// without running out of memory for buffers and other state.
const int AUDIO_INPUT_PIN = 14;        // Input ADC pin for audio data.
const int ANALOG_READ_RESOLUTION = 10; // Bits of resolution for the ADC.
const int ANALOG_READ_AVERAGING = 16;  // Number of samples to average with each ADC reading.
const int POWER_LED_PIN = 13;          // Output pin for power LED (pin 13 to use Teensy 3.0's onboard LED).
//const int MATRIX_PIN = 3;           // Output pin for matrix D_out.
const int BUTTON_PIN = 15;				//Input pin for button commands
const int MATRIX_WIDTH = 48;         // Number of ledss.  You should be able to increase this without
// any other changes to the program.
const int MAX_CHARS = 65;              // Max size of the input command buffer
int display_matrix[24][16] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

////////////////////////////////////////////////////////////////////////////////
// INTERNAL STATE
// These shouldn't be modified unless you know what you're doing.
////////////////////////////////////////////////////////////////////////////////

IntervalTimer samplingTimer;
float samples[FFT_SIZE*2];
float magnitudes[FFT_SIZE];
int sampleCounter = 0;
char commandBuffer[MAX_CHARS];
float frequencyWindow[MATRIX_WIDTH+1];
float hues[MATRIX_WIDTH];


////////////////////////////////////////////////////////////////////////////////
// MAIN SKETCH FUNCTIONS
////////////////////////////////////////////////////////////////////////////////
//data, load, clk, quantity
Matrix myLeds = Matrix(0, 2, 1, 6);

////////////////////////////////////////////////////////////////////////////////
// UTILITY FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

//*******************************************************************************
//                            debounce_switch
// Adapted from Ganssel's "Guide to Debouncing"
// Checks the state of pushbutton S0 It shifts in ones till the button is pushed.
// Function returns a 1 only once per debounced button push so a debounce and toggle
// function can be implemented at the same time.  Expects active low pushbutton on
// Port D bit zero.  Debounce time is determined by external loop delay times 12.
//*******************************************************************************
int debounce_switch() {
	static uint16_t state = 0; //holds present state
	state = (state << 1) | digitalRead(BUTTON_PIN) | 0xE000;
	if (state == 0xF000) return 1;
	return 0;
}
// Compute the average magnitude of a target frequency window vs. all other frequencies.

void display_test(){
	int temp_i, temp_j;
	for (int i = 0; i < 48; i++) {
		for(int j =0; j<8; j++){
			//if greater than 8 in next column so add 24(?) to y and subtract 8 from x
			myLeds.write(i, j, HIGH);
		}
	}

}
void windowMean(float* magnitudes, int lowBin, int highBin, float* windowMean, float* otherMean) {
	*windowMean = 0;
	*otherMean = 0;
	// Notice the first magnitude bin is skipped because it represents the
	// average power of the signal.
	for (int i = 1; i < FFT_SIZE/2; ++i) {
		if (i >= lowBin && i <= highBin) {
			*windowMean += magnitudes[i];
		}
		else {
			*otherMean += magnitudes[i];
		}
	}
	*windowMean /= (highBin - lowBin) + 1;
	*otherMean /= (FFT_SIZE / 2 - (highBin - lowBin));
}

// Convert a frequency to the appropriate FFT bin it will fall within.
int frequencyToBin(float frequency) {
	float binFrequency = float(SAMPLE_RATE_HZ) / float(FFT_SIZE);
	return int(frequency / binFrequency);
}

////////////////////////////////////////////////////////////////////////////////
// SPECTRUM DISPLAY FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

void spectrumSetup() {
	// Set the frequency window values by evenly dividing the possible frequency
	// spectrum across the number leds.
	float windowSize = (SAMPLE_RATE_HZ / 2.0) / float(MATRIX_WIDTH);
	for (int i = 0; i < MATRIX_WIDTH+1; ++i) {
		frequencyWindow[i] = i*windowSize;
	}
	// Evenly spread hues across all pixels.
	for (int i = 0; i < MATRIX_WIDTH; ++i) {
		hues[i] = 360.0*(float(i)/float(MATRIX_WIDTH-1));
	}
}

void volLoop() {
	// Update each LED based on the intensity of the audio
	// in the associated frequency window.
	display_test(); //write all leds
	//TODO use sprites to make faster
	float intensity, otherMean;
	float max = 0;
	float min = 1;
	//  myLeds.setBrightness(1); //TODO make variable
	for (int i = 0; i < MATRIX_WIDTH; ++i) {
		windowMean(magnitudes,
				frequencyToBin(frequencyWindow[i]),
				frequencyToBin(frequencyWindow[i+1]),
				&intensity,
				&otherMean);
		// Convert intensity to decibels.
		intensity = 20.0*log10(intensity);
		// Scale the intensity and clamp between 0 and 1.0.
		intensity -= SPECTRUM_MIN_DB;
		intensity = intensity < 0.0 ? 0.0 : intensity;
		intensity /= (SPECTRUM_MAX_DB-SPECTRUM_MIN_DB);
		intensity = intensity > 1.0 ? 1.0 : intensity;
		//myLeds.clear();
		if (intensity > max){max = intensity;}
		if (intensity < min){min = intensity;}
	}
	myLeds.setBrightness(int(((max-min)/2)*15)); 

}
void fftLoop() {
	// Update each LED based on the intensity of the audio
	// in the associated frequency window.
	float intensity, otherMean;
	float max = 0;
	float min = 1;
	//  myLeds.setBrightness(1); //TODO make variable
	for (int i = 0; i < MATRIX_WIDTH; ++i) {
		windowMean(magnitudes,
				frequencyToBin(frequencyWindow[i]),
				frequencyToBin(frequencyWindow[i+1]),
				&intensity,
				&otherMean);
		// Convert intensity to decibels.
		intensity = 20.0*log10(intensity);
		// Scale the intensity and clamp between 0 and 1.0.
		intensity -= SPECTRUM_MIN_DB;
		intensity = intensity < 0.0 ? 0.0 : intensity;
		intensity /= (SPECTRUM_MAX_DB-SPECTRUM_MIN_DB);
		intensity = intensity > 1.0 ? 1.0 : intensity;
		myLeds.clear();
		if (intensity > max){max = intensity;}
		if (intensity < min){min = intensity;}
		for(int j =0; j<int(intensity*7); j++){
			//TODO change the hight based on the intenisty
			myLeds.write(i, j, HIGH);
		}
	}
	//myLeds.setBrightness(int(((max-min)/2)*15)); 

}
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



void setup() {
	//LED matrix crap
	myLeds.clear();
	myLeds.setBrightness(15); //value range 0-15 zero is still shows value

	//Microphone stuff
	Serial.begin(38400);

	// Set up ADC and audio input.
	pinMode(AUDIO_INPUT_PIN, INPUT);
	analogReadResolution(ANALOG_READ_RESOLUTION);
	analogReadAveraging(ANALOG_READ_AVERAGING);

	pinMode(BUTTON_PIN, INPUT_PULLUP);	//button pin

	// Turn on the power indicator LED.
	pinMode(POWER_LED_PIN, OUTPUT);
	digitalWrite(POWER_LED_PIN, HIGH);

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

////////////////////////////////////////////////////////////////////////////////
// COMMAND PARSING FUNCTIONS
// These functions allow parsing simple commands input on the serial port.
// Commands allow reading and writing variables that control the device.
//
// All commands must end with a semicolon character.
//
// Example commands are:
// GET SAMPLE_RATE_HZ;
// - Get the sample rate of the device.
// SET SAMPLE_RATE_HZ 400;
// - Set the sample rate of the device to 400 hertz.
//
////////////////////////////////////////////////////////////////////////////////

// Macro used in parseCommand function to simplify parsing get and set commands for a variable
#define GET_AND_SET(variableName) \
	else if (strcmp(command, "GET " #variableName) == 0) { \
		Serial.println(variableName); \
	} \
else if (strstr(command, "SET " #variableName " ") != NULL) { \
	variableName = (typeof(variableName)) atof(command+(sizeof("SET " #variableName " ")-1)); \
}

void parseCommand(char* command) {
	if (strcmp(command, "GET MAGNITUDES") == 0) {
		for (int i = 0; i < FFT_SIZE; ++i) {
			Serial.println(magnitudes[i]);
		}
	}
	else if (strcmp(command, "GET SAMPLES") == 0) {
		for (int i = 0; i < FFT_SIZE*2; i+=2) {
			Serial.println(samples[i]);
		}
	}
	else if (strcmp(command, "GET FFT_SIZE") == 0) {
		Serial.println(FFT_SIZE);
	}
	else if (strcmp(command, "GET AUDIO_INPUT_PIN") == 0) {
		Serial.println(AUDIO_INPUT_PIN);
	}
	GET_AND_SET(SAMPLE_RATE_HZ)
		GET_AND_SET(LEDS_ENABLED)
		GET_AND_SET(SPECTRUM_MIN_DB)
		GET_AND_SET(SPECTRUM_MAX_DB)

		// Update spectrum display values if sample rate was changed.
		if (strstr(command, "SET SAMPLE_RATE_HZ ") != NULL) {
			spectrumSetup();
		}

	// Turn off the LEDs if the state changed.
	if (LEDS_ENABLED == 0) {
		myLeds.clear(); // clear display
	}
}
void parserLoop() {
	// Process any incoming characters from the serial port
	while (Serial.available() > 0) {
		char c = Serial.read();
		// Add any characters that aren't the end of a command (semicolon) to the input buffer.
		if (c != ';') {
			c = toupper(c);
			strncat(commandBuffer, &c, 1);
		}
		else
		{
			// Parse the command because an end of command token was encountered.
			parseCommand(commandBuffer);
			// Clear the input buffer
			memset(commandBuffer, 0, sizeof(commandBuffer));
		}
	}
}
void sample_complete(){
	// Run FFT on sample data.
	arm_cfft_radix4_instance_f32 fft_inst;
	arm_cfft_radix4_init_f32(&fft_inst, FFT_SIZE, 0, 1);
	arm_cfft_radix4_f32(&fft_inst, samples);

	// Calculate magnitude of complex numbers output by the FFT.
	arm_cmplx_mag_f32(samples, magnitudes, FFT_SIZE);

	if (LEDS_ENABLED == 1)
	{
		switch(MODE){
			case 0:
				fftLoop();
				break;
			case 1:
				volLoop();
				break;
		}
	}

	// Restart audio sampling.
	samplingBegin();
}

void daft_punk(){
	daft();
	digitalWriteFast(13, HIGH);
	//delay(10);
	//punk();
	//digitalWriteFast(13, LOW);
	//delay(1000);
}


int main(void)
{
	setup();
	sei();		//enable interrupts

	while (1) {
		if(debounce_switch()){
			MODE++;
			if (MODE > 3)
				MODE = 0;
		}
		switch(MODE){
			case 0:
				if (samplingIsDone()){
					sample_complete();
				}
				parserLoop();
				break;
			case 1:
				if (samplingIsDone()){
					sample_complete();
				}
				parserLoop();
				break;
			case 2:
				daft_punk();
				break;
			case 3:
				display_test(); //all LEDs on
				break;
		}
	}


}
