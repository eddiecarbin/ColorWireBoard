#include "AnalogButton.h"

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
