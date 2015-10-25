#ifndef SUDDENDEATHCARD_H
#define SUDDENDEATHCARD_H

#include <Cards/Card.h>
#include <Devices/SevenSegmentDisplay.h>

class SuddenDeathCard : public Card
{
public:
    SuddenDeathCard();

    virtual void play(bool codeChanged);

    virtual void attach(Player* currentPlayer);

    virtual const char* cardName() const {return "SUDDEN_DEATH";}

private:
    uint64_t startTime;
    DisplayAnimation animation;

public:
    static const uint8_t type;
};

#endif // SUDDENDEATHCARD_H
