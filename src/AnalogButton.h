#ifndef ANALOGBUTTON_H_
#define ANALOGBUTTON_H_

#include <Arduino.h>

class AnalogButton
{
private:
    uint8_t m_pin;         // arduino pin number connected to button
    uint32_t m_dbTime;     // debounce time (ms)
    bool m_state;          // current button state, true=pressed
    bool m_lastState;      // previous button state
    bool m_changed;        // state changed since last read
    uint32_t m_time;       // time of current state (ms from millis)
    uint32_t m_lastChange; // time of last state change (ms)
    bool readPin(void);

public:
    AnalogButton(uint8_t pin, uint32_t dbTime = 25)
        : m_pin(pin), m_dbTime(dbTime) {}
    void begin();

    bool read();

    bool isPressed();

    bool isReleased();

    bool wasPressed();

    bool wasReleased();
    
    ~AnalogButton();
};

#endif