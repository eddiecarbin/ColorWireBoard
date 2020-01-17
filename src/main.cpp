#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>

#include "NeonSectionController.h"

/* Plasma

   By: Andrew Tuline

   Date: July, 2017

   This demonstrates 2D sinusoids in 1D using 16 bit math.

   It runs at about 450 fps on an Arduino Nano with 60 APA102's. That would be FAST.


   References:

   https://www.bidouille.org/prog/plasma
   http://lodev.org/cgtutor/plasma.html

*/
#include <JC_Button.h>
#include "../lib/SoundPlayer/SoundPlayer.h"
#include "../lib/FSM/FSM.h"
#include "../lib/PotMonitor/PotMonitor.h"

// Use qsuba for smooth pixel colouring and qsubd for non-smooth pixel colouring
// #define qsubd(x, b) ((x > b) ? b : 0)     // Digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
// #define qsuba(x, b) ((x > b) ? x - b : 0) // Analog Unsigned subtraction macro. if result <0, then => 0

#include "FastLED.h" // FastLED library. Preferably the latest copy of FastLED 2.1.

#include <FastLED.h> // for short list

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

// Fixed definitions cannot change on the fly.
#define LED_DT 6         // Data pin to connect to the strip.
#define COLOR_ORDER GRB  // It's GRB for WS2812 and BGR for APA102.
#define LED_TYPE WS2812B // Using APA102, WS2812, WS2801. Don't forget to modify LEDS.addLeds to suit.

#define DATA_PIN 6

#define NUM_LEDS 16

uint8_t max_bright = 255; // Overall brightness definition. It can be changed on the fly.

//struct CRGB leds[NUM_LEDS]; // Initialize our LED array.
CRGB leds[NUM_LEDS];

int pinData[4] = {0, 0, 0, 0};

// float a0, a1, a2, a3;
Button cablePin0(A0);
Button cablePin1(A1);
Button cablePin2(A2);
Button cablePin3(A3);

PotMonitor volumePot(A5, 12);
SoundPlayer soundPlayer(18, 15);

int av = 256;
int bv = 512;
int cv = 768;
int dv = 1025;

int w = 40;

int a, b, c, d, rightSum = 0;

long map2(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
}

void drawSection(int start, CRGB color, int length = 0)
{
  for (int i = start; i < (start + length); i++)
  {
    Serial.println(i);
    leds[i] = color;
  }
}

NeonSectionController neonController0(0, 267);
NeonSectionController neonController1(4, 512);
NeonSectionController neonController2(8, 768);
NeonSectionController neonController3(12, 1025);

void setup()
{

  Serial.begin(9600); // Initialize serial port for debugging.
  delay(1000);        // Soft startup to ease the flow of electrons.

  // LEDS.addLeds<LED_TYPE, LED_DT, LED_CK, COLOR_ORDER>(leds, NUM_LEDS);  //WS2801 and APA102
  //LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);
  //LEDS.addLeds<ALT_LED_TYPE, DATA_PIN, ALT_COLOR_ORDER>(ALT_leds, ALT_NUM_LEDS); // WS2812

  FastLED.addLeds<UCS1903, DATA_PIN, BRG>(leds, NUM_LEDS);

  //  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(ALT_leds, ALT_NUM_LEDS);

  FastLED.setBrightness(max_bright);

  cablePin0.begin();
  cablePin1.begin();
  cablePin2.begin();
  cablePin3.begin();

  neonController0.initialize(leds, CRGB::Red);
  neonController1.initialize(leds, CRGB::Yellow);
  neonController2.initialize(leds, CRGB::Blue);
  neonController3.initialize(leds, CRGB::Green);

  Serial.println(" Start up activitiy");

} // setup()

void resetBoard()
{
}

bool correctPinOrder(int arr[], int n)
{
  for (int i = 1; i < n; i++)
  {
    if (arr[i - 1] == 0)
      return false;

    // Unsorted pair found
    if (arr[i - 1] > arr[i])
      return false;

    // No unsorted pair found
  }
  return true;
}

void loop()
{
  if (volumePot.hasUpdated())
  {
    long volume = map2(volumePot.getValue(), 0, 1023, 0, 30);
    Serial.println(volume);
    soundPlayer.volume(volume);
  }

  cablePin0.read();
  cablePin1.read();
  cablePin2.read();
  cablePin3.read();

  neonController0.update();
  neonController1.update();
  neonController2.update();
  neonController3.update();
  //leds[4] = CRGB::Red;
  // drawSection(0, CRGB::FairyLight, 4);
  // drawSection(4, CRGB::Red, 4);
  // drawSection(5, CRGB::Blue, 4);

  // Serial.println("loop");
  neonController2.drawColor(CRGB::Red);

  FastLED.show();
  delay(500);
  // Now turn the LED off, then pause
  //leds[4] = CRGB::Black;
  //drawSection(s1, CRGB::Black, 4);
  //drawSection(s3, CRGB::Black, 4);
  //drawSection(s0, CRGB::Black, 4);
  // FastLED.show();
  // delay(500);

  //drawSection(0, CRGB::SeaGreen, 4);
  //drawSection(8, CRGB::DarkMagenta, 4);
  //drawSection(12, CRGB::FairyLight, 4);

  /*   ALT_leds[12] = CRGB(255, 255, 0);
  ALT_leds[13] = CRGB(255, 255, 0);
  ALT_leds[14] = CRGB(255, 255, 0);
  ALT_leds[15] = CRGB(255, 255, 0); */

}
