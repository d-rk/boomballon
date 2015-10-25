#include "VolumeCard.h"

#include <Player.h>
#include <Devices/SevenSegmentDisplay.h>

#include <Arduino.h>

//-----------------------------------------------------------------------------

const uint8_t VolumeCard::type = 1;

//-----------------------------------------------------------------------------

VolumeCard::VolumeCard(int8_t volumeChangePercent, uint8_t intensity)
    : Card(type),
      volumeChangePercent(volumeChangePercent),
      intensity(intensity)
{

}

//-----------------------------------------------------------------------------

void VolumeCard::play(bool codeChanged) {

    if (codeChanged) {

        SevenSegmentDisplay::instance->setAnimation(ANIM_FILL, 100, true, volumeChangePercent < 0, 0);
        OutputDevice::instance->apply(volumeChangePercent, intensity);
        SevenSegmentDisplay::instance->stopAnimation();

        printf("%s: applied volume change of %d%% with intensity %d\n", cardName(), volumeChangePercent, intensity);
        discard = true;
    }
}

//-----------------------------------------------------------------------------

void VolumeCard::attach(Player* currentPlayer) {
    Card::attach(currentPlayer);
    printf("Attached %s(%d) to player %d.\n", cardName(), volumeChangePercent, currentPlayer->id);
}

//-----------------------------------------------------------------------------
