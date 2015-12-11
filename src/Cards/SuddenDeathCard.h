#ifndef SUDDENDEATHCARD_H
#define SUDDENDEATHCARD_H

#include <Cards/Card.h>
#include <Devices/SevenSegmentDisplay.h>

class SuddenDeathCard : public Card
{
public:
    SuddenDeathCard();

    virtual void play(bool newCardInserted, bool waitCardRemoved);

    virtual void attach(Player* targetPlayer);

    virtual bool needsPlayerSelection() {return true;}

    virtual const char* cardName() const {return "SUDDEN_DEATH";}

private:
    uint64_t startTime;
    DisplayAnimation animation;

public:
    static const uint8_t type;
};

#endif // SUDDENDEATHCARD_H
