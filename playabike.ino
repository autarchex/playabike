#include "FastLED.h"

// How many leds in your strip?
#define NUM_LEDS 60 

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806, define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 9
//#define CLOCK_PIN 3 //not used

#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    60
#define BRIGHTNESS_HIGH 255
#define BRIGHTNESS_LOW 20
#define BRIGHTNESS_SWITCH 11


// Define the array of leds
CRGB leds[NUM_LEDS];
int brightness;

void setup() { 
        pinMode(BRIGHTNESS_SWITCH, INPUT_PULLUP);
	Serial.begin(57600);
	Serial.println("resetting");
	LEDS.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds,NUM_LEDS);
	LEDS.setBrightness(BRIGHTNESS_HIGH);
}


void loop() { 
	static uint8_t hue = 0;
        int indices[15];
        int a;
        for(int i = 0; i < NUM_LEDS; i++) {
		//
                
                a = (i);
                leds[a] = CHSV(hue++, 255, 255);
                a = (a + 1) % NUM_LEDS;
                leds[a] = CHSV(hue++, 255, 255);
                a = (a + 1) % NUM_LEDS;
                leds[a] = CHSV(hue++, 255, 255);
                
                a = (i+10) % NUM_LEDS;
                leds[a] = CHSV(hue++, 255, 255);
                a = (a + 1) % NUM_LEDS;
                leds[a] = CHSV(hue++, 255, 255);
                a = (a + 1) % NUM_LEDS;
                leds[a] = CHSV(hue++, 255, 255);
                
                a = (i+20) % NUM_LEDS;
                leds[a] = CHSV(hue++, 255, 255);
                a = (a + 1) % NUM_LEDS;
                leds[a] = CHSV(hue++, 255, 255);
                a = (a + 1) % NUM_LEDS;
                leds[a] = CHSV(hue++, 255, 255);
                
                a = (i+30) % NUM_LEDS;
                leds[a] = CHSV(hue++, 255, 255);
                a = (a + 1) % NUM_LEDS;
                leds[a] = CHSV(hue++, 255, 255);
                a = (a + 1) % NUM_LEDS;
                leds[a] = CHSV(hue++, 255, 255);
                
                FastLED.show();
                for(int j = 0; j < NUM_LEDS; j++){
                  leds[j] = CRGB::Black;
                }
                
		delay(20);
	}
	
}
