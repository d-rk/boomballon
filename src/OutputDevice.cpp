#include "OutputDevice.h"

#include <Constants.h>
#include <Arduino.h>

//-----------------------------------------------------------------------------

OutputDevice::OutputDevice(uint8_t pinMotor, uint8_t pinValve)
    : PIN_MOTOR(pinMotor),
      PIN_VALVE(pinValve),
      volume(0)
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

void OutputDevice::apply(int8_t value) {

    if (value < 0) {
        applyPositive(0);
        applyNegative(-2 * value);
    } else {
        applyPositive(2 * value);
        applyNegative(0);
    }

    volume += value;
}

//-----------------------------------------------------------------------------

void OutputDevice::reset() {
    for (int i=0; i < 100; i++) {
        applyPositive(255);
        delay(50);
    }
    applyPositive(0);

    for (int i=0; i < 100; i++) {
        applyNegative(255);
        delay(50);
    }
    applyNegative(0);

    volume = 0;
}

//-----------------------------------------------------------------------------

void OutputDevice::applyPositive(uint8_t value) {
    analogWrite(PIN_MOTOR, value);
}

//-----------------------------------------------------------------------------

void OutputDevice::applyNegative(uint8_t value) {
    analogWrite(PIN_VALVE, value);
}

//-----------------------------------------------------------------------------

