#include "SuddenDeathCard.h"

#include <Arduino.h>

//-----------------------------------------------------------------------------

SuddenDeathCard::SuddenDeathCard() {

}

//-----------------------------------------------------------------------------

void SuddenDeathCard::play() {
    canBeRemoved = false;

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
    otherPlayer->cards.push_back(this);

    Serial.print("Attached SuddenDeath from Player ");
    Serial.print(currentPlayer->id);
    Serial.print(" to Player ");
    Serial.println(otherPlayer->id);
}

//-----------------------------------------------------------------------------
