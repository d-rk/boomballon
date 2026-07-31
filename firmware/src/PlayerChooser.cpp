#include "PlayerChooser.h"
#include <Constants.h>

#include <Devices/CodeDetector.h>
#include <Devices/PiezoBuzzer.h>
#include <Devices/SevenSegmentDisplay.h>
#include <Tasks/TaskScheduler.h>

#include <Arduino.h>

//-----------------------------------------------------------------------------

PlayerChooser* PlayerChooser::instance = NULL;

//-----------------------------------------------------------------------------

/**
 * @brief PlayerChooser::PlayerChooser Constructor.
 * @param code2Players code that defines player 1.
 * @param code2Players code that defines player 2 or a 2 player game.
 * @param code3Players code that defines player 3 or a 3 player game.
 * @param code4Players code that defines player 4 or a 4 player game.
 * @param code5Players code that defines player 5 or a 5 player game.
 */
PlayerChooser::PlayerChooser(uint8_t code1Players, uint8_t code2Players, uint8_t code3Players, uint8_t code4Players, uint8_t code5Players)
    : CODES{code1Players, code2Players, code3Players, code4Players, code5Players},
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
        uint8_t nPlayers = 1;

        for (const uint8_t playerCode : CODES) {
            if (nPlayers > 1 && code == playerCode) {
                //numPlayers detected
                #ifdef LOGGING_ENABLED
                printf("\n=== Selected %1d players ===\n\n", nPlayers);
                #endif
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

/**
 * @brief PlayerChooser::isPlayerCode check if given code is a player code.
 * @param code code from detector.
 * @return is it a valid player code?
 */
bool PlayerChooser::isPlayerCode(uint8_t code) {
    for (const uint8_t playerCode : CODES) {
        if (code == playerCode) {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------

/**
 * @brief PlayerChooser::playerId get player id (1-based)
 * @param code code from detector.
 * @return id of player.
 */
uint8_t PlayerChooser::playerId(uint8_t code) {
    uint8_t idx = 0;
    for (const uint8_t playerCode : CODES) {
        idx++;
        if (code == playerCode) {
            return idx;
        }
    }
    return 0;
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

