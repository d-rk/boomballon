#include "Card.h"

#include "../Constants.h"

#include <Cards/AngelCard.h>
#include <Cards/DevilCard.h>
#include <Cards/ChangeDirectionCard.h>
#include <Cards/FiftyFiftyCard.h>
#include <Cards/MissRoundCard.h>
#include <Cards/PushToLimitCard.h>
#include <Cards/SuddenDeathCard.h>
#include <Cards/UpDownCard.h>
#include <Cards/VolumeCard.h>

//-----------------------------------------------------------------------------

Card::Card(uint8_t cardType)
    : discard(false),
      cardType(cardType),
      attachedPlayer(0)
{
}

//-----------------------------------------------------------------------------

void Card::playCard(uint8_t code, Player* currentPlayer) {

    Card* card = 0;

    switch (code) {
        case CODE_5:  card = new MissRoundCard(); break;
        case CODE_6:  card = new ChangeDirectionCard( 30, 200); break;
        case CODE_7:  card = new ChangeDirectionCard(-30, 200); break;
        case CODE_9:  card = new FiftyFiftyCard(100); break;
        case CODE_10: card = new VolumeCard( 20, 200); break;
        case CODE_11: card = new VolumeCard( 40, 200); break;
        case CODE_13: card = new VolumeCard(-20, 200); break;
        case CODE_14: card = new VolumeCard(-30, 200); break;
        case CODE_15: card = new PushToLimitCard(255); break;
        case CODE_17: card = new UpDownCard( 35, 200, true); break;
        case CODE_19: card = new UpDownCard(-35, 200, true); break;
        case CODE_21: card = new AngelCard(-30, 50); break;
        case CODE_23: card = new DevilCard( 30, 50); break;
        case CODE_27: card = new SuddenDeathCard(); break;

        default: break;
    }

    if (card != 0) {
        card->attach(currentPlayer);
    }
}

//-----------------------------------------------------------------------------

void Card::attach(Player* currentPlayer) {
    attachedPlayer = currentPlayer;
    attachedPlayer->cards.push_back(this);
}

//-----------------------------------------------------------------------------

