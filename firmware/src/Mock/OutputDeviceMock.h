#ifndef OUTPUTDEVICEMOCK_H
#define OUTPUTDEVICEMOCK_H

#include <Devices/OutputDevice.h>

/**
 * @brief Mock OutputDevice that reports the balloon volume on the serial
 * console instead of driving the pump and valve, so inflate/deflate behaviour
 * can be watched without any 12 V hardware attached. It keeps the real
 * OutputDevice volume model (timing, fill/deflate rates) and only replaces the
 * physical pin writes, which become no-ops.
 *
 * Wired in when the firmware is built with -D MOCK_OUTPUT_DEVICE.
 */
class OutputDeviceMock : public OutputDevice
{
public:
    OutputDeviceMock(uint8_t pinMotor, uint8_t pinValve);

    virtual void setup();

protected:
    // no hardware: swallow the physical writes the base class would perform
    virtual void writeMotor(uint8_t analogValue);
    virtual void writeValve(bool open);

    // print the resulting volume after each inflate/deflate operation
    virtual void afterApply(uint8_t positiveIntensity, uint8_t negativeIntensity);
};

#endif // OUTPUTDEVICEMOCK_H
