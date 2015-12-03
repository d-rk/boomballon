#include "OutputDevice.h"

#include <Tasks/TaskScheduler.h>
#include <Constants.h>
#include <Arduino.h>

//-----------------------------------------------------------------------------

OutputDevice* OutputDevice::instance = NULL;

//-----------------------------------------------------------------------------

OutputDevice::OutputDevice(uint8_t pinMotor, uint8_t pinValve)
    : PIN_MOTOR(pinMotor),
      PIN_VALVE(pinValve),
      FILL_TIME_MIN(8000),
      FILL_TIME_MAX(10000),
      MOTOR_MIN(180),
      DEFLATE_TIME(7500),
      DEFLATE_CYCLE_MAX(200),
      lastValueChange(0),
      currentValue(0),
      volume(0.0f)
{

}

//-----------------------------------------------------------------------------

OutputDevice::~OutputDevice() {

}

//-----------------------------------------------------------------------------

void OutputDevice::setup() {
    pinMode(PIN_MOTOR, OUTPUT);
    pinMode(PIN_VALVE, OUTPUT);
}

//-----------------------------------------------------------------------------

/**
 * @brief apply apply volume change with given intensity.
 * @param volumeChangePercent volume change in percent i.e. in range [-100, 100]
 * @param intensity intensity with which the change is performed. (range [1, 255])
 * @param force force volume change even if the model reaches 100%.
 */
void OutputDevice::apply(int8_t volumeChangePercent, uint8_t intensity, bool force) {

    //intensity has to be at least 1
    intensity = (intensity > 0) ? intensity : 1;

    uint16_t durationMs = getTimeForVolumeChange(volumeChangePercent, intensity);

    if (volumeChangePercent > 0) {
        applyIntensities(intensity, 0, durationMs, force);
    } else {
        applyIntensities(0, intensity, durationMs, force);
    }
}

//-----------------------------------------------------------------------------

void OutputDevice::reset() {

    apply(20, 255, true);
    apply(-100, 255, true);
    apply(-60, 255, true);

    volume = 0.0f;
}

//-----------------------------------------------------------------------------


void OutputDevice::applyIntensities(uint8_t positiveIntensity, uint8_t negativeIntensity, uint16_t durationMs, bool force)
{
    uint8_t stepMs = 5;

    //first determine which value to write for positive itensity (i.e. the motor)
    const float motorFactor = (255 - MOTOR_MIN) / 255.0f;
    uint8_t analogValuePositive = (positiveIntensity > 0) ? (MOTOR_MIN + (uint8_t)(motorFactor * positiveIntensity)) : 0;
    float volumeIncrementPerMs = getVolumeIncrement(positiveIntensity, stepMs);

    //second determine factors needed to write negative intensity (i.e. the valve)
    // HIGH  ------        ------        ------
    //             |      |      |      |      |
    //             |      |      |      |      |
    //  LOW         ------        ------        ------
    //
    //       |------------| DEFLATE_CYCLE_MS
    //
    uint8_t  deflateCycleMs = (uint8_t)((1.0f - ((negativeIntensity-1) / 254.0f)) * DEFLATE_CYCLE_MAX);
    uint8_t  deflateHighMs  = (uint8_t)(deflateCycleMs * 0.5f);
    uint8_t  deflateState   = (negativeIntensity == 0) ? LOW : HIGH;
    uint16_t lastDeflateStateChange = 0;
    float volumeDecrementPerMs = getVolumeDecrement(negativeIntensity, stepMs);

    //loop to apply the changes
    for (uint16_t passed = 0; passed < durationMs; passed += stepMs) {
        //set motor value
        analogWrite(PIN_MOTOR, analogValuePositive);

        //change deflate state if needed
        if (negativeIntensity > 0 && negativeIntensity < 255 && (passed - lastDeflateStateChange) > deflateHighMs) {
            deflateState = (deflateState == LOW) ? HIGH : LOW;
            lastDeflateStateChange = passed;
        }

        //set valve value
        digitalWrite(PIN_VALVE, deflateState);

        //wait certain time
        delay(stepMs);

        //compute volume change for internal volume model
        volume += volumeIncrementPerMs;
        volume -= volumeDecrementPerMs;

        if (!force && volume > 100.0f) {
            break;
        } else if (volume < 0.0f) {
            volume = 0.0f;
        }

        TaskScheduler::instance->iterate();
    }

    //make sure everything is off afterwards
    analogWrite(PIN_MOTOR, 0);
    digitalWrite(PIN_VALVE, LOW);
}

