#include "OutputDeviceDummy.h"

#include <Constants.h>

//-----------------------------------------------------------------------------

OutputDeviceDummy::OutputDeviceDummy(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5, uint8_t pin6, uint8_t pin7, uint8_t pin8)
    : OutputDevice(0, 0),
      barGreen(pin1, pin2, pin3, pin4),
      barRed(pin5, pin6, pin7, pin8)
{

}

//-----------------------------------------------------------------------------

void OutputDeviceDummy::setup() {
    barRed.setup();
    barGreen.setup();
}

//-----------------------------------------------------------------------------

void OutputDeviceDummy::applyPositive(uint8_t value) {
    barGreen.setLight(value);
}

//-----------------------------------------------------------------------------

void OutputDeviceDummy::applyNegative(uint8_t value) {
    barRed.setLight(value);
}

//-----------------------------------------------------------------------------
