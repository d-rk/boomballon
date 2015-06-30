#ifndef LED_H
#define LED_H

#include <stdint.h>

class Led {

public:
    Led(uint8_t pin);

    void setup();

    void setOn(bool on);

    void blink(uint8_t times, int delayMs, bool onAfterwards);

private:
    const uint8_t pin;
    bool on;
};

#endif // LED_H
