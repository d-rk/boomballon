#include "CodeDetector.h"

#include <Arduino.h>
#include <Constants.h>

//-----------------------------------------------------------------------------

CodeDetector* CodeDetector::instance = NULL;

//-----------------------------------------------------------------------------

/**
 * @brief CodeDetector::CodeDetector Constructor.
 * @param pin1 first pin for the code detection.
 * @param pin2 second pin for the code detection.
 * @param pin3 third pin for the code detection.
 * @param pin4 fourth pin for the code detection.
 * @param pin5 fifth pin for the code detection.
 */
CodeDetector::CodeDetector(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5)
    : CODE_PINS{pin1, pin2, pin3, pin4, pin5},
      MIN_THRESHOLD(20),
      MAX_THRESHOLD(28),
      TIME_WAIT_MS(200),
      activeCode(0),
      currentCode(0),
      timeLastCodeChange(0)
{
}

//-----------------------------------------------------------------------------

/**
 * @brief CodeDetector::setup Initial setup for the component.
 */
void CodeDetector::setup() {
    codeChanged();
}

//-----------------------------------------------------------------------------

/**
 * @brief CodeDetector::codeChanged function reads the current code from the pins and check if the code has changed.
 * @param ignoreAllNoneCode if true, function returns false if there was a code change to CODE_ALL or CODE_NONE.
 * @return true if the activeCode changed, false otherwise.
 */
bool CodeDetector::codeChanged(bool ignoreAllNoneCode) {

    unsigned long time = millis();
    uint8_t code = readCode();

    if (code != currentCode) {
        //remember the time the code changed
        currentCode = code;
        timeLastCodeChange = time;
    }

    //only if code changed AND enough time has passed change the activeCode
    if (currentCode != activeCode && currentCode != activeCodeMirrored && (time - timeLastCodeChange) >= TIME_WAIT_MS) {

        setActiveCode(currentCode);

        if (!ignoreAllNoneCode) {
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

/**
 * @brief CodeDetector::readCode read code from analog pins and convert it to integer.
 * @return integer representation of the code which is currently applied on the pins.
 */
uint8_t CodeDetector::readCode() {
    uint8_t code = currentCode;
    uint8_t i    = 0;

    //read current code value
    for (const uint8_t& pin : CODE_PINS) {
        int value = analogRead(pin);

        switch (i) {
            case 0: value = map(value,  20, 950, 0, 100); break;
            case 1: value = map(value,  20, 910, 0, 100); break;
            case 2: value = map(value,  20, 885, 0, 100); break;
            case 3: value = map(value,  20, 900, 0, 100); break;
            case 4: value = map(value,  20, 895, 0, 100); break;
            default: break;
        }

        if (value <= MIN_THRESHOLD) {
            code &= ~(1 << i); //set i-th bit to 0
        } else if (value >= MAX_THRESHOLD) {
            code |= 1 << i; //set i-th bit to 1
        }

#ifdef DETECTOR_CALIBRATION
        rawValue[i] = value;
#endif
        i++;
    }

    return code;
}

//-----------------------------------------------------------------------------

/**
 * @brief CodeDetector::getActiveCode return the active code.
 * @return active code (mirror invariant)
 */
uint8_t CodeDetector::getActiveCode() {
    return activeCode;
}

//-----------------------------------------------------------------------------

/**
 * @brief CodeDetector::setActiveCode set the active code.
 * @param code code to set.
 * @note the given might be mirrored before being stored internally.
 */
void CodeDetector::setActiveCode(uint8_t code) {

    //generate mirror invariant code
    activeCodeMirrored = 0;
    uint8_t numPins = sizeof(CODE_PINS) / sizeof(CODE_PINS[0]);

    for (uint8_t i = 0; i < numPins; i++) {
        activeCodeMirrored |= ((code >> i) & 1) << (numPins - i - 1);
    }

    //activeCode is always the smaller code of [code, mirroredCode]
    if (code < activeCodeMirrored) {
        activeCode = code;
    } else {
        activeCode = activeCodeMirrored;
        activeCodeMirrored = code;
    }

    #ifndef DETECTOR_CALIBRATION
    printf("\tActive Code: %2d (%2d)\n", activeCode, activeCodeMirrored);
    #endif
}


//-----------------------------------------------------------------------------

#ifdef DETECTOR_CALIBRATION
/**
 * @brief CodeDetector::printCodeBits debug function to print the bits of a code.
 * @param code code from which to print the single bits.
 */
void CodeDetector::printCodeBits(uint8_t code) {

    uint8_t numPins = sizeof(CODE_PINS) / sizeof(CODE_PINS[0]);

    for (int i=numPins-1; i >= 0; i--) {
        Serial.print((code >> i) & 1); //print i-th bit
    }
    Serial.println();
}

//-----------------------------------------------------------------------------

/**
 * @brief CodeDetector_printRawValues Print raw values to serial.
 */
void CodeDetector::printRawValues(bool codeChanged, bool displayChangesOnly) {

    if (!codeChanged && displayChangesOnly) {
        return;
    }

    uint8_t numPins = sizeof(CODE_PINS) / sizeof(CODE_PINS[0]);

    char c[3];

    for (int i=0; i < numPins; i++) {
        printf("%3d ", rawValue[i]);
    }

    Serial.print(F(" |  "));


    for (int i=0; i < numPins; i++) {
        if (rawValue[i] <= MIN_THRESHOLD) {
            Serial.print(F("0 "));
        } else if (rawValue[i] >= MAX_THRESHOLD) {
            Serial.print(F("1 "));
        }
    }

    printf("  |  %2d = %2d\n", activeCode, activeCodeMirrored);
}

//-----------------------------------------------------------------------------

#endif
