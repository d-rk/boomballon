#include "AngelCard.h"
#include <Arduino.h>

//-----------------------------------------------------------------------------

AngelCard::AngelCard(int8_t volumeChangePercent, uint8_t intensity)
    : VolumeCard(volumeChangePercent, intensity)
{

}

//-----------------------------------------------------------------------------

void AngelCard::play(bool newCardInserted, bool waitCardRemoved) {

    if (newCardInserted && attachedPlayer != NULL) {
        Card* lastCard = NULL;
        for (Vector<Card*>::iterator it = attachedPlayer->cards.begin(); it != attachedPlayer->cards.end(); it++) {
            if (lastCard == this) {
                //ok we have a card which comes after this card
                //we can assume that it is the next round now and apply this card
                VolumeCard::play(newCardInserted, waitCardRemoved);
            } else {
                lastCard = *it;
            }
        }
    }
}

//-----------------------------------------------------------------------------

