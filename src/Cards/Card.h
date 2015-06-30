#ifndef CARD
#define CARD

#include <stdint.h>

#include <Player.h>
#include <OutputDevice.h>

class Card {

public:
    Card();

    virtual void play() = 0;

    virtual void attach(Player* currentPlayer) = 0;

    static void playCard(uint8_t code, Player* currentPlayer);

    static OutputDevice* output;

public:
    bool canBeRemoved;
};

#endif // CARD

