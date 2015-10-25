#ifndef CODEDETECTOR_H
#define CODEDETECTOR_H

#include <stdint.h>

/**
 * @brief The CodeDetector class.
 *
 * Class to interpret values applied to a number of analog pins as an integer code.
 *
 */
class CodeDetector
{
public:
    CodeDetector(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5);

    virtual void setup();

    bool codeChanged(bool ignoreAllNoneCode = false);
    uint8_t getActiveCode();

    void setActiveCode(uint8_t code);

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

    // code that is seen as active from outside
    uint8_t activeCode;
    uint8_t activeCodeMirrored;

    // the code that actually is found currently
    // currentCode has to be on for a certain time until it is adopted by activeCode
    uint8_t currentCode;
    uint64_t timeLastCodeChange;

    #ifdef DEBUG
    uint8_t rawValue[] = {0, 0, 0, 0, 0};
    #endif

public:
    static CodeDetector* instance;
};

#endif // CODEDETECTOR_H
