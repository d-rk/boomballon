#include "UpDownCard.h"

//-----------------------------------------------------------------------------

UpDownCard::UpDownCard(int8_t volumeChangePercent, uint8_t intensity, bool startUp)
    : VolumeCard(volumeChangePercent, intensity),
      startUp(startUp)
{
}

//-----------------------------------------------------------------------------

void UpDownCard::play(bool newCardInserted, bool waitCardRemoved) {

    if (newCardInserted) {
        if (!startUp) volumeChangePercent *= -1;

        // alternate between fill and deflate
        for (uint8_t i = 0; i < 4; i++) {
            VolumeCard::play(newCardInserted, waitCardRemoved);
            volumeChangePercent *= -1;
        }
    }
}

//-----------------------------------------------------------------------------

