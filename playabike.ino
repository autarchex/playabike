#include "FastLED.h"

// How many leds in your strip?
#define NUM_LEDS 60 

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806, define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 9
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    60		//60 LEDs in the whole string
#define BRIGHTNESS_HIGH 255
#define BRIGHTNESS_LOW 20
#define BRIGHTNESS_SWITCH 10	//this port has an active-low slide switch, when high, cut the brightness
#define MODE_0_SWITCH 1		//this port has an active-low slide switch
#define MODE_1_SWITCH 2		//this port has an active-low slide switch


// Define the array of leds
CRGB leds[NUM_LEDS];
int brightness;

void setup() { 
        pinMode(BRIGHTNESS_SWITCH, INPUT_PULLUP);
	pinMode(MODE_0_SWITCH, INPUT_PULLUP);
	pinMode(MODE_1_SWITCH, INPUT_PULLUP);
	LEDS.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds,NUM_LEDS);
	LEDS.setBrightness(BRIGHTNESS_HIGH);				//should this start as HIGH?
}


void loop() {
	int mode = (DigitalRead(MODE_0_SWITCH) & 1) + (DigitalRead(MODE_1_SWITCH) & 2)	//get mode state from switch states
	int brightness = DigitalRead(BRIGHTNESS_SWITCH) ? 255 : 20			//get brightness from switch state

	switch(mode) {						//select action based on switch state
		case 0:
			//Multiple rovers of changing color chasing each other
			static uint8_t hue = 0;
			int indices[15];
			int a;
			int rovers = 4;						//number of discrete groups of light
			int roverlength = 3;					//length of the group
			for(int i = 0; i < NUM_LEDS; i++) {			//iterate over entire string of leds
				for(int j = 0; j < rovers; j++) {		//iterate over all rovers
				a = (i + (10 * j));			//rover's base point
				for(int k = 0; k < roverlength; k++) {	//iterate over pixels in a rover
					a = (a + k) % NUM_LEDS;		//handle overflow over end of led string
					leds[a] = CHSV(hue++, 255, 255);//each pixel written has a different hue
				}
			}
			FastLED.show();					//output the new string state
			for(int j = 0; j < NUM_LEDS; j++){
				leds[j] = CRGB::Black;			//blank out the in-memory state of the string
			}
			delay(20);					//wait 20 milliseconds
			break;
	}
}
