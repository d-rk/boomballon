#include "Game.h"

#include <Constants.h>
#include <Player.h>
#include <OutputDevice.h>
#include <Cards/Card.h>

#include <Arduino.h>

//-----------------------------------------------------------------------------

Game::Game()
    : MAX_VOLUME(500),
      started(false),
      gameEnded(false),
      numPlayers(-1),
      currentPlayer(0),
      codeDetector(0),
      redLed(0),
      greenLed(0)
{

}

//-----------------------------------------------------------------------------

Game::~Game()
{
    if (currentPlayer != 0) {
        delete currentPlayer;
    }
}

//-----------------------------------------------------------------------------

void Game::setup(CodeDetector* codeDetector, OutputDevice* outputDevice, Led* redLed, Led* greenLed) {
    this->codeDetector = codeDetector;
    this->redLed = redLed;
    this->greenLed = greenLed;
    Card::output = outputDevice;
}

//-----------------------------------------------------------------------------

bool Game::isStarted() {
    return started;
}

//-----------------------------------------------------------------------------

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
    Serial.print("--Turn Changed to: Player ");
    Serial.println(currentPlayer->id);

    redLed->setOn(false);
    greenLed->setOn(true);
}

//-----------------------------------------------------------------------------

void Game::loop() {

    bool codeChanged = codeDetector->codeChanged(true);

    if (gameEnded) {
        if (codeDetector->getActiveCode() == CODE_NONE) {
            Serial.println("- RESTART -");
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

    if (Card::output->volume > MAX_VOLUME) {
        Card::output->reset();
        gameEnded = true;
        Serial.print("--Player ");
        Serial.print(currentPlayer->id);
        Serial.println(" lost the game!");
        redLed->blink(5, 600, true);
        return;
    }

    if (codeChanged) {
        currentPlayer = currentPlayer->nextPlayer;
        redLed->setOn(false);
        greenLed->setOn(true);
        Card::output->apply(0);
        Serial.print("New Volume: ");
        Serial.println(Card::output->volume);
        Serial.print("--Turn Changed to: Player ");
        Serial.println(currentPlayer->id);
    }
}

//-----------------------------------------------------------------------------


