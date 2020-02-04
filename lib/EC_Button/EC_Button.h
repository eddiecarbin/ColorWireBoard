#ifndef EC_BUTTON_H_INCLUDED
#define EC_BUTTON_H_INCLUDED

#include <Arduino.h>

class Button
{
private:
    uint8_t m_pin;     // arduino pin number connected to button
    uint32_t m_dbTime; // debounce time (ms)
    bool m_puEnable;   // internal pullup resistor enabled
    bool m_invert;     // if true, interpret logic low as pressed, else interpret logic high as pressed
    bool m_state;      // current button state, true=pressed
    bool m_lastState;  // previous button state
    bool m_changed;    // state changed since last read
    bool m_analog;
    uint32_t m_time;       // time of current state (ms from millis)
    uint32_t m_lastChange; // time of last state change (ms)
public:
    Button(uint8_t pin, uint32_t dbTime = 25, uint8_t puEnable = true, uint8_t pAnalog = false, uint8_t invert = true)
        : m_pin(pin), m_dbTime(dbTime), m_puEnable(puEnable), m_analog(pAnalog), m_invert(invert) {}
    Button();
    bool read(void);
    void begin(void);
    // Returns true if the button state was pressed at the last call to read().
    // Does not cause the button to be read.
    bool isPressed();

    // Returns true if the button state was released at the last call to read().
    // Does not cause the button to be read.
    bool isReleased();

    // Returns true if the button state at the last call to read() was pressed,
    // and this was a change since the previous read.
    bool wasPressed();

    // Returns true if the button state at the last call to read() was released,
    // and this was a change since the previous read.
    bool wasReleased();

    // Returns true if the button state at the last call to read() was pressed,
    // and has been in that state for at least the given number of milliseconds.
    bool pressedFor(uint32_t ms);

    // Returns true if the button state at the last call to read() was released,
    // and has been in that state for at least the given number of milliseconds.
    bool releasedFor(uint32_t ms);

    // Returns the time in milliseconds (from millis) that the button last
    // changed state.
    uint32_t lastChange();
};

Button::Button()
{
}

void Button::begin()
{
    pinMode(m_pin, m_puEnable ? INPUT_PULLUP : INPUT);
    m_state = digitalRead(m_pin);
    if (m_invert)
        m_state = !m_state;
    m_time = millis();
    m_lastState = m_state;
    m_changed = false;
    m_lastChange = m_time;
}
bool Button::read()
{
    uint32_t ms = millis();
    bool pinVal = digitalRead(m_pin);
    if (m_invert)
        pinVal = !pinVal;
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

/*----------------------------------------------------------------------*
 * isPressed() and isReleased() check the button state when it was last *
 * read, and return false (0) or true (!=0) accordingly.                *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
bool Button::isPressed()
{
    return m_state;
}

bool Button::isReleased()
{
    return !m_state;
}

/*----------------------------------------------------------------------*
 * wasPressed() and wasReleased() check the button state to see if it   *
 * changed between the last two reads and return false (0) or           *
 * true (!=0) accordingly.                                              *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
bool Button::wasPressed()
{
    return m_state && m_changed;
}

bool Button::wasReleased()
{
    return !m_state && m_changed;
}

/*----------------------------------------------------------------------*
 * pressedFor(ms) and releasedFor(ms) check to see if the button is     *
 * pressed (or released), and has been in that state for the specified  *
 * time in milliseconds. Returns false (0) or true (!=0) accordingly.   *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
bool Button::pressedFor(uint32_t ms)
{
    return m_state && m_time - m_lastChange >= ms;
}

bool Button::releasedFor(uint32_t ms)
{
    return !m_state && m_time - m_lastChange >= ms;
}

/*----------------------------------------------------------------------*
 * lastChange() returns the time the button last changed state,         *
 * in milliseconds.                                                     *
 *----------------------------------------------------------------------*/
uint32_t Button::lastChange()
{
    return m_lastChange;
}
#endif
