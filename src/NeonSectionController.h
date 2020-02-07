#ifndef NEONSECTIONCONTROLLER_H_
#define NEONSECTIONCONTROLLER_H_

#include <Arduino.h>
#include "FastLED.h" // FastLED library. Preferably the latest copy of FastLED 2.1.
#include <JC_Button.h>

#include <FastLED.h> // for short list

enum WireState
{
    NONE,
    OFF,
    EFFECT,
    ON
};

class NeonSectionController
{

public:
    NeonSectionController(int section, int total);
    void initialize(struct CRGB *data, CRGB color);
    void update(void);
    bool setState(WireState state);
    void setColor(CRGB color);
    void drawColor(CRGB color);
    virtual ~NeonSectionController();

private:
    void effect(void);
    void effect2(void);
    int section;
    int length;
    int signal;
    int buttonPin;
    WireState currentState;
    bool gReverseDirection;
    CRGB *_leds;
    uint32_t timebase;
    CRGB correctColor;
    // Button cableButton;
};

#endif /* NEONSECTIONCONTROLLER_H_ */