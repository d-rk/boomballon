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

    virtual void play(bool newCardInserted, bool waitCardRemoved);

    virtual void attach(Player* currentPlayer);

    virtual const char* cardName() const {return "MISS_ROUND";}

public:
    static const uint8_t type;
};

#endif // MISSROUNDCARD_H
