#include "UpDownCard.h"
#include <Arduino.h>

//-----------------------------------------------------------------------------

UpDownCard::UpDownCard(int8_t volumeChangePercent, uint8_t intensity)
    : VolumeCard(volumeChangePercent, intensity)
{
    startUp = (random(2) == 0);
}

//-----------------------------------------------------------------------------

void UpDownCard::play(bool newCardInserted, bool waitCardRemoved) {

    if (newCardInserted) {
        if (!startUp) volumeChangePercent *= -1;

        int8_t volume100 = volumeChangePercent;
        int8_t volume80  = (int8_t)(volumeChangePercent * 0.8f);

        // alternate between fill and deflate once but only 80% volume
        volumeChangePercent = volume80;
        for (uint8_t i = 0; i < 2; i++) {
            VolumeCard::play(newCardInserted, waitCardRemoved);
            volumeChangePercent *= -1;
        }

        // fill or deflate with 100%
        volumeChangePercent = volume100;
        VolumeCard::play(newCardInserted, waitCardRemoved);
    }
}

//-----------------------------------------------------------------------------

