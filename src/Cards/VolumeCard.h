#ifndef VOLUMECARD_H
#define VOLUMECARD_H

#include <Cards/Card.h>

class VolumeCard : public Card
{
public:
    VolumeCard(int8_t volumeChange);

    virtual void play();

    virtual void attach(Player* currentPlayer);

private:
    int8_t volumeChange;
};

#endif // VOLUMECARD_H
