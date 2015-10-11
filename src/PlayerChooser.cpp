#include "PlayerChooser.h"
#include <Constants.h>

#include <Arduino.h>

//-----------------------------------------------------------------------------

/**
 * @brief PlayerChooser::PlayerChooser Constructor.
 * @param code2Players code that defines a 2 player game.
 * @param code3Players code that defines a 3 player game.
 * @param code4Players code that defines a 4 player game.
 * @param code5Players code that defines a 5 player game.
 */
PlayerChooser::PlayerChooser(uint8_t code2Players, uint8_t code3Players, uint8_t code4Players, uint8_t code5Players)
    : CODES{code2Players, code3Players, code4Players, code5Players},
      numPlayers(-1),
      codeDetector(0),
      redLed(0),
      greenLed(0),
      timeLastBlink(0),
      TIME_WAIT_MS(3000)
{
}

//-----------------------------------------------------------------------------

/**
 * @brief PlayerChooser::setup setup the component.
 * @param codeDetector object used for code detection.
 * @param redLed red led.
 * @param greenLed green led.
 */
void PlayerChooser::setup(CodeDetector* codeDetector, Led* redLed, Led* greenLed) {
    this->codeDetector = codeDetector;
    this->redLed = redLed;
    this->greenLed = greenLed;
}

//-----------------------------------------------------------------------------

/**
 * @brief PlayerChooser::detectNumPlayers Detect number of players that should be used for the game.
 * @return was a code for a number of players detected?
 */
bool PlayerChooser::detectNumPlayers() {

    if (codeDetector->codeChanged() && numPlayers == -1) {

        uint8_t code = codeDetector->getActiveCode();
        uint8_t nPlayers = 2;

        for (const uint8_t playerCode : CODES) {
            if (code == playerCode) {
                //numPlayers detected
                printf("NumPlayers: %2d", nPlayers);
                numPlayers = nPlayers;
                return true;
            }
            nPlayers++;
        }

        if (numPlayers == -1) {
            //player placed a wrong card in the slot.
            if (code != CODE_ALL && code != CODE_NONE) {
                redLed->blink(2, 500, true);
                redLed->setOn(true);
            }
        }
    }

    if (numPlayers != -1) {

        if ((millis() - timeLastBlink) >= (TIME_WAIT_MS + numPlayers*500*2)) {
            timeLastBlink = millis();
            redLed->setOn(true);
            greenLed->blink(numPlayers, 500, true);
        }

        return true;
    } else {
        return false;
    }
}

//-----------------------------------------------------------------------------

/**
 * @brief PlayerChooser::getNumPlayers Get the number of players.
 * @return number of players for the game.
 */
int8_t PlayerChooser::getNumPlayers() {
    return numPlayers;
}

//-----------------------------------------------------------------------------

/**
 * @brief PlayerChooser::setNumPlayers Set the number of players.
 * @param numPlayers number of players for the game.
 */
void PlayerChooser::setNumPlayers(int8_t numPlayers) {
    this->numPlayers = numPlayers;
}

//-----------------------------------------------------------------------------

