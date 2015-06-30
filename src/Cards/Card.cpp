#include "Card.h"

#include "../Constants.h"

#include <Cards/VolumeCard.h>
#include <Cards/SuddenDeathCard.h>

//-----------------------------------------------------------------------------

OutputDevice* Card::output = 0;

//-----------------------------------------------------------------------------

Card::Card() : canBeRemoved(false) {

}

//-----------------------------------------------------------------------------

void Card::playCard(uint8_t code, Player* currentPlayer) {

    Card* card = 0;

    switch (code) {
        case CODE_5:  card = new VolumeCard(50); break;
        case CODE_6:  card = new VolumeCard(127); break;
        case CODE_7:  card = new VolumeCard(-50); break;
        case CODE_9:  card = new VolumeCard(-127); break;
        case CODE_10: card = new SuddenDeathCard(); break;
    }

    if (card != 0) {
        card->attach(currentPlayer);
    }
}

//-----------------------------------------------------------------------------

