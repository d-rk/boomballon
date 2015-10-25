#include "ChangeDirectionCard.h"
#include <Arduino.h>

//-----------------------------------------------------------------------------

/**
 * @brief ChangeDirectionCard::ChangeDirectionCard Constructor
 * @param volumeChange volume change to apply before direction change.
 */
ChangeDirectionCard::ChangeDirectionCard(int8_t volumeChangePercent, uint8_t intensity)
    : VolumeCard(volumeChangePercent, intensity)
{

}

//-----------------------------------------------------------------------------

/**
 * @brief ChangeDirectionCard::play Card is played, apply volume and change direction.
 */
void ChangeDirectionCard::play(bool codeChanged) {
    if (codeChanged) {
        VolumeCard::play(codeChanged);
        attachedPlayer->changeDirection();
        printf("Changed direction.");
    }
}

//-----------------------------------------------------------------------------

