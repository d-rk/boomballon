#include "PushToLimitCard.h"

//-----------------------------------------------------------------------------

PushToLimitCard::PushToLimitCard(uint8_t intensity)
    : VolumeCard(0, intensity)
{
}

//-----------------------------------------------------------------------------

void PushToLimitCard::play() {
    // push the volume to 99%
    volumeChangePercent = (int8_t)(100.0f - Card::output->volume - 1.0f);
    VolumeCard::play();
}

//-----------------------------------------------------------------------------
