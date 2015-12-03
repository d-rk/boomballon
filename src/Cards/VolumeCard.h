#ifndef VOLUMECARD_H
#define VOLUMECARD_H

#include <Cards/Card.h>

class VolumeCard : public Card
{
public:
    VolumeCard(int8_t volumeChangePercent, uint8_t intensity);

    virtual void play(bool newCardInserted, bool waitCardRemoved);

    virtual void attach(Player* currentPlayer);

    virtual const char* cardName() const {return "VOLUME";}

protected:
    int8_t volumeChangePercent;
    uint8_t intensity;
public:
    static const uint8_t type;
};

#endif // VOLUMECARD_H
