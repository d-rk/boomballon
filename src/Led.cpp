#include "Led.h"
#include <Arduino.h>

//-----------------------------------------------------------------------------

Led::Led(uint8_t pin) : pin(pin), on(false) {
}

//-----------------------------------------------------------------------------

void Led::setup() {
    pinMode(pin, OUTPUT);
    setOn(on);
}

//-----------------------------------------------------------------------------

void Led::setOn(bool on) {
    this->on = on;
    digitalWrite(pin, on ? HIGH : LOW);
}

//-----------------------------------------------------------------------------

void Led::blink(uint8_t times, int delayMs, bool onAfterwards) {
    //if we switch the state of the LED as well, we have to decrease by one iteration.
    times = (onAfterwards != on) ? times -1 : times;

    for (int i=0; i < times; i++) {
        digitalWrite(pin, on ? LOW : HIGH);
        delay(delayMs);
        digitalWrite(pin, on ? HIGH : LOW);
        delay(delayMs);
    }

    setOn(onAfterwards);
}

//-----------------------------------------------------------------------------

