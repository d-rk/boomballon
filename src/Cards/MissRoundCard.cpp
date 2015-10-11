#include "MissRoundCard.h"
#include <Player.h>
#include <Arduino.h>

//-----------------------------------------------------------------------------

/**
 * @brief MissRoundCard::MissRoundCard Constructor.
 */
MissRoundCard::MissRoundCard()
{
}

//-----------------------------------------------------------------------------

/**
 * @brief MissRoundCard::play The card is played from opponents stack of cards.
 * Invalidate the next card following this one.
 */
void MissRoundCard::play() {

    if (attachedPlayer != NULL) {
        Card* lastCard = NULL;
        for (Vector<Card*>::iterator it = attachedPlayer->cards.begin(); it != attachedPlayer->cards.end(); it++) {
            if (lastCard == this) {
                //ok we found the card which should be played after this one
                printf("%s discarded card %s.\n", cardName(), (*it)->cardName());
                (*it)->discard = true;
                discard = true;
            } else {
                lastCard = *it;
            }
        }
    }
    discard = false;
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

