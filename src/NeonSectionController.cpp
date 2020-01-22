#include <Arduino.h>
#include "NeonSectionController.h"
#include "FastLED.h" // FastLED library. Preferably the latest copy of FastLED 2.1.

#include <FastLED.h> // for short list
// #define LED_TOTAL 3

int section;
int length = 0;

bool gReverseDirection = false;

CRGB *_leds = NULL;
CRGB correctColor;

WireState currentState = WireState::NONE;

NeonSectionController::NeonSectionController(int section, int total)
{
    this->section = section;
    this->length = total;
}

void NeonSectionController::initialize(struct CRGB *data, CRGB color)
{
    this->_leds = data;
    this->correctColor = color;
    drawColor(color);
}

void NeonSectionController::drawColor(CRGB color)
{
    for (int i = section; i < (section + length); i++)
    {
        _leds[i] = color;
    }
}

void NeonSectionController::setColor(CRGB color)
{
    correctColor = color;
    drawColor(correctColor);
}

bool NeonSectionController::setState(WireState state)
{
    if (state == currentState)
        return false;

    currentState = state;

    switch (currentState)
    {
    case WireState::OFF:
        drawColor(CRGB::Black);
        break;
    case WireState::ON:
        drawColor(correctColor);
        break;
    default:
        // display wrong effect
        effect();
        break;
    }

    return true;
}

void NeonSectionController::update()
{
    if (currentState == WireState::EFFECT)
    {
        effect();
    }
}

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
#define COOLING 55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 220

void NeonSectionController::effect()
{

    // Array of temperature readings at each simulation cell
    static byte heat[3];

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < length; i++)
    {
        heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / length) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = length - 1; k >= 2; k--)
    {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKING)
    {
        int y = random8(7);
        heat[y] = qadd8(heat[y], random8(160, 255));
    }

    // Step 4.  Map from heat cells to LED colors
    for (int j = 0; j < length; j++)
    {
        CRGB color = HeatColor(heat[j]);
        int pixelnumber;
        if (gReverseDirection)
        {
            pixelnumber = section + (length - 1) - j;
        }
        else
        {
            pixelnumber = section + j;
        }
        _leds[pixelnumber] = color;
    }
}

NeonSectionController::~NeonSectionController()
{
}