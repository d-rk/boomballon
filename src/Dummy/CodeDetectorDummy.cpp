#include "CodeDetectorDummy.h"

#include <Arduino.h>

//-----------------------------------------------------------------------------

CodeDetectorDummy::CodeDetectorDummy(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5)
    : CodeDetector(pin1, pin2, pin3, pin4, pin5),
      serialCode(-1)
{

}

//-----------------------------------------------------------------------------

void CodeDetectorDummy::setup() {

}

//-----------------------------------------------------------------------------

uint8_t CodeDetectorDummy::readCode() {

    while(Serial.available()) {
        int byte = Serial.read();

        if (byte == 13) { // \n
            if (serialCode != -1) {
                uint8_t code = (uint8_t)serialCode;
                serialCode = -1;
                return code;
            }
        } else if (byte >= 48 && byte <= 57) { // chars [0-9]
            if (serialCode == -1) {
                serialCode = 0;
            }
            serialCode = 10*serialCode + (byte - 48);
        }
    }

    //no complete code read
    return currentCode;
}

//-----------------------------------------------------------------------------
