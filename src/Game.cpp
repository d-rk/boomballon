#include "Game.h"

#include <Constants.h>
#include <Player.h>
#include <OutputDevice.h>
#include <Cards/Card.h>

#include <Arduino.h>

//-----------------------------------------------------------------------------

/**
 * @brief Game::Game Constructor.
 */
Game::Game()
    : started(false),
      gameEnded(false),
      numPlayers(-1),
      currentPlayer(0),
      codeDetector(0),
      redLed(0),
      greenLed(0)
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
 * @brief Game::setup Setup the component.
 * @param codeDetector object used for code detection.
 * @param outputDevice device to apply played cards to.
 * @param redLed red status led.
 * @param greenLed green status led.
 */
void Game::setup(CodeDetector* codeDetector, OutputDevice* outputDevice, Led* redLed, Led* greenLed) {
    this->codeDetector = codeDetector;
    this->redLed = redLed;
    this->greenLed = greenLed;
    Card::output = outputDevice;
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
void Game::start(int8_t numPlayers) {
    this->started = true;
    this->gameEnded = false;
    this->numPlayers = numPlayers;

    //create player objects
    if (currentPlayer != 0) {
        delete currentPlayer;
    }

    currentPlayer = new Player(numPlayers); //last player
    currentPlayer = currentPlayer->nextPlayer; //first player
    printf("=== Turn changed to player %1d ===\n", currentPlayer->id);

    redLed->setOn(false);
    greenLed->setOn(true);
}

//-----------------------------------------------------------------------------

/**
 * @brief Game::loop the game loop.
 * Everything that happens during the game is handled here.
 */
void Game::loop() {

    bool codeChanged = codeDetector->codeChanged(true);

    if (gameEnded) {
        if (codeDetector->getActiveCode() == CODE_ALL) {
            printf("=== RESTART ===\n");
            start(numPlayers);
        }
        return;
    }

    if (codeChanged) {
        redLed->setOn(true);
        greenLed->setOn(false);
        Card::playCard(codeDetector->getActiveCode(), currentPlayer);
    }

    currentPlayer->loop();

    if (Card::output->volume > 100.0f) {
        Card::output->reset();
        gameEnded = true;
        printf("==  Player %1d lost the game!\n", currentPlayer->id);
        redLed->blink(5, 600, true);
        return;
    }

    if (codeChanged) {
        currentPlayer = currentPlayer->nextPlayer;
        redLed->setOn(false);
        greenLed->setOn(true);
        Card::output->apply(0);
        printf("New volume is ");
        Serial.println(Card::output->volume);
        printf("=== Turn changed to player %1d ===\n", currentPlayer->id);
    }
}

//-----------------------------------------------------------------------------


