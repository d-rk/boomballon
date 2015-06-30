#include "VolumeCard.h"

#include <Player.h>

#include <Arduino.h>

//-----------------------------------------------------------------------------

VolumeCard::VolumeCard(int8_t volumeChange)
    : volumeChange(volumeChange)
{

}

//-----------------------------------------------------------------------------

void VolumeCard::play() {
    canBeRemoved = true;
    Serial.println("play volumecard - start");

    int sign = volumeChange >= 0 ? 1 : -1;

    for (int i=0; i < sign * volumeChange; i++) {
        Card::output->apply(sign * i);
        delay(50);
    }

    Card::output->apply(0);

    Serial.println("play volumecard - end");
}

//-----------------------------------------------------------------------------

void VolumeCard::attach(Player* currentPlayer) {
    currentPlayer->cards.push_back(this);
    Serial.print("Attached VolumeCard (");
    Serial.print(volumeChange);
    Serial.print(") to Player ");
    Serial.println(currentPlayer->id);
}

//-----------------------------------------------------------------------------
