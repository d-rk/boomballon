#ifndef ANGELCARD_H
#define ANGELCARD_H

#include <Cards/VolumeCard.h>

class AngelCard : public VolumeCard
{
public:
    AngelCard(int8_t volumeChangePercent, uint8_t intensity);

    virtual void play(bool newCardInserted, bool waitCardRemoved);

    virtual const char* cardName() const {return "ANGEL";}
};

#endif // ANGELCARD_H
