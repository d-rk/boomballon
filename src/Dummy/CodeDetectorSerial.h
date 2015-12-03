#ifndef CODEDETECTORSERIAL_H
#define CODEDETECTORSERIAL_H

#include <Devices/CodeDetector.h>

class CodeDetectorSerial : public CodeDetector
{
public:
    CodeDetectorSerial(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5);

    virtual void setup();

protected:
    virtual uint8_t readCode();
    int8_t serialCode;
};

#endif // CODEDETECTORSERIAL_H
