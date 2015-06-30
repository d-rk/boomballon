#ifndef PLAYERCHOOSER_H
#define PLAYERCHOOSER_H

#include <Led.h>
#include <CodeDetector.h>

class PlayerChooser
{
public:
    PlayerChooser(uint8_t code2Players, uint8_t code3Players, uint8_t code4Players, uint8_t code5Players);

    void setup(CodeDetector* codeDetector, Led* redLed, Led* greenLed);

    bool detectNumPlayers();

    int8_t getNumPlayers();

private:
    const uint8_t CODES[4];
    int8_t numPlayers;
    CodeDetector* codeDetector;
    Led* redLed;
    Led* greenLed;

    uint64_t timeLastBlink;
    const unsigned long TIME_WAIT_MS;

};

#endif // PLAYERCHOOSER_H
