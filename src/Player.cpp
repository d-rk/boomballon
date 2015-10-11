#include "Player.h"

#include <Cards/Card.h>

#include <Arduino.h>

//-----------------------------------------------------------------------------

/**
 * @brief Player::Player Constructor.
 * @param numPlayers number of players in the game.
 * @param nextPlayer player which comes right after this one.
 * @note this constructor recursivly creates all players and links them together.
 */
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

/**
 * @brief Player::~Player Destructor.
 * Delete all players and the attached cards.
 */
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

/**
 * @brief Player::loop function to let player participate in game loop.
 */
void Player::loop() {

    bool removalNeeded = false;

    for (Vector<Card*>::iterator it = cards.begin(); it != cards.end(); it++) {
        if ( !(*it)->discard ) {
            (*it)->play();
        }
        removalNeeded |= (*it)->discard;
    }

    if (removalNeeded) {
        //at least 1 card has to be removed from player.
        Vector<Card*> cardsOld = cards;
        cards.clear();

        for (Vector<Card*>::iterator it = cardsOld.begin(); it != cardsOld.end(); it++) {
            if ( !(*it)->discard ) {
                cards.push_back(*it);
            } else {
                delete (*it);
            }
        }
    }
}

//-----------------------------------------------------------------------------

/**
 * @brief Player::changeDirection Recursively change neighborhood so that
 * next becomes previous and vice versa.
 */
void Player::changeDirection() {

    bool nextPlayerChanged = (nextPlayer->prevPlayer != this);

    //switch next and previous
    Player* tmp = nextPlayer;
    nextPlayer = prevPlayer;
    prevPlayer = tmp;
    printf("Player %d - prev: %d next: %d\n", id, prevPlayer->id, nextPlayer->id);

    if (!nextPlayerChanged) {
        //do it recursive for all players.
        prevPlayer->changeDirection();
    }
}
