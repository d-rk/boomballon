#include "MissRoundCard.h"
#include <Player.h>
#include <Arduino.h>
#include <Devices/PiezoBuzzer.h>
#include <Devices/SevenSegmentDisplay.h>
#include <Tasks/TaskScheduler.h>

#include <Constants.h>

//-----------------------------------------------------------------------------

const uint8_t MissRoundCard::type = 2;

//-----------------------------------------------------------------------------

/**
 * @brief MissRoundCard::MissRoundCard Constructor.
 */
MissRoundCard::MissRoundCard() : Card(type)
{
}

//-----------------------------------------------------------------------------

/**
 * @brief MissRoundCard::play The card is played from opponents stack of cards.
 * Invalidate the next card following this one.
 */
void MissRoundCard::play(bool codeChanged) {

    if (codeChanged && attachedPlayer != NULL) {
        Card* lastCard = NULL;
        for (Vector<Card*>::iterator it = attachedPlayer->cards.begin(); it != attachedPlayer->cards.end(); it++) {
            if (lastCard == this) {
                //ok we found the card which should be played after this one
                printf("%s discarded card %s.\n", cardName(), (*it)->cardName());
                (*it)->discard = true;
                discard = true;
                //show it to the user
                delay(500);
                PiezoBuzzer::instance->playTone(PLAY_ERROR, 4);
                SevenSegmentDisplay::instance->setCharacter(CHAR_MINUS);
                TaskScheduler::instance->loop();
                return;
            } else {
                lastCard = *it;
            }
        }
    }
    discard = false;
    byte bb = B10010;
}

//-----------------------------------------------------------------------------

/**
 * @brief MissRoundCard::attach Attach the card to a random opponent of current player.
 * @param currentPlayer player who played the card.
 */
void MissRoundCard::attach(Player* currentPlayer) {

    Player* otherPlayer = currentPlayer;

    while (otherPlayer == currentPlayer) {
        for (int i=0; i < random(5); i++) {
            otherPlayer = otherPlayer->nextPlayer;
        }
    }

    Card::attach(otherPlayer);
    printf("Attached %s from Player %1d to %1d.\n", cardName(), currentPlayer->id, otherPlayer->id);
}

//-----------------------------------------------------------------------------

