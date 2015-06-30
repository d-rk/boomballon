#ifndef OUTPUTDEVICEDUMMY_H
#define OUTPUTDEVICEDUMMY_H

#include <Dummy/LedBar.h>
#include <OutputDevice.h>

class OutputDeviceDummy : public OutputDevice
{
public:
    OutputDeviceDummy(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7, uint8_t pin8);

    virtual void setup();

    virtual void apply(int8_t value);

private:
    LedBar barGreen;
    LedBar barRed;
};

#endif // OUTPUTDEVICEDUMMY_H
