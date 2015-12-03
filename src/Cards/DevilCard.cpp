#include "DevilCard.h"
#include <Arduino.h>

//-----------------------------------------------------------------------------

DevilCard::DevilCard(int8_t volumeChangePercent, uint8_t intensity)
    : VolumeCard(volumeChangePercent, intensity)
{
}

//-----------------------------------------------------------------------------

void DevilCard::attach(Player* currentPlayer) {

    Player* otherPlayer = currentPlayer;

    while (otherPlayer == currentPlayer) {
        for (int i=0; i < random(5); i++) {
            otherPlayer = otherPlayer->nextPlayer;
        }
    }

    Card::attach(otherPlayer);
    printf("\tAttached %s from Player %1d to %1d.\n", cardName(), currentPlayer->id, otherPlayer->id);
}

//-----------------------------------------------------------------------------

