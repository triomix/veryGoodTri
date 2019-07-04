#include "FastLED.h"
#include <SharpIR.h>

// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED.

#define IRPin A0
#define STAR_PIN 6
#define HOLE_PIN 5
#define STAR_NUM_LEDS 113
#define HOLE_NUM_LEDS 22

#define LED_TYPE    NEOPIXEL

CRGB star[STAR_NUM_LEDS];
CRGB hole[HOLE_NUM_LEDS];

#define FRAMES_PER_SECOND  120
#define NB_FLASH_CYCLES 4

#define model 20150
// Create variable to store the distance:
int distance_cm = 100;
/* Model :
  GP2Y0A02YK0F --> 20150
  GP2Y0A21YK0F --> 1080
  GP2Y0A710K0F --> 100500
  GP2YA41SK0F --> 430
*/
SharpIR mySensor = SharpIR(IRPin, model);

int count_bottle = 0;
int brightness = 100;
boolean go_count = false; // decide if we have to count  loop or not.
int loop_count = 0; // loop count for flashing scenario.
int gauge_counter = 0; // filling gauge of the trash

void setup() {
  Serial.begin(9600);
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, STAR_PIN>(star, STAR_NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, HOLE_PIN>(hole, HOLE_NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(brightness);
  // All leds black
  star[0] = CRGB::Black;
  hole[0] = CRGB::Black;
  FastLED.show();
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { bpm, yellow_confetti };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void loop()
{
  // read distance to see if an object is thrown.
  distance_cm = mySensor.distance();
  if (distance_cm > 10 && distance_cm < 30) { // all time we pas here even breathing is activated
    // The bottle falls in the trash
    Serial.println(distance_cm);
    brightness = 255;
    gCurrentPatternNumber = 1;
    go_count = true;
    count_bottle++;
  }

  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.setBrightness(brightness);
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) {
    gHue++;  // slowly cycle the "base color" through the rainbow
  }

  EVERY_N_SECONDS( 1 ) {
    // See if we need to change scenario
    if (loop_count == NB_FLASH_CYCLES) {
      gCurrentPatternNumber = 0;
      loop_count = 0;
      brightness = 100;
      go_count = false;
    } else {
      if (go_count) {
        loop_count++;
      }
    }
    //nextPattern();  // change patterns periodically
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

/*
  void rainbow()
  {
  Serial.println("rainbow");
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  }

  void rainbowWithGlitter()
  {
  Serial.println("rainbowWithGlitter");
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
  }
*/

void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    star[ random16(STAR_NUM_LEDS) ] += CRGB::White;
    hole[ random16(HOLE_NUM_LEDS) ] += CRGB::White;
  }
}

/*
  void confetti()
  {
  Serial.println("confetti");
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
  }
*/
void yellow_confetti()
{
  Serial.println("white_confetti");
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( star, STAR_NUM_LEDS, 10);
  int pos = random16(STAR_NUM_LEDS);
  star[pos] += CRGB::Yellow;

  fadeToBlackBy( hole, HOLE_NUM_LEDS, 10);
  pos = random16(HOLE_NUM_LEDS);
  hole[pos] += CRGB::Yellow;

  addGlitter(20);
}
/*
  void sinelon()
  {
  Serial.println("sinelon");
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
  }

  void sinelon_glitter()
  {
  Serial.println("white_sinelon");
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS);
  leds[pos] += CRGB::Blue;
  addGlitter(16);
  }


  void white_bpm()
  {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 2;
  CRGBPalette16 palette = PartyColors_p; // ForestColors_p
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < STAR_NUM_LEDS; i++) { //9948
    star[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
  for ( int i = 0; i < HOLE_NUM_LEDS; i++) { //9948
    hole[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
  }
*/
void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 30;
  CRGBPalette16 palette = ForestColors_p; //PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < STAR_NUM_LEDS; i++) { //9948
    star[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }

  for ( int i = 0; i < HOLE_NUM_LEDS; i++) { //9948
    hole[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

/*
  void juggle() {
  Serial.println("juggle");
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS)] |=  CRGB::Blue; //CHSV(dothue, 200, 255);
    dothue += 32;
  }
  }
*/
