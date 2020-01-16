#ifndef NEONSECTIONCONTROLLER_H_
#define NEONSECTIONCONTROLLER_H_

#include <Arduino.h>
#include "FastLED.h" // FastLED library. Preferably the latest copy of FastLED 2.1.

#include <FastLED.h> // for short list

class NeonSectionController
{

public:
    NeonSectionController( int section, int code);
    int getValue(void);
    void initialize(struct CRGB *data);
    void update(void);
    void drawSection(CRGB color);
    virtual ~NeonSectionController();

private:
    int section;
    int length;
    int signal;
    int buttonPin;
    CRGB* _leds;
};

#endif /* NEONSECTIONCONTROLLER_H_ */