#ifndef CODEDETECTOR_H
#define CODEDETECTOR_H

#include <stdint.h>

class CodeDetector
{
public:
    CodeDetector(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5);

    virtual void setup();

    bool codeChanged(bool ignoreBorderCode = false);
    uint8_t getActiveCode();

protected:
    virtual uint8_t readCode();

protected:
    // pins from which we will read the code
    const uint8_t CODE_PINS[5];

    // threshold for detection of LIGHT, NO_LIGHT states
    const int MIN_THRESHOLD;
    const int MAX_THRESHOLD;

    // minimum time that needs to pass so that a codeChange is valid
    const unsigned long TIME_WAIT_MS;

    // variables used internally
    uint8_t activeCode;
    uint8_t activeCodeMirror;
    uint8_t currentCode;
    uint64_t timeLastCodeChange;

    #ifdef DEBUG
    uint8_t rawValue[] = {0, 0, 0, 0, 0};
    #endif
};

#endif // CODEDETECTOR_H
