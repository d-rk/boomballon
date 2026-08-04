#include "OutputDeviceMock.h"

#include <Constants.h>
#include <Arduino.h>

//-----------------------------------------------------------------------------

OutputDeviceMock::OutputDeviceMock(uint8_t pinMotor, uint8_t pinValve)
    : OutputDevice(pinMotor, pinValve)
{

}

//-----------------------------------------------------------------------------

void OutputDeviceMock::setup() {
    // no pins to configure; announce and show the starting volume
    printf("[mock] output device: printing volume to serial (no motor/valve)\n");
    afterApply(0, 0);
}

//-----------------------------------------------------------------------------

void OutputDeviceMock::writeMotor(uint8_t /*analogValue*/) {
    // no hardware attached
}

//-----------------------------------------------------------------------------

void OutputDeviceMock::writeValve(bool /*open*/) {
    // no hardware attached
}

//-----------------------------------------------------------------------------

void OutputDeviceMock::afterApply(uint8_t positiveIntensity, uint8_t negativeIntensity) {

    const char* op = (positiveIntensity > 0) ? "inflate"
                   : (negativeIntensity > 0) ? "deflate"
                                             : "hold   ";
    uint8_t intensity = (positiveIntensity > 0) ? positiveIntensity : negativeIntensity;

    uint8_t vol = (volume < 0.0f) ? 0 : (volume > 100.0f) ? 100 : (uint8_t)(volume + 0.5f);

    // 20-cell bar, one cell per 5 %
    char bar[21];
    uint8_t filled = vol / 5;
    for (uint8_t i = 0; i < 20; i++) {
        bar[i] = (i < filled) ? '#' : '.';
    }
    bar[20] = '\0';

    printf("[mock] %s i=%3u  volume %3u%% [%s]\n", op, intensity, vol, bar);
}

//-----------------------------------------------------------------------------
