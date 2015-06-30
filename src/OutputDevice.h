#ifndef OUTPUTDEVICE_H
#define OUTPUTDEVICE_H

#include <stdint.h>

class OutputDevice
{
public:
    OutputDevice();
    virtual ~OutputDevice();

    virtual void setup();

    virtual void apply(int8_t value);
};

#endif // OUTPUTDEVICE_H
