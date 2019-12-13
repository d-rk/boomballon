#ifndef OUTPUTDEVICE_H
#define OUTPUTDEVICE_H

#include <stdint.h>

class OutputDevice
{
public:
    OutputDevice(uint8_t PIN_MOTOR, uint8_t PIN_VALVE);
    virtual ~OutputDevice();

    virtual void setup();

    void apply(int8_t volumeChangePercent, uint8_t intensity, bool force = false);

    void applyIntensities(uint8_t positiveIntensity, uint8_t negativeIntensity, uint16_t durationMs, bool force);

    void reset();

    void setValveOpen(bool open);

protected:
    virtual float getVolumeIncrement(uint8_t intensity, uint16_t durationMs);
    virtual float getVolumeDecrement(uint8_t intensity, uint16_t durationMs);

    virtual uint16_t getTimeForVolumeChange(int8_t volumeChangePercent, uint8_t intensity);

    float getDeflateTime(uint8_t deflateCycleMs);

public:
    const uint8_t PIN_MOTOR;
    const uint8_t PIN_VALVE;

    const uint16_t FILL_TIME_MIN;
    const uint16_t FILL_TIME_MAX;
    const uint8_t  MOTOR_MIN;
    const float    FILL_TIME_FACTOR_NO_PWM;

    const uint16_t DEFLATE_TIME;
    const uint8_t  DEFLATE_CYCLE_MAX;

    uint64_t lastValueChange;
    int8_t currentValue;
    float volume;

    static OutputDevice* instance;
};

#endif // OUTPUTDEVICE_H
