/* Playabike Mk3 code
Runs my playa bike lighting controller
Requires the Mk3 version of the hardware
Platform is a cheap clone of Arduino Nano V3
UI is a quadrature rotary encoder w/ shaft pushbutton
2017-08-20
*/

#include "FastLED.h"
#include "TimerOne.h"

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
#define BRIGHTNESS_MED 100
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
volatile static uint8_t newstate[3];
volatile static uint8_t prev_switchstate[3];  //used by debounce code

void setup() {
    for(int i=0; i<3; i++) {
        switchdebounce[i] = 0;
        prev_switchstate[i] = 0;
        newstate[i] = 0;
    }
    mode = 0;
    pinMode(PIN_SHAFTBUTTON, INPUT_PULLUP);
	pinMode(PIN_SA, INPUT_PULLUP);
	pinMode(PIN_SB, INPUT_PULLUP);
	LEDS.addLeds<LED_TYPE,PIN_DATA,COLOR_ORDER>(leds,NUM_LEDS);
	LEDS.setBrightness(brightness);
  Timer1.initialize(5000);  //initialize Timer 3 to 5 ms period
  Timer1.attachInterrupt(debounceSwitches);   //set callback function to debounce switches every 10 ms
}

void loop() {
    //This loop executes a full pass of one of the lighting mode subroutines
    //each time through.  The selected mode number is passed to the lighting
    //routine at entry; if the mode changes due to user action while it is
    //running, the subroutine should detect this and immediately return. This
    //loop will then execute the newly selected mode routine.
    switch(mode) {
        case 0:
            runLightsSparkles(mode);
            break;
        case 1:
            runLightsColorChasers(mode);
            break;
        case 2:
            runLightsCylonEye(mode);
            break;
        case 3:
            runLightsBlank(mode);
            break;
        default:
            runLightsSparkles(mode);    //functional lights default
            break;
    }
}


//Lighting mode that does absolutely nothing but waste time
//Approx max latency to user mode change is 0 ms.
void runLightsBlank(uint8_t thismode){
    return;     //do nothing
}

//lighting mode that twinkles all the lights rapidly.  Power hog.
//Approx. max latency to user mode change is 25ms.
void runLightsSparkles(uint8_t thismode){
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
    if(mode != thismode) return; //catch asynchronous mode change by user
}

//Color Chasers.  This light mode has rainbow chasers zipping around.
//Approx. 120ms max latency to user mode change.
void runLightsColorChasers(uint8_t thismode){
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
        if(mode != thismode) return; //catch asynchronous mode change by user
    }
    delay(100);
}

//Single chaser makes it around the bike then slowly changes color between cycles.
//Approx. 100ms max latency to user mode change.
void runLightsCylonEye(uint8_t thismode){
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
    if(mode != thismode) return; //catch asynchronous mode change by user
    }
}

//Called when a shaft button press detected. Cycles to next brightness state.
void changeBrightness() {
    if(brightness == BRIGHTNESS_LOW) brightness = BRIGHTNESS_MED;
    else if(brightness == BRIGHTNESS_MED) brightness = BRIGHTNESS_HIGH;
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
    for(int i = 0; i < 3; i++) switchdebounce[i] <<= 1;  //make room for new value
    switchdebounce[0] |= (0x01 & !digitalRead(PIN_SA));          //bring in the new switch logic values
    switchdebounce[1] |= (0x01 & !digitalRead(PIN_SB));          //and invert it to positive logic for sanity's sake
    switchdebounce[2] |= (0x01 & !digitalRead(PIN_SHAFTBUTTON));
    for(int i=0; i<3; i++){
      if(switchdebounce[i] == 0xff || switchdebounce[i] == 0x00){   //is it stable?
        newstate[i] = switchdebounce[i];                        //then pay attention
      }
    }
    if(newstate[0] != prev_switchstate[0]){       //did switch A change?
      if((newstate[0] && !newstate[1]) || (!newstate[0] && newstate[1])){  //turned cw
        changeMode(true);
      }
    }
    if(newstate[1] != prev_switchstate[1]){       //did switch B change?
      if((newstate[1] && newstate[0]) || (!newstate[1] && !newstate[0])){  //turned ccw
        changeMode(false);
      }
    }
    if(newstate[2] != prev_switchstate[2]){     //did shaft button change?
      if(newstate[2] && !prev_switchstate[2]){  //was it pressed?
        changeBrightness();
      }
    }
    for(int i=0; i<3; i++){
      prev_switchstate[i] = newstate[i];          //record for next iteration
    }
}
