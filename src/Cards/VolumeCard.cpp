#include "VolumeCard.h"

#include <Player.h>

#include <Arduino.h>

//-----------------------------------------------------------------------------

VolumeCard::VolumeCard(int8_t volumeChangePercent, uint8_t intensity)
    : volumeChangePercent(volumeChangePercent),
      intensity(intensity)
{

}

//-----------------------------------------------------------------------------

void VolumeCard::play() {

    Card::output->apply(volumeChangePercent, intensity);

    printf("%s: applied volume change of %d%% with intensity %d\n", cardName(), volumeChangePercent, intensity);
    discard = true;
}

//-----------------------------------------------------------------------------

void VolumeCard::attach(Player* currentPlayer) {
    Card::attach(currentPlayer);
    printf("Attached %s(%d) to player %d.\n", cardName(), volumeChangePercent, currentPlayer->id);
}

//-----------------------------------------------------------------------------
