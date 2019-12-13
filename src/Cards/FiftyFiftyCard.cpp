#include "FiftyFiftyCard.h"
#include <Arduino.h>

//-----------------------------------------------------------------------------

FiftyFiftyCard::FiftyFiftyCard(uint8_t intensity)
    : VolumeCard(0, intensity)
{
}

//-----------------------------------------------------------------------------

void FiftyFiftyCard::play(bool newCardInserted, bool waitCardRemoved) {

    if (newCardInserted) {
        //sign is 1 or -1 randomly
        int8_t sign = random(2)*2 - 1;

        if (sign > 0) {
            // inflate 50% of the remaining volume
            volumeChangePercent = (int8_t)((100.0f - OutputDevice::instance->volume) * 0.5f);
        } else {
            // deflate 50% of the current volume
            volumeChangePercent = (int8_t)(OutputDevice::instance->volume * 0.5f * sign);
        }
        VolumeCard::play(newCardInserted, waitCardRemoved);
    }
}

//-----------------------------------------------------------------------------

