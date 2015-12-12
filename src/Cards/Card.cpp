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

Card* Card::playCard(uint8_t code) {

    Card* card = 0;

    switch (code) {
        case CODE_1:  card = new VolumeCard(-20, 200); break;          // Hochdruck -10
        case CODE_2:  card = new VolumeCard(-30, 200); break;          // Hochdruck -20
        case CODE_3:  card = new ChangeDirectionCard(-30, 200); break; // Kurswechsel -30
        case CODE_4:  card = new AngelCard(-30, 50); break;            // Engelsbote -30
        case CODE_5:  card = new MissRoundCard(); break;               // Blockierung

        case CODE_6:  card = new VolumeCard( 30, 200); break;          // Tiefdruck +20
        case CODE_7:  card = new VolumeCard( 40, 200); break;          // Tiefdruck +30
        case CODE_9:  card = new ChangeDirectionCard(40, 200); break;  // Kurswechsel +40
        case CODE_10: card = new DevilCard( 40, 50); break;            // Teufelsbote +40
        case CODE_11: card = new SuddenDeathCard(); break;             // Weltuntergang

        case CODE_13: card = new FiftyFiftyCard(100); break;           // Zwiegespalten +/- 50
        case CODE_14: card = new UpDownCard(35, 200); break;           // Berg und Tal 20

        case CODE_27: card = new PushToLimitCard(255); break;          // Yin Yan
        default: break;
    }

    return card;
}

//-----------------------------------------------------------------------------

void Card::attach(Player* targetPlayer) {
    attachedPlayer = targetPlayer;
    attachedPlayer->cards.push_back(this);
}

//-----------------------------------------------------------------------------

