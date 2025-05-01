#include <Arduino.h>
#include "hwdef.h"
#include "ring.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 8 // Popular NeoPixel ring size

static Adafruit_NeoPixel pixels(NUMPIXELS, WS2812B_DI, NEO_GRB + NEO_KHZ800);
static Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, WS2812B_DI , NEO_GRB + NEO_KHZ800);

const int MinBrightness = 5;
const int MaxBrightness = 20;

Ring::Ring() {
  fadeIn = false;
  b = MinBrightness;
  durCnt = 0;

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

void Ring::init() {
  pixels.clear();
  pixels.show();
  for (int i=0; i<NUMPIXELS; i++) {
      int c = 10;
      pixels.setPixelColor(i, pixels.Color(c, c, c));
      pixels.show();   // Send the updated pixel colors to the hardware.
  }
}

extern bool deviceConnected;

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void Ring::rainbow(int wait) {
   strip.setBrightness(255);
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    //strip.rainbow(firstPixelHue);
    // Above line is equivalent to:
    strip.rainbow(firstPixelHue, 1, 255/2, 255/4, true);
    strip.show(); // Update strip with new contents

    if (deviceConnected)
      break;

    delay(wait);  // Pause for a moment
  }
}

void Ring::onDist(bool stop) {

  if (durCnt > 0) {
    durCnt--;
    return;
  }

  durCnt = 2;
  uint px = 255;
  uint div = 1000;
  
  uint32_t c = stop ? strip.Color(px, px/div, px/div) : strip.Color(px/div, px, px/div);

  if (fadeIn) {
     strip.setBrightness(b * 255 / 255);
      for (uint16_t i = 0; i < strip.numPixels(); i++)
        strip.setPixelColor(i, c);
      strip.show();
  } else {
      strip.setBrightness(b * 255 / 255);
      for (uint16_t i = 0; i < strip.numPixels(); i++)
        strip.setPixelColor(i, c);
      strip.show();
  }

  if (fadeIn) {
      if (b >= MaxBrightness)
        fadeIn = false;
      else
        b++;
  } else {
      if (b <= MinBrightness)
        fadeIn = true;
      else
        b--;
  }
}

