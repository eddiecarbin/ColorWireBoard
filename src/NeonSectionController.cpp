#include <Arduino.h>
#include "NeonSectionController.h"
#include "FastLED.h" // FastLED library. Preferably the latest copy of FastLED 2.1.

#include <FastLED.h> // for short list
#define LED_TOTAL 4

int section, signal;

CRGB *_leds = NULL;
CRGB correctColor;

WireState currentState = WireState::OFF;

NeonSectionController::NeonSectionController(int section, int code)
{
    this->section = section;
    this->signal = code;
}

void NeonSectionController::initialize(struct CRGB *data, CRGB color)
{
    this->_leds = data;
    this->correctColor = color;
    Button cableButton(A0);
    // cableButton.read();
}

void NeonSectionController::drawColor(CRGB color)
{
    for (int i = section; i < (section + LED_TOTAL); i++)
    {
        _leds[i] = color;
        // Serial.println(_leds[i]);
        // Serial.println(_leds[10000]);
    }
}

void NeonSectionController::setState(WireState state)
{
    currentState = state;

    switch (currentState)
    {
    case WireState::OFF:
        drawColor(CRGB::Black);
        break;
    case WireState::CORRECT:
        drawColor(correctColor);
        break;
    default:
        // display wrong effect
        
        break;
    }
}

int NeonSectionController::getValue()
{
    return signal;
}

void NeonSectionController::update()
{
    // cableButton.read();
    if (currentState == WireState::WRONG)
    {
    }
}

NeonSectionController::~NeonSectionController()
{
}