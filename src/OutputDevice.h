#ifndef OUTPUTDEVICE_H
#define OUTPUTDEVICE_H

#include <stdint.h>

class OutputDevice
{
public:
    OutputDevice(uint8_t PIN_MOTOR, uint8_t PIN_VALVE);
    virtual ~OutputDevice();

    virtual void setup();

    void apply(int8_t value);

    void reset();

protected:
    virtual void applyPositive(uint8_t value);
    virtual void applyNegative(uint8_t value);

public:
    const uint8_t PIN_MOTOR;
    const uint8_t PIN_VALVE;
    int volume;
};

#endif // OUTPUTDEVICE_H
