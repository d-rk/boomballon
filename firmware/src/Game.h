#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#include <Devices/CodeDetector.h>

class Card;
class Player;
class OutputDevice;
class PiezoBuzzer;
class SevenSegmentDisplay;

/**
 * @brief The Game class
 *
 * Class representing a game with a defined number of players.
 *
 */
class Game
{
public:
    Game();
    ~Game();

    bool isStarted();

    void start(int8_t numPlayers, bool playJingle = true);

    void loop();

private:
    void changePlayer();

    Player* getPlayer(uint8_t playerId);

    void acceptedFeedback();
    void errorFeedback();

private:
    bool started;
    bool gameEnded;
    bool playJingle;
    bool waitCardRemoved;
    int8_t numPlayers;
    Player* currentPlayer;
    Card* currentCard;
};

#endif // GAME_H
