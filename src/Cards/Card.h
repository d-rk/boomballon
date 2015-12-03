#ifndef CARD
#define CARD

#include <stdint.h>

#include <Player.h>
#include <Devices/OutputDevice.h>

/**
 * @brief The Card class
 *
 * Base class for all cards in the game.
 *
 */
class Card {

public:
    Card(uint8_t cardType);

    //todo: remove codeChanged
    virtual void play(bool newCardInserted, bool waitCardRemoved) = 0;

    virtual void attach(Player* currentPlayer);

    static void playCard(uint8_t code, Player* currentPlayer);

    virtual const char* cardName() const = 0;

public:
    bool discard;
    uint8_t cardType;
protected:
    Player* attachedPlayer;
};

#endif // CARD

