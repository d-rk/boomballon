#ifndef SUDDENDEATHCARD_H
#define SUDDENDEATHCARD_H

#include <Cards/Card.h>

class SuddenDeathCard : public Card
{
public:
    SuddenDeathCard();

    virtual void play();

    virtual void attach(Player* currentPlayer);

    virtual const char* cardName() const {return "SUDDEN_DEATH";}

private:
    uint64_t startTime;
};

#endif // SUDDENDEATHCARD_H
