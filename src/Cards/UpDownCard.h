#ifndef UPDOWNCARD_H
#define UPDOWNCARD_H

#include <Cards/VolumeCard.h>

class UpDownCard : public VolumeCard
{
public:
    UpDownCard(int8_t volumeChangePercent, uint8_t intensity, bool startUp);

    virtual void play(bool codeChanged);

    virtual const char* cardName() const {return "UP_DOWN";}

private:
    bool startUp;
};

#endif // UPDOWNCARD_H
