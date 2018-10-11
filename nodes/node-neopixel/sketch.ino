#include <Adafruit_NeoPixel.h>

#define PIN 3
#define TERMINATE '\n'
#define TERMINATE_OPTIONAL ';'

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN, NEO_GRB + NEO_KHZ800);

uint16_t CirclePosition = 0;
long LastRainbowCycleCall = 0;
String Input;
bool Feedback = false;
uint16_t Speed = 100;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  strip.begin();
  strip.show();
}

void loop() {
  /*
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  colorWipe(strip.Color(255, 255, 255), 50); // White RGBW
  */

  /*
  theaterChase(strip.Color(127, 127, 127), 50); // White
  theaterChase(strip.Color(127, 0, 0), 50); // Red
  theaterChase(strip.Color(0, 0, 127), 50); // Blue
*/


  //rainbow(20);
  rainbowCycle();
  SerialTask();
  //colorWipe(strip.Color(255, 255, 255), 50); // White RGBW

  //theaterChaseRainbow(50);

}

void SerialTask() {
    if (! Serial.available()) return; /* nothing to do when nothing arrived. */

  char recieved = Serial.read();

  if ((recieved == TERMINATE) || (recieved == TERMINATE_OPTIONAL)) {
    Input.toUpperCase();
    if (do_action(Input)) {
      if (Feedback) Serial.write("T\n");
    }
    else {
      if (Feedback) Serial.write("F\n");
    }

    Input = "";
    Feedback = true;

    return; /* once LF came, return. */
  }

  Input += recieved; /* else, append recieved char to input */
}

bool do_action(String input) {
  Speed = input.toInt();
  return true;
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle() {
  if (millis() - LastRainbowCycleCall < (2500 / Speed)) {
    return;
  }

  if (CirclePosition ++ < 256) {
    for(uint16_t i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + CirclePosition) & 255));
    }
    strip.show();
  }
  else {
    CirclePosition = 0;
  }

  LastRainbowCycleCall = millis();
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
