#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#include <Led.h>
#include <CodeDetector.h>

class Player;
class OutputDevice;

class Game
{
public:
    Game();
    ~Game();

    void setup(CodeDetector* codeDetector, OutputDevice* outputDevice, Led* redLed, Led* greenLed);

    bool isStarted();

    void start(int8_t numPlayers);

    void loop();

private:
    const int MAX_VOLUME;

    bool started;
    bool gameEnded;
    int8_t numPlayers;
    Player* currentPlayer;
    CodeDetector* codeDetector;
    Led* redLed;
    Led* greenLed;
};

#endif // GAME_H
