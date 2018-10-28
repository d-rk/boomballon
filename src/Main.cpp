#include <Arduino.h>

#include <Game.h>
#include <PlayerChooser.h>
#include <Devices/CodeDetector.h>
#include <Devices/OutputDevice.h>
#include <Devices/PiezoBuzzer.h>
#include <Devices/SevenSegmentDisplay.h>
#include <Tasks/TaskScheduler.h>
#include <Helper/Log.h>

#include <Constants.h>

//#include <Dummy/CodeDetectorSerial.h>

//-----------------------------------------------------------------------------

Game game;

//-----------------------------------------------------------------------------

/**
 * @brief setup setup everything needed for the program to run.
 */
void setup() {

    // wait two seconds so that the serial connection is established
    delay(2000);

    Log::setup();
    randomSeed(analogRead(PIN_A5)); //PIN 5 needs to be unconnected

    PlayerChooser::instance = new PlayerChooser(CODE_15, CODE_17, CODE_19, CODE_21, CODE_23);

    TaskScheduler::instance = new TaskScheduler();
    OutputDevice::instance = new OutputDevice(PIN_6, PIN_7);

    CodeDetector::instance = new CodeDetector(PIN_A0, PIN_A1, PIN_A2, PIN_A3, PIN_A4);
    PiezoBuzzer::instance  = new PiezoBuzzer(PIN_11);
    SevenSegmentDisplay::instance = new SevenSegmentDisplay(PIN_9, PIN_10, PIN_8);

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

