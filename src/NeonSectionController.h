#ifndef NEONSECTIONCONTROLLER_H_
#define NEONSECTIONCONTROLLER_H_

#include <Arduino.h>
#include "FastLED.h" // FastLED library. Preferably the latest copy of FastLED 2.1.
#include <JC_Button.h>

#include <FastLED.h> // for short list

enum WireState
{
    OFF,
    WRONG,
    CORRECT
};

class NeonSectionController
{

public:
    NeonSectionController(int section, int code);
    int getValue(void);
    void initialize(struct CRGB *data, CRGB color);
    void update(void);
    void setState(WireState state);
    void drawColor(CRGB color);
    virtual ~NeonSectionController();

private:

    void effect();
    int section;
    // int length;
    int signal;
    int buttonPin;
    WireState currentState;
    bool gReverseDirection;
    CRGB *_leds;
    CRGB correctColor;
    // Button cableButton;
};

#endif /* NEONSECTIONCONTROLLER_H_ */