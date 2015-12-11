#ifndef DEVILCARD_H
#define DEVILCARD_H

#include <Cards/VolumeCard.h>

class DevilCard : public VolumeCard
{
public:
    DevilCard(int8_t volumeChangePercent, uint8_t intensity);

    virtual const char* cardName() const {return "DEVIL";}

    virtual bool needsPlayerSelection() {return true;}

    virtual void attach(Player* targetPlayer);
};

#endif // DEVILCARD_H
