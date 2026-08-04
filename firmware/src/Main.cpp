#include <Arduino.h>

#include <Constants.h>

#include <Game.h>
#include <PlayerChooser.h>
#include <Devices/CodeDetector.h>
#include <Devices/OutputDevice.h>
#include <Devices/PiezoBuzzer.h>
#include <Devices/SevenSegmentDisplay.h>
#include <Tasks/TaskScheduler.h>
#include <Helper/Log.h>

#ifdef MOCK_CODE_DETECTOR
#include <Mock/CodeDetectorMock.h>
#endif
#ifdef MOCK_OUTPUT_DEVICE
#include <Mock/OutputDeviceMock.h>
#endif

//-----------------------------------------------------------------------------

Game game;

#ifdef DETECTOR_CALIBRATION
bool paused = false;
bool displayChangesOnly = false;
#endif

//-----------------------------------------------------------------------------

/**
 * @brief setup setup everything needed for the program to run.
 */
void setup() {

    SevenSegmentDisplay::instance = new SevenSegmentDisplay(PIN_9, PIN_10, PIN_8);
    SevenSegmentDisplay::instance->setup();
    SevenSegmentDisplay::instance->setCharacter(CHAR_MINUS);

    // wait two seconds so that the serial connection is established
    #if defined(LOGGING_ENABLED) || defined(MOCK_CODE_DETECTOR) || defined(MOCK_OUTPUT_DEVICE)
    delay(2500);
    #endif
    SevenSegmentDisplay::instance->setCharacter(CHAR_DOT);

    Log::setup();
    randomSeed(analogRead(PIN_A5)); //PIN 5 needs to be unconnected

    PlayerChooser::instance = new PlayerChooser(CODE_15, CODE_17, CODE_19, CODE_21, CODE_23);

    TaskScheduler::instance = new TaskScheduler();

    #ifdef MOCK_OUTPUT_DEVICE
    OutputDevice::instance = new OutputDeviceMock(PIN_6, PIN_7);
    #else
    OutputDevice::instance = new OutputDevice(PIN_6, PIN_7);
    #endif

    #ifdef MOCK_CODE_DETECTOR
    CodeDetector::instance = new CodeDetectorMock(PIN_A0, PIN_A1, PIN_A2, PIN_A3, PIN_A4);
    #else
    CodeDetector::instance = new CodeDetector(PIN_A0, PIN_A1, PIN_A2, PIN_A3, PIN_A4);
    #endif

    PiezoBuzzer::instance  = new PiezoBuzzer(PIN_11);


    OutputDevice::instance->setup();
    CodeDetector::instance->setup();
    PiezoBuzzer::instance->setup();

    #ifdef LOGGING_ENABLED
    #ifndef DETECTOR_CALIBRATION
    printf("=================================\n");
    printf("=== Select number of players  ===\n");
    printf("=================================\n");
    #else
    printf("=====================================\n");
    printf("=== <SPACE> = Pause               ===\n");
    printf("===   <TAB> = Toggle Display Mode ===\n");
    printf("=====================================\n");
    #endif
    #endif

    #ifdef AUTOSTART_GAME
    game.start(2, false);
    #endif
}

//-----------------------------------------------------------------------------

#ifndef DETECTOR_CALIBRATION
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

#else // DETECTOR_CALIBRATION
void loop() {

    if (!paused) {
        bool codeChanged = CodeDetector::instance->codeChanged();
        CodeDetector::instance->printRawValues(codeChanged, displayChangesOnly);
    }

    int sleep = 0;

    while (sleep++ < 500) {
        if (Serial.available() > 0) {
            char receivedChar = Serial.read();

            if (receivedChar == CHAR_SPACE) {
                paused = !paused;
                if (paused) {
                    printf("<PAUSED>\n");
                }
            } else if (receivedChar == CHAR_TAB) {
                displayChangesOnly = !displayChangesOnly;
            }
        }
        delay(1);
    }
}
#endif

//-----------------------------------------------------------------------------

