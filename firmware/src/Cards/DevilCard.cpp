#include "DevilCard.h"
#include <Arduino.h>
#include <Constants.h>

//-----------------------------------------------------------------------------

DevilCard::DevilCard(int8_t volumeChangePercent, uint8_t intensity)
    : VolumeCard(volumeChangePercent, intensity)
{
}

//-----------------------------------------------------------------------------

void DevilCard::play(bool newCardInserted, bool waitCardRemoved) {

    if (random(2) == 0) {
        //only do it perhaps
        VolumeCard::play(newCardInserted, waitCardRemoved);
    }
}

//-----------------------------------------------------------------------------

void DevilCard::attach(Player* targetPlayer) {
    Card::attach(targetPlayer);
    printf("\tAttached %s to %1d.\n", cardName(), targetPlayer->id);
}

//-----------------------------------------------------------------------------

