#include "OutputDeviceDummy.h"

#include <Constants.h>

//-----------------------------------------------------------------------------

OutputDeviceDummy::OutputDeviceDummy(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7, uint8_t pin8)
    : barGreen(pin1,pin2, pin3, pin4),
      barRed(pin5,pin6, pin7, pin8)
{

}

//-----------------------------------------------------------------------------

void OutputDeviceDummy::setup() {
    barRed.setup();
    barGreen.setup();
}

//-----------------------------------------------------------------------------

void OutputDeviceDummy::apply(int8_t value) {
    if (value < 0) {
        barGreen.setLight(0);
        barRed.setLight(-2 * value);
    } else {
        barGreen.setLight(2 * value);
        barRed.setLight(0);
    }
}

//-----------------------------------------------------------------------------
