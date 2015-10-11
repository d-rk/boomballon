#ifndef PLAYERCHOOSER_H
#define PLAYERCHOOSER_H

#include <Led.h>
#include <CodeDetector.h>

/**
 * @brief The PlayerChooser class.
 *
 * Class to detect and visualize the number of players that will
 * participate in the game.
 * The PlayerChooser is used at the start of each game.
 *
 */
class PlayerChooser
{
public:
    PlayerChooser(uint8_t code2Players, uint8_t code3Players, uint8_t code4Players, uint8_t code5Players);

    void setup(CodeDetector* codeDetector, Led* redLed, Led* greenLed);

    bool detectNumPlayers();

    int8_t getNumPlayers();

    void setNumPlayers(int8_t numPlayers);

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
