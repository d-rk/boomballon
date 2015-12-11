#include "SuddenDeathCard.h"

#include <Arduino.h>
#include <Devices/SevenSegmentDisplay.h>

//-----------------------------------------------------------------------------

const uint8_t SuddenDeathCard::type = 3;

//-----------------------------------------------------------------------------

SuddenDeathCard::SuddenDeathCard() : Card(type) {

}

//-----------------------------------------------------------------------------

void SuddenDeathCard::play(bool newCardInserted, bool waitCardRemoved) {
    discard = false;

    if (!waitCardRemoved) {

        const int LOOP_MS = 2000;
        const int FILL_DURATION_MS = 500;

        uint64_t elapsed = (millis() - startTime) % LOOP_MS;

        if (elapsed < FILL_DURATION_MS) {
            SevenSegmentDisplay::instance->setAnimation(animation, 100, true, false);
            OutputDevice::instance->applyIntensities(255, 0, FILL_DURATION_MS, false);
            SevenSegmentDisplay::instance->stopAnimation();
        }
    }
}

//-----------------------------------------------------------------------------

void SuddenDeathCard::attach(Player* targetPlayer) {

    bool hasSuddenDeathCard = false;

    for (Vector<Card*>::iterator it = targetPlayer->cards.begin(); it != targetPlayer->cards.end(); it++) {
        if ((*it)->cardType == SuddenDeathCard::type) {
            hasSuddenDeathCard = true;
            break;
        }
    }

    if (hasSuddenDeathCard) {
        printf("\tCannot attach SuddenDeathCard. Player already has card.");
        return;
    }

    switch(targetPlayer->id) {
        case 1: animation = ANIM_P1_FLOATING; break;
        case 2: animation = ANIM_P2_FLOATING; break;
        case 3: animation = ANIM_P3_FLOATING; break;
        case 4: animation = ANIM_P4_FLOATING; break;
        case 5: animation = ANIM_P5_FLOATING; break;
        default: animation = ANIM_ERROR;
    }

    startTime = millis();

    Card::attach(targetPlayer);
    printf("\tAttached SuddenDeath to %1d.\n", targetPlayer->id);
}

//-----------------------------------------------------------------------------
