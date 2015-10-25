#ifndef PLAYERCHOOSER_H
#define PLAYERCHOOSER_H

#include <stdint.h>
#include <Devices/SevenSegmentDisplay.h>

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

    bool detectNumPlayers();

    int8_t getNumPlayers();

    void setNumPlayers(int8_t numPlayers);

private:
    DisplayAnimation getAnimation(uint8_t numPlayers);
private:
    const uint8_t CODES[4];
    int8_t numPlayers;
};

#endif // PLAYERCHOOSER_H
