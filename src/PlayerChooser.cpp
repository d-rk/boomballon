#include "PlayerChooser.h"
#include <Constants.h>

#include <Devices/CodeDetector.h>
#include <Devices/PiezoBuzzer.h>
#include <Devices/SevenSegmentDisplay.h>
#include <Tasks/TaskScheduler.h>

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
      numPlayers(-1)
{
}

//-----------------------------------------------------------------------------

/**
 * @brief PlayerChooser::detectNumPlayers Detect number of players that should be used for the game.
 * @return was a code for a number of players detected?
 */
bool PlayerChooser::detectNumPlayers() {

    if (CodeDetector::instance->codeChanged() && numPlayers == -1) {

        uint8_t code = CodeDetector::instance->getActiveCode();
        uint8_t nPlayers = 2;

        for (const uint8_t playerCode : CODES) {
            if (code == playerCode) {
                //numPlayers detected
                printf("NumPlayers: %2d\n", nPlayers);
                numPlayers = nPlayers;
                //play animation once
                PiezoBuzzer::instance->playTone(NOTE_DS8, 16, 1, 16);
                SevenSegmentDisplay::instance->setAnimation(getAnimation(numPlayers), 600, false, false);
                TaskScheduler::instance->loop();

                //start playing it until someone else stops it.
                SevenSegmentDisplay::instance->setAnimation(getAnimation(numPlayers), 600, false, false, 0);
                return true;
            }
            nPlayers++;
        }

        if (numPlayers == -1) {
            //player placed a wrong card in the slot.
            if (code != CODE_ALL && code != CODE_NONE) {
                PiezoBuzzer::instance->playTone(PLAY_ERROR, 4);
                SevenSegmentDisplay::instance->setCharacter(CHAR_MINUS);
                TaskScheduler::instance->loop();
            } else {
                SevenSegmentDisplay::instance->setCharacter(CHAR_DOT);
            }
        }
    }

    if (numPlayers != -1) {
        //apply changes to running tasks(numPlayers animation) without blocking
        TaskScheduler::instance->iterate();
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

DisplayAnimation PlayerChooser::getAnimation(uint8_t numPlayers) {
    switch(numPlayers) {
        case 2: return ANIM_2P;
        case 3: return ANIM_3P;
        case 4: return ANIM_4P;
        case 5: return ANIM_5P;
        default: return ANIM_ERROR;
    }
}

//-----------------------------------------------------------------------------

