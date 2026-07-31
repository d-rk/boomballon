#ifndef FIFTYFIFTYCARD_H
#define FIFTYFIFTYCARD_H

#include <Cards/VolumeCard.h>

class FiftyFiftyCard : public VolumeCard
{
public:
    FiftyFiftyCard(uint8_t intensity);

    virtual void play(bool newCardInserted, bool waitCardRemoved);

    virtual const char* cardName() const {return "FIFTY_FIFTY";}
};

#endif // FIFTYFIFTYCARD_H
