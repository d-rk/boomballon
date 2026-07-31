#ifndef CHANGEDIRECTIONCARD_H
#define CHANGEDIRECTIONCARD_H

#include <Cards/VolumeCard.h>

/**
 * @brief The ChangeDirectionCard class
 *
 * Add a specified volume to the ballon and switch between
 * clockwise and counter-clockwise player rotation.
 *
 */
class ChangeDirectionCard : public VolumeCard
{
public:
    ChangeDirectionCard(int8_t volumeChangePercent, uint8_t intensity);

    virtual void play(bool newCardInserted, bool waitCardRemoved);

    virtual const char* cardName() const {return "DIRECTION";}
};

#endif // CHANGEDIRECTIONCARD_H
