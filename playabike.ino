/* Playabike Mk3 code
Runs my playa bike lighting controller
Requires the Mk3 version of the hardware
Platform is a cheap clone of Arduino Nano V3
UI is a quadrature rotary encoder w/ shaft pushbutton
2017-08-20
*/

#include "FastLED.h"

// How many leds in your strip?
#define NUM_LEDS 60

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806, define both DATA_PIN and CLOCK_PIN
#define PIN_DATA 4  //LED string data is connected to this port
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    60		//60 LEDs in the whole string
#define BRIGHTNESS_HIGH 255
#define BRIGHTNESS_LOW 20
#define BRIGHTNESS_SWITCH 10	//this port has an active-low slide switch, when high, cut the brightness
#define PIN_SA 7    //rotary encoder switch A, active low
#define PIN_SB 6    //rotary encoder switch B, active low
#define PIN_SHAFTBUTTON 5    //shaft pushbutton, active low
#define NUM_MODES 4

// Define the array of leds
CRGB leds[NUM_LEDS];

static uint8_t hue = 0;                         //global HSV color vars
static uint8_t sat = 255;
static uint8_t val = 255;
volatile static int mode = 0;                        //global lighting mode
volatile static int thismode = 0;                    //used to track mode changes
volatile static uint8_t brightness = BRIGHTNESS_LOW;     //global brightness; default low
volatile static uint8_t switchdebounce[3];  //used by debounce code
volatile static uint8_t prev_switchstate[3];  //used by debounce code

void setup() {
    for(int i=0; i<3; i++) {
        switchdebounce[i] = 0;
        prev_switchstate[i] = 0;
    }
    pinMode(PIN_SHAFTBUTTON, INPUT_PULLUP);
	pinMode(PIN_SA, INPUT_PULLUP);
	pinMode(PIN_SB, INPUT_PULLUP);
	LEDS.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds,NUM_LEDS);
	LEDS.setBrightness(brightness);
}

void loop() {
    //the while loop executes one full cycle of selected lighting mode,
    //unless a break occurs due to an asynchronoush mode change.
    while(true) {
        //PRGM 0 - Sparkles!
	if(mode == 0) {						//select action based on switch state
        thismode = 0;
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
        if(mode != thismode) break; //catch asynchronous mode change by user
	}
        //PRGM 1 - COLOR CHASERS (rovers)
        else if(mode == 1) {
            thismode = 1;
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
                if(mode != thismode) break; //catch asynchronous mode change by user
                }
                delay(100);
        }
        //PRGM 2 - Cylon Eye
        else if(mode == 2) {
            thismode = 2;
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
                if(mode != thismode) break; //catch asynchronous mode change by user
                }
                //delay(100);
        }
        //PRGM 3 - LEAVE BLANK FOR PROGRAMMING
        else if(mode == 3) {
            thismode = 3;
                1;  //LEAVE BLANK
        }
    }//outer while loop
}//end of loop()

//Called when a shaft button press detected. Cycles to next brightness state.
void changeBrightness() {
    if(brightness == BRIGHTNESS_LOW) brightness = BRIGHTNESS_HIGH;
    else if(brightness == BRIGHTNESS_HIGH) brightness = BRIGHTNESS_LOW;
    else brightness = BRIGHTNESS_LOW;
    LEDS.setBrightness(brightness);     //immediately update the FastLED code
}

//Called when shaft rotation occurs. Cycles to next or previous mode.
//Argument 'increase' is boolean; TRUE->increase mode number, FALSE->decrease.
void changeMode(bool increase) {
    if(increase) {
        mode++;
        if(mode >= NUM_MODES) mode = 0;     //roll over to zero
    }
    else {
        mode--;
        if(mode < 0) mode = NUM_MODES - 1;  //roll over to max
    }
}

//called by timer interrupt
void debounceSwitches() {
    for(int i = 0; i < 3; i++) switchdebounce[i] << 1;
    switchdebounce[0] |= !digitalRead(PIN_SA);
    switchdebounce[1] |= !digitalRead(PIN_SB);
    switchdebounce[2] |= !digitalRead(PIN_SC);
    //rotary encoder A
    if(switchdebounce[0] == 0x00 || switchdebounce[0] == 0xff){ //stable?
        if(switchdebounce[0] != prev_switchstate[0]){  //new?
            //TODO:: rotary encoder logic
            //call changeMode(increase=) with either TRUE or FALSE
        }
        prev_switchstate[0] = switchdebounce[0];        //record
    }
    //rotary encoder B
    if(switchdebounce[1] == 0x00 || switchdebounce[1] == 0xff){ //stable?
        if(switchdebounce[1] != prev_switchstate[1]){           //new?
            //TODO:: rotary encoder logic
            //call changeMode(increase=) with either TRUE or FALSE
        }
        prev_switchstate[1] = switchdebounce[1];        //record
    }
    //shaft switch
    if(switchdebounce[2] == 0x00 || switchdebounce[2] == 0xff){ //stable?
        if(switchdebounce[2] && switchdebounce[2] != prev_switchstate[2]){  //newly pressed?
            changeBrightness();
        }
        prev_switchstate[2] = switchdebounce[2];
    }
}
