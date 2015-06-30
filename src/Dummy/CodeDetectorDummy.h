#ifndef CODEDETECTORDUMMY_H
#define CODEDETECTORDUMMY_H

#include <CodeDetector.h>

class CodeDetectorDummy : public CodeDetector
{
public:
    CodeDetectorDummy(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5);

    virtual void setup();

protected:
    virtual uint8_t readCode();
    int8_t serialCode;
};

#endif // CODEDETECTORDUMMY_H
