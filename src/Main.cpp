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

PlayerChooser playerChooser(CODE_2, CODE_3, CODE_4, CODE_5);
Game game;

//-----------------------------------------------------------------------------

/**
 * @brief setup setup everything needed for the program to run.
 */
void setup() {

    delay(1000);

    Log::setup();
    randomSeed(analogRead(PIN_A0)); //PIN 0 needs to be unconnected

    TaskScheduler::instance = new TaskScheduler();
    OutputDevice::instance = new OutputDevice(PIN_3, PIN_4);

    CodeDetector::instance = new CodeDetectorSerial(PIN_A1, PIN_A2, PIN_A3, PIN_A4, PIN_A5);
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

    game.start(2, false);
}

//-----------------------------------------------------------------------------

/**
 * @brief loop main loop for the program.
 */
void loop() {

    if (!game.isStarted()) {
        if (playerChooser.detectNumPlayers() && CodeDetector::instance->getActiveCode() == CODE_ALL) {
            game.start(playerChooser.getNumPlayers());
        }
    } else {
        //game running
        game.loop();
    }
}

//-----------------------------------------------------------------------------

