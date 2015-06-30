#include "CodeDetector.h"

#include <Arduino.h>

#include <Constants.h>

//-----------------------------------------------------------------------------

CodeDetector::CodeDetector(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5)
    : CODE_PINS{pin1, pin2, pin3, pin4, pin5},
      MIN_THRESHOLD(500),
      MAX_THRESHOLD(600),
      TIME_WAIT_MS(200),
      activeCode(0),
      activeCodeMirror(0),
      currentCode(0),
      timeLastCodeChange(0)
{

}

//-----------------------------------------------------------------------------

void CodeDetector::setup() {

}

//-----------------------------------------------------------------------------

/**
 * Function reads the current code from the pins and check if the code has changed.
 */
bool CodeDetector::codeChanged(bool ignoreBorderCode) {

  unsigned long time = millis();
  uint8_t code = readCode();

  if (code != currentCode) {
    //remember the time the code changed
    currentCode = code;
    timeLastCodeChange = time;
  }

  //only if code changed AND enough time has passed change the activeCode
  if (currentCode != activeCode && (time - timeLastCodeChange) >= TIME_WAIT_MS) {
    activeCode = currentCode;

    //generate mirror invariant code
    activeCodeMirror = 0;
    uint8_t numPins = sizeof(CODE_PINS) / sizeof(CODE_PINS[0]);

    for (uint8_t i = 0; i < numPins; i++) {
      activeCodeMirror |= ((activeCode >> i) & 1) << (numPins - i - 1);
    }

    if (activeCode < activeCodeMirror) {
      activeCodeMirror = activeCode;
    }

    Serial.print("New Code: ");
    Serial.print(activeCode);
    Serial.print(" ");
    Serial.println(activeCodeMirror);

    if (!ignoreBorderCode) {
        return true;
    } else {
        // 0 and 31 should be ignored
        return (activeCode != CODE_NONE) && (activeCode != CODE_ALL);
    }

  } else {
    return false;
  }
}

//-----------------------------------------------------------------------------

uint8_t CodeDetector::readCode() {
    uint8_t code = currentCode;
    uint8_t i    = 0;

    //read current code value
    for (const uint8_t& pin : CODE_PINS) {
      int value = analogRead(pin);

      if (value <= MIN_THRESHOLD) {
        code &= ~(1 << i); //set i-th bit to 0
      } else if (value >= MAX_THRESHOLD) {
        code |= 1 << i; //set i-th bit to 1
      }

      #ifdef DEBUG
      rawValue[i] = value;
      #endif
      i++;
    }

    return code;
}

//-----------------------------------------------------------------------------

/**
 * Return the active code.
 */
uint8_t CodeDetector::getActiveCode() {
  return activeCodeMirror;
}

//-----------------------------------------------------------------------------

#ifdef DEBUG
/**
 * Debug function to print the bits of a code.
 */
void CodeDetector::printCodeBits(uint8_t code) {
  for (int i=NUM_PINS-1; i >= 0; i--) {
    Serial.print((code >> i) & 1); //print i-th bit
  }
  Serial.println();
}

void CodeDetector_printRawValues() {
  for (int i=0; i < NUM_PINS; i++) {
    Serial.print(rawValue[i]);
    Serial.print(",");
  }
  Serial.println();
}

#endif
