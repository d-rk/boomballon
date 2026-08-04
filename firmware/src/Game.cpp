#include "Game.h"

#include <Constants.h>
#include <Player.h>
#include <Devices/OutputDevice.h>
#include <Devices/PiezoBuzzer.h>
#include <Devices/SevenSegmentDisplay.h>
#include <Cards/Card.h>
#include <Tasks/TaskScheduler.h>
#include <Helper/Diagnostic.h>
#include <PlayerChooser.h>

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
      currentPlayer(0),
      currentCard(0)
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

    if (currentCard != 0) {
        delete currentCard;
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

    OutputDevice::instance->setValveOpen(false);

    //create player objects
    if (currentPlayer != 0) {
        delete currentPlayer;
    }

    if (currentCard != 0) {
        delete currentCard;
        currentCard = 0;
    }

    currentPlayer = new Player(numPlayers); //last player is current afterwards

    if (playJingle) {
        if (random(2) == 0) {
            PiezoBuzzer::instance->setMelody(PiezoBuzzer::M_BEVERLY_HILLS, PiezoBuzzer::R_BEVERLY_HILLS);
            SevenSegmentDisplay::instance->setAnimation(ANIM_CIRCLE2, 75, false, false, 12);
        } else {
            PiezoBuzzer::instance->setMelody(PiezoBuzzer::M_ATEAM, PiezoBuzzer::R_ATEAM);
            SevenSegmentDisplay::instance->setAnimation(ANIM_CIRCLE2, 75, false, false, 12);
        }

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

    bool newCardInserted = false;

    if (codeChanged) {

        uint8_t code = CodeDetector::instance->getActiveCode();

        if (currentCard == 0) {
            //waiting for a card to be played
            currentCard = Card::playCard(code);

            if (currentCard != 0) {
                //a valid card was inserted
                acceptedFeedback();

                if (!currentCard->needsPlayerSelection()) {
                    //directly play card
                    currentCard->attach(currentPlayer);
                    newCardInserted = true;
                    waitCardRemoved = true;
                    currentCard = 0;
                } else {
                    printf("\tWaiting for a player-card to attach %s...\n", currentCard->cardName());
                }
            } else {
                //wrong card inserted
                errorFeedback();
                printf("\tError: not a valid game card.\n");
            }
        } else {
            //waiting for a player card
            if (PlayerChooser::instance->isPlayerCode(code)) {
                //player card was inserted
                PiezoBuzzer::instance->playTone(NOTE_D6, 64, 1, 16);
                TaskScheduler::instance->loop();

                Player* targetPlayer = getPlayer(PlayerChooser::instance->playerId(code));

                if (targetPlayer != 0) {
                    //play current card on target player
                    acceptedFeedback();
                    currentCard->attach(targetPlayer);
                    newCardInserted = true;
                    waitCardRemoved = true;
                    currentCard = 0;
                } else {
                    errorFeedback();
                    printf("\tError: target player not found.\n");
                }
            } else {
                //wrong card inserted
                errorFeedback();
                printf("\tError: not a valid player card.\n");
            }
        }
    }

    currentPlayer->loop(newCardInserted, waitCardRemoved);
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

/**
 * @brief Game::getPlayer get player with given id.
 * @param playerId id of player.
 * @return player or null.
 */
Player* Game::getPlayer(uint8_t playerId) {

    Player* current = currentPlayer;

    do {
        if (current->id == playerId) {
            return current;
        }
        current = current->nextPlayer;
    } while (current != currentPlayer);

    return 0;
}

//-----------------------------------------------------------------------------

void Game::acceptedFeedback() {
    //play a nice sound :)
    PiezoBuzzer::instance->playTone(NOTE_D6, 64, 1, 16);
    TaskScheduler::instance->loop();
}

//-----------------------------------------------------------------------------

void Game::errorFeedback() {
    //play sound and display error
    PiezoBuzzer::instance->playTone(PLAY_ERROR, 4);
    SevenSegmentDisplay::instance->setCharacter(CHAR_MINUS);
    TaskScheduler::instance->loop();
}

//-----------------------------------------------------------------------------

