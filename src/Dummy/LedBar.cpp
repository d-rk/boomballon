#include "LedBar.h"

#include <Arduino.h>

//-----------------------------------------------------------------------------

LedBar::LedBar(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4)
    : PINS{pin1, pin2, pin3, pin4}
{

}

//-----------------------------------------------------------------------------

void LedBar::setup() {
    for(uint8_t pin : PINS) {
        pinMode(pin, OUTPUT);
    }
    setLight(0);
}

//-----------------------------------------------------------------------------

void LedBar::setLight(uint8_t light) {
    digitalWrite(PINS[0], light > 0   ? HIGH : LOW);
    digitalWrite(PINS[1], light > 64  ? HIGH : LOW);
    digitalWrite(PINS[2], light > 128 ? HIGH : LOW);
    digitalWrite(PINS[3], light > 196 ? HIGH : LOW);
}

//-----------------------------------------------------------------------------

