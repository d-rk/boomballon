#include "CodeDetectorMock.h"

#include <Constants.h>
#include <Arduino.h>

//-----------------------------------------------------------------------------

CodeDetectorMock::CodeDetectorMock(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5)
    : CodeDetector(pin1, pin2, pin3, pin4, pin5),
      serialCode(-1)
{

}

//-----------------------------------------------------------------------------

void CodeDetectorMock::setup() {
    currentCode = CODE_ALL;
    printf("[mock] code detector: type a card code + <enter> to feed it\n");
}

//-----------------------------------------------------------------------------

uint8_t CodeDetectorMock::readCode() {

    while(Serial.available()) {
        int byte = Serial.read();

        // echo the typed character so it is visible in the serial monitor
        if (byte == 13) {
            Serial.write('\n');
        } else if (byte >= 32 && byte <= 126) {
            Serial.write((char)byte);
        }

        if (byte == 13) { // \n
            if (serialCode != -1 && currentCode == CODE_ALL) {
                uint8_t code = (uint8_t)serialCode;
                serialCode = -1;
                return code;
            } else {
                //card has to be pulled out before new code is accepted.
                serialCode = -1;
                return CODE_ALL;
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
