#include "Player.h"

#include <Cards/Card.h>

#include <Arduino.h>

//-----------------------------------------------------------------------------

Player::Player(int8_t numPlayers, Player* nextPlayer)
{
    //create 'numPlayers' player objects and link them together
    id = (uint8_t)numPlayers;
    this->nextPlayer = nextPlayer;

    if (--numPlayers > 0) {
        this->prevPlayer = new Player(numPlayers, this);
    } else {
        //find last player;
        Player* lastPlayer = this;

        while(lastPlayer->nextPlayer != 0) {
            lastPlayer = lastPlayer->nextPlayer;
        }

        //last player is previous
        prevPlayer = lastPlayer;
        lastPlayer->nextPlayer = this;
    }
}

//-----------------------------------------------------------------------------

Player::~Player() {

    //delete attached cards
    for (Vector<Card*>::iterator it = cards.begin(); it != cards.end(); it++) {
        delete (*it);
    }
    cards.clear();

    //delete all player objects
    if (prevPlayer != 0 && prevPlayer != this) {
        prevPlayer->nextPlayer = 0;
    }

    if (nextPlayer != 0 && nextPlayer != this) {
        nextPlayer->prevPlayer = 0;
        delete nextPlayer;
    }
}

//-----------------------------------------------------------------------------

void Player::loop() {

    bool removalNeeded = false;

    for (Vector<Card*>::iterator it = cards.begin(); it != cards.end(); it++) {
        (*it)->play();
        removalNeeded |= (*it)->canBeRemoved;
    }

    if (removalNeeded) {
        Vector<Card*> cardsOld = cards;
        cards.clear();

        for (Vector<Card*>::iterator it = cardsOld.begin(); it != cardsOld.end(); it++) {
            if ( !(*it)->canBeRemoved ) {
                cards.push_back(*it);
            } else {
                delete (*it);
            }
        }
    }
}

//-----------------------------------------------------------------------------
