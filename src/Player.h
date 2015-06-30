#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include <Helper/Vector.h>

class Card;

class Player
{
public:
    Player(int8_t numPlayers, Player* nextPlayer = 0);
    ~Player();

    void loop();

public:
    Player* prevPlayer;
    Player* nextPlayer;
    uint8_t id;
    Vector<Card*> cards;
};

#endif // PLAYER_H
