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
	Serial.begin(57600);
	Serial.println("resetting");
	LEDS.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds,NUM_LEDS);
	LEDS.setBrightness(BRIGHTNESS_HIGH);
}


void loop() {
	//all following code is a single orbit of mode 0 pattern
	static uint8_t hue = 0;
        int indices[15];
        int a;
	int rovers = 4;					//number of discrete groups of light
	int roverlength = 3;				//length of the group
        for(int i = 0; i < NUM_LEDS; i++) {
		for(int j = 0; j < rovers; j++) {
	                a = (i + (10 * j));		//rover's base point
	                leds[a] = CHSV(hue++, 255, 255);
	                a = (a + 1) % NUM_LEDS;
	                leds[a] = CHSV(hue++, 255, 255);
	                a = (a + 1) % NUM_LEDS;
	                leds[a] = CHSV(hue++, 255, 255);
		}
                FastLED.show();
                for(int j = 0; j < NUM_LEDS; j++){
                  leds[j] = CRGB::Black;
                }
		delay(20);
	}
	
}
