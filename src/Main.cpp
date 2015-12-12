#include <Arduino.h>

#include <Game.h>
#include <PlayerChooser.h>
#include <Devices/OutputDevice.h>
#include <Devices/PiezoBuzzer.h>
#include <Devices/SevenSegmentDisplay.h>
#include <Tasks/TaskScheduler.h>
#include <Helper/Log.h>

#include <Constants.h>

#include <Dummy/CodeDetectorSerial.h>

//-----------------------------------------------------------------------------

Game game;

//-----------------------------------------------------------------------------

/**
 * @brief setup setup everything needed for the program to run.
 */
void setup() {

    delay(1000);

    Log::setup();
    randomSeed(analogRead(PIN_A0)); //PIN 0 needs to be unconnected

    PlayerChooser::instance = new PlayerChooser(CODE_15, CODE_17, CODE_19, CODE_21, CODE_23);

    TaskScheduler::instance = new TaskScheduler();
    OutputDevice::instance = new OutputDevice(PIN_3, PIN_4);

    CodeDetector::instance = new CodeDetector(PIN_A1, PIN_A2, PIN_A3, PIN_A4, PIN_A5);
    PiezoBuzzer::instance  = new PiezoBuzzer(PIN_6);
    SevenSegmentDisplay::instance = new SevenSegmentDisplay(PIN_8, PIN_9, PIN_7);

    OutputDevice::instance->setup();
    CodeDetector::instance->setup();
    PiezoBuzzer::instance->setup();

    SevenSegmentDisplay::instance->setup();

    SevenSegmentDisplay::instance->setCharacter(CHAR_DOT);

    printf("=================================\n");
    printf("=== Select number of players  ===\n");
    printf("=================================\n");

    //game.start(2, false);
}

//-----------------------------------------------------------------------------

/**
 * @brief loop main loop for the program.
 */
void loop() {

    if (!game.isStarted()) {
        if (PlayerChooser::instance->detectNumPlayers() && CodeDetector::instance->getActiveCode() == CODE_ALL) {
            game.start(PlayerChooser::instance->getNumPlayers());
        }
    } else {
        //game running
        game.loop();
    }
}

//-----------------------------------------------------------------------------

