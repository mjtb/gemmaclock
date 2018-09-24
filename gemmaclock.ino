// gemmaclock - Copyright (C) 2018 Michael Trenholm-Boyle
// This source c ode is distributed under a permissive "open source" license.
// See the LICENSE file in the root of the source tree for license information.

// A simple hour clock visualization using four NeoPixel RGB LEDs.
// Every hour is divided into 15 minute segments. Each minute in a
// fifteen minute segment is represented by a single colour from the
// Palette array: the NeoPixels blink in this colour. The rate at which
// they blink is always a 2:1 ratio of on-off time and depends on if the
// current time is in the 1st, 2nd, 3rd or 4th 15-minute segment, as
// controlled by the OnTime array. In the final 10 seconds of every
// 15-minute segment, the pixels quickly cycle through all of the
// palette colours.
//
// All time is expressed in 100 ms units.


#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 1
#define NUM_PIXELS 4

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

int Palette[15] = {
  0x050505, // lch(15 0 0)
  0x161616, // lch(35 0 0)
  0x340C16, // lch(35 30 0)
  0x1C1504, // lch(35 30 90)
  0x001D15, // lch(35 30 180)
  0x091739, // lch(35 30 270)
  0xC33A1E, // lch(65 50 45)
  0x236C1A, // lch(65 50 135)
  0x006CCA, // lch(65 50 235)
  0x8D3EBA, // lch(65 50 315)
  0xD82E5A, // lch(65 50 0)
  0xE2A404, // lch(85 80 90)
  0x007555, // lch(65 50 180)
  0x225AEE, // lch(65 50 270)
  0xA8A8A8 // lch(85 0 0)
};

int OnTime[4] = { 20, 16, 10, 8 };

// sets pixel colour values to the end of 15-minute rainbow
// int t - time since the start of the rainbow
void rainbow(int t) {
  int i, q;
  q = t / 3; // hold for 300 ms
  for(i = 0; i < NUM_PIXELS; ++i) {
    strip.setPixelColor(i, Palette[(q + i) % 15]);
  }
}

// determine if a blinking LED should be on
// int on - time that LEDs should be on when blinking
// int r  - the current time
bool blink(int on, int r) {
  int off, total, now;
  off = on / 2;
  total = on + off;
  now = r % total;
  return now < on;
}

// sets pixel colors according to the current time
// int t - the current time
void tick(int t) {
  int h, q, p, r, i;
  t %= 36000;
  h = t / 9000;
  q = t % 9000;
  if(q >= 8900) {
    rainbow(q - 8900);
  } else {
    p = q / 600;
    r = q % 600;
    for(i = 0; i < NUM_PIXELS; ++i) {
      strip.setPixelColor(i, blink(OnTime[h], r + 2 * i) ? Palette[p] : 0);
    }
  }
}

// loop indefinitely setting pixel colour values based on time
void loop() {
  int t;
  while(true) {
    // reset pattern every hour
    for(t = 0; t < 36000; ++t) {
      tick(t);
      delay(100);
    }
  }
}



