#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

class Button
{
public:
    Button(uint8_t PIN);

    void setup();

    bool isPressed();

    bool isReleased();

private:
    const uint8_t PIN;
    bool pressed;
};

#endif // BUTTON_H
