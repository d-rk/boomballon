#include "Button.h"
#include <Arduino.h>

//-----------------------------------------------------------------------------

Button::Button(uint8_t pin) : PIN(pin), pressed(false)
{
}

//-----------------------------------------------------------------------------

void Button::setup() {
    pinMode(PIN, INPUT);
}

//-----------------------------------------------------------------------------

bool Button::isPressed() {
    pressed = (digitalRead(PIN) == HIGH);
    return pressed;
}

//-----------------------------------------------------------------------------

bool Button::isReleased() {
    bool buttonDown = (digitalRead(PIN) == HIGH);

    if (pressed) {
        //button was pressed in last iteration
        pressed = buttonDown;
        return !buttonDown; //return if its up now
    } else {
        pressed = buttonDown;
        return false;
    }
}

//-----------------------------------------------------------------------------

