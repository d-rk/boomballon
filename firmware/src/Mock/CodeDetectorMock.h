#ifndef CODEDETECTORMOCK_H
#define CODEDETECTORMOCK_H

#include <Devices/CodeDetector.h>

/**
 * @brief Mock CodeDetector that reads card codes from the serial console
 * instead of the photoresistors, so the game can be exercised without the
 * physical card reader attached. Type a code followed by <enter> to "insert"
 * a card; send an empty line (or any non-code) to "pull it out" again.
 *
 * Wired in when the firmware is built with -D MOCKED_DEVICES.
 */
class CodeDetectorMock : public CodeDetector
{
public:
    CodeDetectorMock(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5);

    virtual void setup();

protected:
    virtual uint8_t readCode();
    int8_t serialCode;
};

#endif // CODEDETECTORMOCK_H
