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
#define COLOR_ORDER GRB  // It's GRB for WS2812 and BGR for APA102.
#define LED_TYPE UCS1903 // Using APA102, WS2812, WS2801. Don't forget to modify LEDS.addLeds to suit.
#define DATA_PIN 6

#define NUM_LEDS 16
#define FRAME_PER_SECOND 60
#define MIN_SIGNAL 100
#define TEST_RANGE 70
uint8_t max_bright = 255; // Overall brightness definition. It can be changed on the fly.

//struct CRGB leds[NUM_LEDS]; // Initialize our LED array.
CRGB leds[NUM_LEDS];

int pinData[4] = {203, 383, 615, 1023};

//PotMonitor volumePot(A5, 12);
//SoundPlayer soundPlayer(18, 15);

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
  delay(3000);        // Soft startup to ease the flow of electrons.

  FastLED.addLeds<UCS1903, DATA_PIN, BRG>(leds, NUM_LEDS);
  FastLED.setBrightness(max_bright);

  neonController0.initialize(leds, CRGB::Red);
  neonController1.initialize(leds, CRGB::Yellow);
  neonController2.initialize(leds, CRGB::Blue);
  neonController3.initialize(leds, CRGB::Green);

  Serial.println(" Start up activitiy");
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

float a0, a1, a2, a3;

bool inRange(int value, int d, int range)
{
  return (((d - range) <= value) && (value <= (d + range)));
}

void loop()
{

  a0 = analogRead(0);
  a1 = analogRead(1);
  a2 = analogRead(2);
  a3 = analogRead(3);

  if (a0 > MIN_SIGNAL)
  {
    Serial.println("high a0 " + String(a0));
    if (inRange(a0, pinData[0], TEST_RANGE))
    {
      neonController0.setState(WireState::CORRECT);
    }
    else
    {
      neonController0.setState(WireState::WRONG);
    }
  }
  else
  {
    neonController0.setState(WireState::OFF);
  }
  if (a1 > MIN_SIGNAL)
  {
    Serial.println("high a1 " + String(a1));
    if (inRange(a1, pinData[1], TEST_RANGE))
    {
      neonController1.setState(WireState::CORRECT);
    }
    else
    {
      neonController1.setState(WireState::WRONG);
    }
  }
  else
  {
    neonController1.setState(WireState::OFF);
  }
  if (a2 > MIN_SIGNAL)
  {
    Serial.println("high a2 " + String(a2));
    if (inRange(a2, pinData[2], TEST_RANGE))
    {
      neonController2.setState(WireState::CORRECT);
    }
    else
    {
      neonController2.setState(WireState::WRONG);
    }
  }
  else
  {
    neonController2.setState(WireState::OFF);
  }
  if (a3 > MIN_SIGNAL)
  {
    Serial.println("high a3 " + String(a3));
    if (inRange(a3, pinData[3], TEST_RANGE))
    {
      neonController3.setState(WireState::CORRECT);
    }
    else
    {
      neonController3.setState(WireState::WRONG);
    }
  }
  else
  {
    neonController3.setState(WireState::OFF);
  }


  neonController0.update();
  neonController1.update();
  neonController2.update();
  neonController3.update();

  FastLED.show();
  delay(1000 / FRAME_PER_SECOND);
}
