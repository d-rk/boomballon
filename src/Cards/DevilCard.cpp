#include "DevilCard.h"
#include <Arduino.h>

//-----------------------------------------------------------------------------

DevilCard::DevilCard(int8_t volumeChangePercent, uint8_t intensity)
    : VolumeCard(volumeChangePercent, intensity)
{
}

//-----------------------------------------------------------------------------

void DevilCard::attach(Player* targetPlayer) {
    Card::attach(targetPlayer);
    printf("\tAttached %s to %1d.\n", cardName(), targetPlayer->id);
}

//-----------------------------------------------------------------------------

