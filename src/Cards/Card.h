#ifndef CARD
#define CARD

#include <stdint.h>

#include <Player.h>
#include <OutputDevice.h>

/**
 * @brief The Card class
 *
 * Base class for all cards in the game.
 *
 */
class Card {

public:
    Card();

    virtual void play() = 0;

    virtual void attach(Player* currentPlayer);

    static void playCard(uint8_t code, Player* currentPlayer);

    static OutputDevice* output;

    virtual const char* cardName() const = 0;

public:
    bool discard;
protected:
    Player* attachedPlayer;
};

#endif // CARD

