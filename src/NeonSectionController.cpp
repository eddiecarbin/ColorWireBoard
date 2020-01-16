#include <Arduino.h>
#include "NeonSectionController.h"
#include "FastLED.h" // FastLED library. Preferably the latest copy of FastLED 2.1.

#include <FastLED.h> // for short list
int section, signal;
int length = 4;
CRGB* _leds = NULL;

NeonSectionController::NeonSectionController(int section, int code)
{
    this->section = section;
    this->signal = code;
}

void NeonSectionController::initialize(struct CRGB *data)
{
    this->_leds = data;
}

void NeonSectionController::drawSection(CRGB color)
{
    for (int i = section; i < (section + length); i++)
    {
        Serial.println(i);
        _leds[i] = color;
    }
}

void NeonSectionController::update()
{
}

NeonSectionController::~NeonSectionController()
{
}