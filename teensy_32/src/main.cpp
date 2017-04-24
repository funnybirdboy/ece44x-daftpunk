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
#define REG_DISPLAYTEST 0x0F
int SAMPLE_RATE_HZ = 9000;             // Sample rate of the audio in hertz.
int BRIGHTNESS = 0;             		// Brightness control of matrix 0-15
float SPECTRUM_MIN_DB = 50.0;          // Audio intensity (in decibels) that maps to low LED brightness.;
float SPECTRUM_MAX_DB = 70.0;          // Audio intensity (in decibels) that maps to high LED brightness.
int LEDS_ENABLED = 1;                  // Control if the LED's should display the spectrum or not.  1 is true, 0 is false.
int MODE = 0; 							// Sets the mode default is 0
// Useful for turning the LED display on and off with commands from the serial port.
//const int FFT_SIZE = 256;              // Size of the FFT.  Realistically can only be at most 256
const int FFT_SIZE = 256;              // Size of the FFT.  Realistically can only be at most 256
// without running out of memory for buffers and other state.
const int AUDIO_INPUT_PIN = 14;        // Input ADC pin for audio data.
const int ANALOG_READ_RESOLUTION = 10; // Bits of resolution for the ADC.
const int ANALOG_READ_AVERAGING = 16;  // Number of samples to average with each ADC reading.
const int POWER_LED_PIN = 13;          // Output pin for power LED (pin 13 to use Teensy 3.0's onboard LED).
//const int MATRIX_PIN = 3;           // Output pin for matrix D_out.
const int BUTTON_PIN = 15;				//Input pin for button commands
const int NUM_MATRIX = 6;
const int MATRIX_WIDTH = NUM_MATRIX * 8;         // Number of ledss.  You should be able to increase this without
// any other changes to the program.
const int MAX_CHARS = 65;              // Max size of the input command buffer
int current_matrix[48][8];		//FIXME should actually be used
////////////////////////////////////////////////////////////////////////////////
// INTERNAL STATE
// These shouldn't be modified unless you know what you're doing.
////////////////////////////////////////////////////////////////////////////////

IntervalTimer samplingTimer;
float samples[FFT_SIZE*2];
float magnitudes[FFT_SIZE];
int sampleCounter = 0;
char commandBuffer[MAX_CHARS];
//float frequencyWindow[MATRIX_WIDTH+1]; //FIXME had to do this for it to make more sense
float frequencyWindow[16+1];
float hues[MATRIX_WIDTH];

//data, clk, load, quantity
Matrix myLeds = Matrix(3, 5, 4, NUM_MATRIX);

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

void all_on_leds(){
	for( int i = 0; i <= MATRIX_WIDTH; i+=8){
		myLeds.write(i, 0, all_on);
	}
}

void all_off_leds(){
	for( int i = 0; i <= MATRIX_WIDTH; i+=8){
		myLeds.write(i, 0, all_off);
	}
}

void display_test(){
	static int test = 12;
	test = test -1;
	for(int j = 0; j<8; j++){
		for (int i = 0; i < test; ++i) {
			if(current_matrix[i][j]!=1){
				myLeds.write(i, j, HIGH);
				current_matrix[i][j] =1;
			}
		}
		for (int i = test; i < 24; ++i) {
			if(current_matrix[i][j]!=0){
				myLeds.write(i, j, LOW);
				current_matrix[i][j]=0;
			}
		}
	}
	for(int j = 8; j<16; j++){
		for (int i = 24; i < 24+test; ++i) {
			if(current_matrix[i][j-8]!=1){
				myLeds.write(i, j-8, HIGH);
				current_matrix[i][j-8]=1;
			}
		}
		for (int i = 24+test; i < 48; ++i) {
			if(current_matrix[i][j-8]!=0){
				myLeds.write(i, j-8, LOW);
				current_matrix[i][j-8]=0;
			}
		}
	}
	//delay(1000);
	/*
	   all_off_leds();
	   delay(1000);

	   all_on_leds();
	   delay(1000);
	 */
}

//from bottom of display fills all rows up to provided count
void bar_fill(int count){
	static int previous = 0;
	if (count != previous){ //prevent some needless flickering
		for(int j = previous; j<8; j++){
			for (int i = 0; i < count; i++) {
				myLeds.write(i, j, HIGH);
			}
			for (int i = count; i < 24; i++) {
				myLeds.write(i, j, LOW);
			}
		}
		for(int j = 8; j<16; j++){
			for (int i = 24; i < 24+count; i++) {
				myLeds.write(i, j-8, HIGH);
			}
			for (int i = 24+count; i < 48; i++) {
				myLeds.write(i, j-8, LOW);
			}
		}
		previous = count;
	}

}
// used to test bar_fill()
void bar_filler(){
	myLeds.clear();
	for(int j =0; j<MATRIX_WIDTH; j++){
		bar_fill(j);
		delay(1000);
	}
}

//flashes left then right whole column
void flip_flop(){
	myLeds.clear();
	static int previous = 0;

	if (previous == 0){
		for(int j =0; j<8; j++){
			for (int i = 0; i < MATRIX_WIDTH/2; i++) {
				myLeds.write(i, j, HIGH);
			}
		}
		previous = 1;
	}
	else{
		for(int j =0; j<8; j++){
			for (int i = MATRIX_WIDTH/2; i < MATRIX_WIDTH; i++) {
				myLeds.write(i, j, HIGH);
			}
		}
		previous = 0;
	}
	delay(1000);

}

