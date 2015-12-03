#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include <Helper/Vector.h>

class Card;

/**
 * @brief The Player class
 *
 * Defines a player in the game.
 */
class Player
{
public:
    Player(int8_t numPlayers, Player* nextPlayer = 0);
    ~Player();

    void loop(bool codeChanged, bool waitCardRemoved);

    void changeDirection();

public:
    Player* prevPlayer;
    Player* nextPlayer;
    uint8_t id;
    Vector<Card*> cards;
};

#endif // PLAYER_H
