#include "SuddenDeathCard.h"

#include <Arduino.h>

//-----------------------------------------------------------------------------

SuddenDeathCard::SuddenDeathCard() {

}

//-----------------------------------------------------------------------------

void SuddenDeathCard::play() {
    discard = false;

    const int LOOP_MS = 2000;
    const int FILL_DURATION_MS = 500;

    uint64_t elapsed = (millis() - startTime) % LOOP_MS;

    if (elapsed < FILL_DURATION_MS) {
        Card::output->apply(127);
    } else {
        Card::output->apply(0);
    }
}

//-----------------------------------------------------------------------------

void SuddenDeathCard::attach(Player* currentPlayer) {

    Player* otherPlayer = currentPlayer;

    while (otherPlayer == currentPlayer) {
        for (int i=0; i < random(5); i++) {
            otherPlayer = otherPlayer->nextPlayer;
        }
    }

    startTime = millis();

    Card::attach(otherPlayer);
    printf("Attached SuddenDeath from Player %1d to %1d.\n", currentPlayer->id, otherPlayer->id);
}

//-----------------------------------------------------------------------------
