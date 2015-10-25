#include "SuddenDeathCard.h"

#include <Arduino.h>
#include <Devices/SevenSegmentDisplay.h>

//-----------------------------------------------------------------------------

const uint8_t SuddenDeathCard::type = 3;

//-----------------------------------------------------------------------------

SuddenDeathCard::SuddenDeathCard() : Card(type) {

}

//-----------------------------------------------------------------------------

void SuddenDeathCard::play(bool codeChanged) {
    discard = false;

    const int LOOP_MS = 2000;
    const int FILL_DURATION_MS = 500;

    uint64_t elapsed = (millis() - startTime) % LOOP_MS;

    if (elapsed < FILL_DURATION_MS) {
        SevenSegmentDisplay::instance->setAnimation(animation, 100, true, false);
        OutputDevice::instance->applyIntensities(255, 0, FILL_DURATION_MS, false);
        SevenSegmentDisplay::instance->stopAnimation();
    }
}

//-----------------------------------------------------------------------------

void SuddenDeathCard::attach(Player* currentPlayer) {

    Vector<Player*> opponents;

    Player* otherPlayer = currentPlayer->nextPlayer;

    while (otherPlayer != currentPlayer) {
        bool hasSuddenDeathCard = false;
        for (Vector<Card*>::iterator it = otherPlayer->cards.begin(); it != otherPlayer->cards.end(); it++) {
            if ((*it)->cardType == SuddenDeathCard::type) {
                hasSuddenDeathCard = true;
                break;
            }
        }

        if (!hasSuddenDeathCard) {
            opponents.push_back(otherPlayer);
        }
        otherPlayer = otherPlayer->nextPlayer;
    }

    if (opponents.empty()) {
        printf("Cannot attach SuddenDeathCard. No opponents left.");
        return;
    }

    otherPlayer = opponents[random(opponents.size())];

    switch(otherPlayer->id) {
        case 1: animation = ANIM_P1_FLOATING; break;
        case 2: animation = ANIM_P2_FLOATING; break;
        case 3: animation = ANIM_P3_FLOATING; break;
        case 4: animation = ANIM_P4_FLOATING; break;
        case 5: animation = ANIM_P5_FLOATING; break;
        default: animation = ANIM_ERROR;
    }

    startTime = millis();

    Card::attach(otherPlayer);
    printf("Attached SuddenDeath from Player %1d to %1d.\n", currentPlayer->id, otherPlayer->id);
}

//-----------------------------------------------------------------------------
