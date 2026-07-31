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

    virtual void play(bool newCardInserted, bool waitCardRemoved) = 0;

    virtual void attach(Player* targetPlayer);

    virtual bool needsPlayerSelection() {return false;}

    static Card* playCard(uint8_t code);

    virtual const char* cardName() const = 0;

public:
    bool discard;
    uint8_t cardType;
protected:
    Player* attachedPlayer;
};

#endif // CARD

