#ifndef LEDBAR_H
#define LEDBAR_H

#include <stdint.h>

class LedBar
{
public:
    LedBar(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4);

    void setup();

    void setLight(uint8_t light);

private:
    const uint8_t PINS[4];
};

#endif // LEDBAR_H
