#ifndef PUSHTOLIMITCARD_H
#define PUSHTOLIMITCARD_H

#include <Cards/VolumeCard.h>

class PushToLimitCard : public VolumeCard
{
public:
    PushToLimitCard(uint8_t intensity);

    virtual void play(bool codeChanged);

    virtual const char* cardName() const {return "PUSH_LIMIT";}
};

#endif // PUSHTOLIMITCARD_H
