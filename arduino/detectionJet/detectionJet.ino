#include <SharpIR.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Define model and input pin:
#define IRPin A0
#define STAR_PIN 6
#define STAR_NUM_LEDS 120

#define model 20150
// Create variable to store the distance:
int distance_cm;
/* Model :
  GP2Y0A02YK0F --> 20150
  GP2Y0A21YK0F --> 1080
  GP2Y0A710K0F --> 100500
  GP2YA41SK0F --> 430
*/
// Create a new instance of the SharpIR class:
SharpIR mySensor = SharpIR(IRPin, model);

Adafruit_NeoPixel star_strip = Adafruit_NeoPixel(STAR_NUM_LEDS, STAR_PIN, NEO_GRB + NEO_KHZ800);

boolean objectHasFallen = false;
boolean ligthing_in_progress = false;

void setup() {
  // Begin serial communication at a baudrate of 9600:
  Serial.begin(9600);

  // Pin definitions
  pinMode(STAR_PIN, OUTPUT);

  // Leds strips init
  star_strip.begin();
  all_blacks(star_strip);
  star_strip.show(); // Initialize all pixels to 'off'
}
void loop() {
  // Get a distance measurement and store it as distance_cm:
  distance_cm = mySensor.distance();
  if (distance_cm > 10 && distance_cm < 30 && !ligthing_in_progress) {
    // The bottle falls in the trash
    Serial.println(distance_cm);
    objectHasFallen =  true;
  }

  // Lightening scenarios
  if (objectHasFallen) {
    ligthing_in_progress = true;
    star_flashing();
    objectHasFallen = false;
  }

}

void star_flashing() {
  // A neopxiel Flashing Scenario
  theaterChase(star_strip, star_strip.Color(127, 127, 127), 50); // White
  all_blacks(star_strip);
  ligthing_in_progress = false;
}

void all_blacks(Adafruit_NeoPixel &strip) {
  colorWipe(strip, strip.Color(0, 0, 0), 0);
}

//Theatre-style crawling lights.
void theaterChase(Adafruit_NeoPixel &strip, uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// Fill the dots one after the other with a color
void colorWipe(Adafruit_NeoPixel &strip, uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
