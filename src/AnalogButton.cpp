#ifndef BUTTON_H_
#define BUTTON_H_

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
void AnalogButton::begin()
{
    pinMode(m_pin, INPUT);
    m_state = readPin();
    m_time = millis();
    m_lastState = m_state;
    m_changed = false;
    m_lastChange = m_time;
}

bool AnalogButton::read()
{
    uint32_t ms = millis();
    bool pinVal = readPin();
    if (ms - m_lastChange < m_dbTime)
    {
        m_changed = false;
    }
    else
    {
        m_lastState = m_state;
        m_state = pinVal;
        m_changed = (m_state != m_lastState);
        if (m_changed)
            m_lastChange = ms;
    }
    m_time = ms;
    return m_state;
}

bool AnalogButton::readPin()
{
    int pinVal = analogRead(m_pin);
    if (pinVal > 50)
        return true;

    return false;
}

bool AnalogButton::isPressed()
{
    return m_state;
}

bool AnalogButton::isReleased()
{
    return !m_state;
}

bool AnalogButton::wasPressed()
{
    return m_state && m_changed;
}

bool AnalogButton::wasReleased()
{
    return !m_state && m_changed;
}

AnalogButton::~AnalogButton()
{
}

#endif