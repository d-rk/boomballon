#include "Led.h"
#include <Arduino.h>

//-----------------------------------------------------------------------------

/**
 * @brief Led::Led Constructor.
 * @param pin digital pin on which the led can be addressed.
 */
Led::Led(uint8_t pin) : PIN(pin), on(false) {
}

//-----------------------------------------------------------------------------

/**
 * @brief Led::setup setup the component.
 */
void Led::setup() {
    pinMode(PIN, OUTPUT);
    setOn(on);
}

//-----------------------------------------------------------------------------

/**
 * @brief Led::setOn Set the LED on or off.
 * @param on set it on?
 */
void Led::setOn(bool on) {
    this->on = on;
    digitalWrite(PIN, on ? HIGH : LOW);
}

//-----------------------------------------------------------------------------

/**
 * @brief Led::blink let the LED blink.
 * @param times number of times the LED should blink.
 * @param delayMs the delay in ms between blinks.
 * @param onAfterwards should the LED stay on after blinking?
 */
void Led::blink(uint8_t times, int delayMs, bool onAfterwards) {
    //if we switch the state of the LED as well, we have to decrease by one iteration.
    times = (onAfterwards != on) ? times -1 : times;

    for (int i=0; i < times; i++) {
        digitalWrite(PIN, on ? LOW : HIGH);
        delay(delayMs);
        digitalWrite(PIN, on ? HIGH : LOW);
        delay(delayMs);
    }

    setOn(onAfterwards);
}

//-----------------------------------------------------------------------------

