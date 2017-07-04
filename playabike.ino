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

static uint8_t hue = 0;
static uint8_t sat = 255;
static uint8_t val = 255;

void setup() { 
        pinMode(BRIGHTNESS_SWITCH, INPUT_PULLUP);
	pinMode(MODE_0_SWITCH, INPUT_PULLUP);
	pinMode(MODE_1_SWITCH, INPUT_PULLUP);
	LEDS.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds,NUM_LEDS);
	LEDS.setBrightness(BRIGHTNESS_HIGH);				//should this start as HIGH?
}


void loop() {
	int mode = (digitalRead(MODE_0_SWITCH) << 0) | (digitalRead(MODE_1_SWITCH) << 1);	//get mode state from switch states
	int brightness = digitalRead(BRIGHTNESS_SWITCH) ? BRIGHTNESS_HIGH : BRIGHTNESS_LOW;	//get brightness from switch state
        LEDS.setBrightness(brightness);                          //set master brightness level, from switch state
        
        
        //PRGM 0 - Sparkles!
	if(mode == 0) {						//select action based on switch state
		//Multiple rovers of changing color chasing each other
		int rovers = 4;						//number of discrete groups of light
		int roverlength = 3;					//length of the group
		for(int led = 0; led < NUM_LEDS; led++) {			//iterate over entire string of leds
			for(int rover = 0; rover < rovers; rover++) {		//iterate over all rovers
			    int a = (led + (10 * rover));			//rover's base point
			    for(int k = 0; k < roverlength; k++) {	//iterate over pixels in a rover
				a = (a + k) % NUM_LEDS;		//handle overflow over end of led string
				leds[a] = CHSV(hue++, 255, 255);//each pixel written has a different hue
			    }
		        }
                }
       		FastLED.show();					//output the new string state
		for(int j = 0; j < NUM_LEDS; j++){
			leds[j] = CRGB::Black;			//blank out the in-memory state of the string
		}
		delay(25);					//brief visual pause
	}
        //PRGM 1 - COLOR CHASERS (rovers)
        else if(mode == 1) {
                for(int led = 0; led < NUM_LEDS; led++) {      //iterate over string of all LEDs, for a complete loop
                    for(int rover = 0; rover < 4; rover++) {   //iterate over four rovers
                        int roverstart = led + (12 * rover);   //space the rovers apart by 12 pixels
                        for(int roverpixel = 0; roverpixel < 3; roverpixel++) {  //iterate over 3 pixels in the rover
                            leds[(roverstart + roverpixel) % NUM_LEDS] = CHSV(hue++, 255, 255);  //new color each pixel write
                        }
                    }
                    FastLED.show();
                    for(int j = 0; j < NUM_LEDS; j++){
                      leds[j] = CRGB::Black;
                    }
    		    delay(20);
                }
                delay(100);
        }
        //PRGM 2 - Cylon Eye
        else if(mode == 2) {
                for(int led = 0; led < NUM_LEDS; led++) {      //iterate over string of all LEDs, for a complete loop
                    hue++;
                    for(int rover = 0; rover < 3; rover++) {   //iterate over 3 rovers
                        int roverstart = led + (20 * rover);
                        leds[(roverstart + 0) % NUM_LEDS] = CHSV(hue, sat, 40);
                        leds[(roverstart + 1) % NUM_LEDS] = CHSV(hue, sat, 100);
                        leds[(roverstart + 2) % NUM_LEDS] = CHSV(hue, sat, 255);
                        leds[(roverstart + 3) % NUM_LEDS] = CHSV(hue, sat, 100);
                        leds[(roverstart + 4) % NUM_LEDS] = CHSV(hue, sat, 40);
                    }
                    FastLED.show();
                    for(int j = 0; j < NUM_LEDS; j++){
                      leds[j] = CRGB::Black;
                    }
    		    delay(100);
                }
                //delay(100);
        }
        //PRGM 3 - LEAVE BLANK FOR PROGRAMMING
        else if(mode == 3) {
                1;  //LEAVE BLANK
        }
        
        
}

