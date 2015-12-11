#include "Game.h"

#include <Constants.h>
#include <Player.h>
#include <Devices/OutputDevice.h>
#include <Devices/PiezoBuzzer.h>
#include <Devices/SevenSegmentDisplay.h>
#include <Cards/Card.h>
#include <Tasks/TaskScheduler.h>
#include <Helper/Diagnostic.h>

#include <Arduino.h>

//-----------------------------------------------------------------------------

/**
 * @brief Game::Game Constructor.
 */
Game::Game()
    : started(false),
      gameEnded(false),
      playJingle(true),
      waitCardRemoved(false),
      numPlayers(-1),
      currentPlayer(0)
{
}

//-----------------------------------------------------------------------------

/**
 * @brief Game::~Game Destructor.
 */
Game::~Game()
{
    if (currentPlayer != 0) {
        delete currentPlayer;
    }
}

//-----------------------------------------------------------------------------

/**
 * @brief Game::isStarted Check if game is started.
 * @return is it started?
 */
bool Game::isStarted() {
    return started;
}

//-----------------------------------------------------------------------------

/**
 * @brief Game::start Start the game.
 * @param numPlayers number of players that participate.
 */
void Game::start(int8_t numPlayers, bool playJingle) {
    printf("\n");
    printf("=================================\n");
    printf("=== Start game with %1d players ===\n", numPlayers);
    printf("=================================\n");
    this->started = true;
    this->gameEnded = false;
    this->playJingle = playJingle;
    this->numPlayers = numPlayers;
    this->waitCardRemoved = false;

    //create player objects
    if (currentPlayer != 0) {
        delete currentPlayer;
    }

    currentPlayer = new Player(numPlayers); //last player is current afterwards

    if (playJingle) {
        PiezoBuzzer::instance->setMelody(PiezoBuzzer::M_BEVERLY_HILLS, PiezoBuzzer::R_BEVERLY_HILLS);
        SevenSegmentDisplay::instance->setAnimation(ANIM_CIRCLE2, 75, false, false, 12);
        TaskScheduler::instance->loop();
    }

    changePlayer();
}

//-----------------------------------------------------------------------------

void Game::changePlayer() {
    currentPlayer = currentPlayer->nextPlayer; //first player
    printf("\n=== Turn changed to player %1d ===\n", currentPlayer->id);
    printf("\tFree Ram: %d bytes\n", freeRam());

    SevenSegmentDisplay::instance->setCharacter(0);
    delay(500);
    SevenSegmentDisplay::instance->setCharacter(CHAR_P);
    PiezoBuzzer::instance->playTone(NOTE_C1, 8, 1, 2);
    TaskScheduler::instance->loop();

    SevenSegmentDisplay::instance->setNumber(currentPlayer->id);
}

//-----------------------------------------------------------------------------

/**
 * @brief Game::loop the game loop.
 * Everything that happens during the game is handled here.
 */
void Game::loop() {

    bool codeChanged = CodeDetector::instance->codeChanged(true);

    if (gameEnded) {
        if (CodeDetector::instance->getActiveCode() == CODE_ALL) {
            printf("** Restarting game...\n");
            start(numPlayers, playJingle);
        }
        return;
    }

    if (codeChanged) {
        PiezoBuzzer::instance->playTone(NOTE_D6, 64, 1, 16);
        TaskScheduler::instance->loop();
        Card::playCard(CodeDetector::instance->getActiveCode(), currentPlayer);
        waitCardRemoved = true;
    }

    currentPlayer->loop(codeChanged, waitCardRemoved);
    SevenSegmentDisplay::instance->setNumber(currentPlayer->id);

    if (OutputDevice::instance->volume > 100.0f) {
        printf("\n** Player %1d lost the game!\n", currentPlayer->id);
        printf("** Perform reset...\n");
        OutputDevice::instance->reset();
        printf("** Remove card to restart game...");

        gameEnded = true;
        return;
    }

    if (waitCardRemoved && CodeDetector::instance->getActiveCode() == CODE_ALL) {
        waitCardRemoved = false;
        changePlayer();
        printf("\tCurrent volume: ");
        Serial.println(OutputDevice::instance->volume);
    }
}

//-----------------------------------------------------------------------------


