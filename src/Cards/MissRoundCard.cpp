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
void MissRoundCard::play(bool newCardInserted, bool waitCardRemoved) {

    if (newCardInserted && attachedPlayer != NULL) {
        Card* lastCard = NULL;

        if (random(2) == 0) {
            //just return in 50% of the time.
            return;
        }

        for (Vector<Card*>::iterator it = attachedPlayer->cards.begin(); it != attachedPlayer->cards.end(); it++) {
            if (lastCard == this) {
                //ok we found the card which should be played after this one
                #ifdef LOGGING_ENABLED
                printf("\t%s discarded card %s.\n", cardName(), (*it)->cardName());
                #endif
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
}

//-----------------------------------------------------------------------------

/**
 * @brief MissRoundCard::attach Attach the card to player.
 * @param targetPlayer player to attach the card to.
 */
void MissRoundCard::attach(Player* targetPlayer) {
    Card::attach(targetPlayer);
    #ifdef LOGGING_ENABLED
    printf("\tAttached %s to %1d.\n", cardName(), targetPlayer->id);
    #endif
}

//-----------------------------------------------------------------------------

