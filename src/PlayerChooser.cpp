#include "PlayerChooser.h"
#include <Constants.h>

#include <Arduino.h>

//-----------------------------------------------------------------------------

PlayerChooser::PlayerChooser(uint8_t code2Players, uint8_t code3Players, uint8_t code4Players, uint8_t code5Players)
    : CODES{code2Players, code3Players, code4Players, code5Players},
      numPlayers(-1),
      codeDetector(0),
      redLed(0),
      greenLed(0),
      timeLastBlink(0),
      TIME_WAIT_MS(2500)
{
}

//-----------------------------------------------------------------------------

void PlayerChooser::setup(CodeDetector* codeDetector, Led* redLed, Led* greenLed) {
    this->codeDetector = codeDetector;
    this->redLed = redLed;
    this->greenLed = greenLed;
}

//-----------------------------------------------------------------------------

bool PlayerChooser::detectNumPlayers() {

    if (codeDetector->codeChanged() && numPlayers == -1) {

        uint8_t code = codeDetector->getActiveCode();
        uint8_t nPlayers = 2;

        for (const uint8_t playerCode : CODES) {
            if (code == playerCode) {
                //numPlayers detected
                Serial.print("NumPlayers: ");
                Serial.println(nPlayers);
                numPlayers = nPlayers;
                return true;
            }
            nPlayers++;
        }

        if (numPlayers == -1) {
            //player placed a wrong card in the slot.
            if (code != CODE_NONE) {
                redLed->blink(2, 300, true);
                redLed->setOn(true);
            } else {
                redLed->setOn(false);
            }
        }
    }

    if (numPlayers != -1) {

        if ((millis() - timeLastBlink) >= (TIME_WAIT_MS + numPlayers*300)) {
            timeLastBlink = millis();
            redLed->setOn(true);
            greenLed->blink(numPlayers, 300, true);
        }

        return true;
    } else {
        return false;
    }
}

//-----------------------------------------------------------------------------

int8_t PlayerChooser::getNumPlayers() {
    return numPlayers;
}

//-----------------------------------------------------------------------------
