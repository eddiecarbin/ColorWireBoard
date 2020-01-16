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

// Use qsuba for smooth pixel colouring and qsubd for non-smooth pixel colouring
#define qsubd(x, b) ((x > b) ? b : 0)     // Digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
#define qsuba(x, b) ((x > b) ? x - b : 0) // Analog Unsigned subtraction macro. if result <0, then => 0

#include "FastLED.h" // FastLED library. Preferably the latest copy of FastLED 2.1.

#include <FastLED.h> // for short list

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

// Fixed definitions cannot change on the fly.
#define LED_DT 6 // Data pin to connect to the strip.
//#define LED_CK 11                                             // Clock pin for WS2801 or APA102.
#define COLOR_ORDER GRB  // It's GRB for WS2812 and BGR for APA102.
#define LED_TYPE WS2812B // Using APA102, WS2812, WS2801. Don't forget to modify LEDS.addLeds to suit.

#define DATA_PIN 6

#define NUM_LEDS 16

uint8_t max_bright = 255; // Overall brightness definition. It can be changed on the fly.

//struct CRGB leds[NUM_LEDS]; // Initialize our LED array.
CRGB leds[NUM_LEDS];

int pinData[4] = {0, 0, 0, 0};

float a0, a1, a2, a3;

int av = 256;
int bv = 512;
int cv = 768;
int dv = 1025;

int w = 40;

int a, b, c, d, rightSum = 0;

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

  neonController0.initialize(leds);
  neonController1.initialize(leds);
  neonController2.initialize(leds);

  //set_max_power_in_volts_and_milliamps(5, 500);               // FastLED Power management set at 5V, 500mA.

  //  currentPalette = OceanColors_p; //HeatColors_p
  //  currentPalette = PAL;

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
  //leds[4] = CRGB::Red;
  drawSection(0, CRGB::FairyLight, 4);
  drawSection(4, CRGB::Red, 4);
  drawSection(5, CRGB::Blue, 4);

  FastLED.show();
  delay(500);
  // Now turn the LED off, then pause
  //leds[4] = CRGB::Black;
  //drawSection(s1, CRGB::Black, 4);
  //drawSection(s3, CRGB::Black, 4);
  //drawSection(s0, CRGB::Black, 4);
  FastLED.show();
  delay(500);

  return;
  //drawSection(0, CRGB::SeaGreen, 4);
  //drawSection(8, CRGB::DarkMagenta, 4);
  //drawSection(12, CRGB::FairyLight, 4);

  /*   ALT_leds[12] = CRGB(255, 255, 0);
  ALT_leds[13] = CRGB(255, 255, 0);
  ALT_leds[14] = CRGB(255, 255, 0);
  ALT_leds[15] = CRGB(255, 255, 0); */

  FastLED.show();
  delay(2000);

  return;
  a0 = analogRead(0);
  a1 = analogRead(1);
  a2 = analogRead(2);
  a3 = analogRead(3);
  if (a0 > 0)
  {
    Serial.println("a = " + String(a) + "   " + String(a0));
  }
  if (a1 > 0)
  {
    Serial.println("b = " + String(b) + "   " + String(a1));
  }
  if (a2 > 0)
  {
    Serial.println("c = " + String(c) + "   " + String(a2));
  }
  if (a3 > 0)
  {
    Serial.println("d = " + String(d) + "   " + String(a3));
  }

  // Serial.println("right sum = " + String(rightSum));

  if (av - w < a0 && a0 < av + w)
  {
    a = 1;
    /* ALT_leds[0] = CRGB(0, 255, 0);
    ALT_leds[1] = CRGB(0, 255, 0);
    ALT_leds[2] = CRGB(0, 255, 0);
    ALT_leds[3] = CRGB(0, 255, 0); */
    // FastLED.show();
  }
  else
  {
    a = 0;
    /* ALT_leds[0] = CRGB(255, 255, 255);
    ALT_leds[1] = CRGB(255, 255, 255);
    ALT_leds[2] = CRGB(255, 255, 255);
    ALT_leds[3] = CRGB(255, 255, 255); */
    // FastLED.show();
  }
  if (bv - w < a1 && a1 < bv + w)
  {
    b = 1;
    /* ALT_leds[4] = CRGB(0, 0, 255);
    ALT_leds[5] = CRGB(0, 0, 255);
    ALT_leds[6] = CRGB(0, 0, 255);
    ALT_leds[7] = CRGB(0, 0, 255); */
    //  FastLED.show();
  }
  else
  {
    b = 0;
    /* ALT_leds[4] = CRGB(255, 255, 255);
    ALT_leds[5] = CRGB(255, 255, 255);
    ALT_leds[6] = CRGB(255, 255, 255);
    ALT_leds[7] = CRGB(255, 255, 255); */
    // FastLED.show();
  }
  if (cv - w < a2 && a2 < cv + w)
  {
    c = 1;
    /* ALT_leds[8] = CRGB::Red;
    ALT_leds[9] = CRGB::Red;
    ALT_leds[10] = CRGB::Red;
    ALT_leds[11] = CRGB::Red; */
    //  FastLED.show();
  }
  else
  {
    c = 0;
    /* ALT_leds[8] = CRGB(255, 255, 255);
    ALT_leds[9] = CRGB(255, 255, 255);
    ALT_leds[10] = CRGB(255, 255, 255);
    ALT_leds[11] = CRGB(255, 255, 255); */
    //  FastLED.show();
  }
  if (dv - w < a3 && a3 < dv + w)
  {
    d = 1;
    /* ALT_leds[12] = CRGB(255, 255, 0);
    ALT_leds[13] = CRGB(255, 255, 0);
    ALT_leds[14] = CRGB(255, 255, 0);
    ALT_leds[15] = CRGB(255, 255, 0); */
    //  FastLED.show();
  }
  else
  {
    d = 0;
    /* ALT_leds[12] = CRGB(255, 255, 255);
    ALT_leds[13] = CRGB(255, 255, 255);
    ALT_leds[14] = CRGB(255, 255, 255);
    ALT_leds[15] = CRGB(255, 255, 255); */
    // FastLED.show();
  }

  rightSum = a + b + c + d;

  FastLED.show();

  delay(500);
}