//-----------------------------------------------------------------------------

/**
 * @brief OutputDevice::getVolumeIncrement compute volume change when positive intensity is applied for given duration.
 * @param intensity intensity that is applied. In range [0, 255].
 * @param durationMs duration for which value is applied in milliseconds.
 * @return the volume change in percent >= 0.0f.
 */
float OutputDevice::getVolumeIncrement(uint8_t intensity, uint16_t durationMs) {

    if (intensity == 0) {
        return 0.0f;
    }

    // FILL operation
    // value can be in range [1, 255]. First map it to [0, 1]
    float mapped =  (intensity - 1) / 254.0f;

    //fillTime is the time needed to get the ballon to 100% with this value
    float fillTime = FILL_TIME_MIN + (1.0f - mapped) * (FILL_TIME_MAX - FILL_TIME_MIN);

    //return change in volume in percent
    return (durationMs / fillTime) * 100.0f;
}

//-----------------------------------------------------------------------------

/**
 * @brief OutputDevice::getVolumeDecrement compute volume change when negative intensity is applied for given duration.
 * @param intensity intensity that is applied. In range [0, 255].
 * @param durationMs duration for which value is applied in milliseconds.
 * @return the volume change in percent.
 */
float OutputDevice::getVolumeDecrement(uint8_t intensity, uint16_t durationMs) {

    if (intensity == 0) {
        return 0.0f;
    }

    // DEFLATE operation
    // value can be in range [1, 255]. First map it to [0, 1]
    float mapped =  (intensity - 1) / 254.0f;

    uint8_t deflateCycleMs = (uint8_t)((1.0f - mapped) * DEFLATE_CYCLE_MAX);
    float deflateTime = getDeflateTime(deflateCycleMs);

    //return change in volume in percent
    return (durationMs / deflateTime) * 100.0f;
}

//-----------------------------------------------------------------------------

/**
 * @brief OutputDevice::getTimeForVolumeChange Get time needed to perform given volume change.
 * @param volumeChangePercent volume change in percent i.e. in range [-100, 100]
 * @param intensity intensity with which the change is performed. (range [1, 255])
 * @return time in ms to perform the volume change.
 */
uint16_t OutputDevice::getTimeForVolumeChange(int8_t volumeChangePercent, uint8_t intensity) {

    if (volumeChangePercent >= 0) {
        // we want to increase the volume
        // intensity is in range [1, 255] .. map it to [0, 1]
        float mapped = (intensity - 1) / 254.0f;

        //fillTime is the time needed to get the ballon to 100% with this value
        float fillTime = FILL_TIME_MIN + (1.0f - mapped) * (FILL_TIME_MAX - FILL_TIME_MIN);

        //return time needed to fill given percentage.
        return (uint16_t) fillTime * (volumeChangePercent / 100.0f);

    } else if (volumeChangePercent < 0) {
        // we want to decrease the volume
        // intensity is in range [1, 255] .. map it to [0, 1]
        float mapped = (intensity - 1) / 254.0f;

        uint8_t deflateCycleMs = (uint8_t)((1.0f - mapped) * DEFLATE_CYCLE_MAX);
        float deflateTime = getDeflateTime(deflateCycleMs);

        //return time needed to fill given percentage.
        return (uint16_t) deflateTime * (-volumeChangePercent / 100.0f);
    }
}

//-----------------------------------------------------------------------------

/**
 * @brief OutputDevice::getDeflateTime Get time needed for a complete deflate.
 * @param deflateCycleMs time a deflate cycle takes.
 * @return time to deflate the ballon completely.
 *
 * @note if deflateCycleMs = 0 then the balloon will be deflated with a permanently opened valve.
 */
float OutputDevice::getDeflateTime(uint8_t deflateCycleMs) {

    float valveOpenedInterval;

    if (deflateCycleMs == 0) {
        //we just need the normal deflate time, since the valve is permanently opened.
        valveOpenedInterval = 1.0f;
    } else if (deflateCycleMs <= 10) {
        //we need a bit more. The cycle is (probably) so small that the valve does not close completely.
        valveOpenedInterval = 0.85f;
    } else {
        //the valve is only opened half the time.
        valveOpenedInterval = 0.5f;
    }

    return (DEFLATE_TIME / valveOpenedInterval);
}

//-----------------------------------------------------------------------------




