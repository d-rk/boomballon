#ifndef MISSROUNDCARD_H
#define MISSROUNDCARD_H

#include <Cards/Card.h>

class Player;

/**
 * @brief The MissRoundCard class
 *
 * A random opponent of current player will miss the next card he plays.
 *
 */
class MissRoundCard : public Card
{
public:
    MissRoundCard();

    virtual void play();

    virtual void attach(Player* currentPlayer);

    virtual const char* cardName() const {return "MISS_ROUND";}
};

#endif // MISSROUNDCARD_H