// Makes crazy patterns
void make_it_rain(){
	for(int j =0; j<8; j++){
		//delay(100);
		for (int i = 0; i < MATRIX_WIDTH; i++) {
			myLeds.write(i, j, HIGH);
			myLeds.clear();
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
	//TODO check if needed
	// Set the frequency window values by evenly dividing the possible frequency
	// spectrum across the number leds.
	float windowSize = (SAMPLE_RATE_HZ / 2.0) / float(MATRIX_WIDTH);
	for (int i = 0; i < MATRIX_WIDTH+1; ++i) {
		frequencyWindow[i] = i*windowSize;
	}
}

void volLoop() {
	// Update each LED based on the intensity of the audio
	// in the associated frequency window.
	static int previous = 0;
	float intensity, otherMean;
	float max = 0;
	float min = 1;
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
	//myLeds.setBrightness(int(((max-min)/2)*15)); //old method
	int average = int(((max-min)/2)*24); //old method
	previous+=1;
	if(previous >=3){
		bar_fill(average);
		previous =0;
	}
	//bar_fill(int(((max-min)/2)*MATRIX_WIDTH));
	//bar_fill(int(intensity*8));


}
void fftLoop() {
	// Update each LED based on the intensity of the audio
	// in the associated frequency window.
	float intensity, otherMean;
	//float max = 0;
	//float min = 1;
	myLeds.clear();
	//TODO use a static array to determine if the led is already turned on.
	for (int i = 0; i < 8; ++i) {
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
		for(int j = 0; j<int(intensity*24); j++){
			myLeds.write(j, i, HIGH);
		}
		//for(int j = int(intensity*24); j<24; j++){
		//	myLeds.write(j, i, LOW);
		// }
	}
	for (int i = 8; i < 16; ++i) {
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
		for(int j = 24; j<int(intensity*24+24); j++){
			myLeds.write(j, i-8, HIGH);
		}
		//for(int j = int(intensity*24+24); j<48; j++){
		//myLeds.write(j, i-8, LOW);
		//}
	}
	/*
	   for (int i = 8; i < 16; ++i) {
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
for(int j = 8; j>8-int(intensity*7); j--){
myLeds.write(i+24, j, HIGH);
}
}
	 */
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
	//set absurd initial value
	//FIXME should actually be using this
	for(int i =0; i <48; i++){
		for(int j=0; j<8; j++){
			current_matrix[i][j] = 2; 
		}
	}

	// Clear display, not strictly needed.
	// Set initial brightness, default is usuablly max
	myLeds.clear();
	myLeds.setBrightness(15); //value range 0-15 zero still shows value

	// Open serial channel for app
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

////////////////////////////////////////////////////////////////////////////////
// SILLY PRINT WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////
void daft(){
	myLeds.write(0, 0, letter_D);
	myLeds.write(24, 0, letter_A);
	myLeds.write(8, 0, letter_F);
	myLeds.write(32, 0, letter_T);
}

void punk(){
	myLeds.write(0, 0, letter_P);
	myLeds.write(24, 0, letter_U);
	myLeds.write(8, 0, letter_N);
	myLeds.write(32, 0, letter_K);
}

void daft_punk(){
	daft();
	digitalWriteFast(13, HIGH);
	delay(1000);;
	punk();
	digitalWriteFast(13, LOW);
	delay(1000);
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
	else if (strcmp(command, "SET MODE") == 0) {
		MODE = 2;
	}
	else if (strcmp(command, "GET MODE") == 0) {
		Serial.println(MODE);
	}
	GET_AND_SET(SAMPLE_RATE_HZ)
		GET_AND_SET(MODE)
		GET_AND_SET(LEDS_ENABLED)
		GET_AND_SET(SPECTRUM_MIN_DB)
		GET_AND_SET(SPECTRUM_MAX_DB)
		GET_AND_SET(BRIGHTNESS)

		// Turn off the LEDs if the state changed.
		// Not actually being used in app
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
				//fftLoop();
				volLoop();
				break;
		}
	}

	// Restart audio sampling.
	samplingBegin();
}



int main(void)
{
	//delay(10);
	setup();
	sei();		//enable interrupts

	myLeds.setBrightness(10); //TODO make variable
	MODE = 0;	//inital mode
	while (1) {
		if(debounce_switch()){
			MODE++;
			if (MODE > 3)
				MODE = 0;
		}
		parserLoop(); //process any serial commands
		switch(MODE){
			case 0:
				if (samplingIsDone()){
					sample_complete();
				}
				break;
			case 1:
				//myLeds.setBrightness(0);
				if (samplingIsDone()){
					sample_complete();
				}
				break;
			case 2:
				//myLeds.setBrightness(0);
				//flip_flop();
				//bar_filler();
				bar_fill(21);
				//daft_punk();
				//display_test();
				break;
			case 3:
				//setup();/
				//myLeds.setBrightness(0); //TODO make variable
				make_it_rain();
				//daft_punk();

				//display_test();
				break;
			case 4:
				//myLeds.clear();
				break;
		}
	}


}
