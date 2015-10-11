#include "FiftyFiftyCard.h"
#include <Arduino.h>

//-----------------------------------------------------------------------------

FiftyFiftyCard::FiftyFiftyCard(uint8_t intensity)
    : VolumeCard(0, intensity)
{
}

//-----------------------------------------------------------------------------

void FiftyFiftyCard::play() {

    //sign is 1 or -1 randomly
    int8_t sign = random(2)*2 - 1;

    // 50% of the current volume plus or minus randomly
    volumeChangePercent = (int8_t)(Card::output->volume * 0.5f * sign);
    VolumeCard::play();
}

//-----------------------------------------------------------------------------

