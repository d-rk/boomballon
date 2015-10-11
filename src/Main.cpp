#include <Arduino.h>

#include <Button.h>
#include <Led.h>
#include <Game.h>
#include <PlayerChooser.h>

#include <Dummy/CodeDetectorDummy.h>
#include <Dummy/OutputDeviceDummy.h>

#include <Helper/Log.h>

#include <Constants.h>

//-----------------------------------------------------------------------------

Led redLed(PIN_8);
Led greenLed(PIN_2);
OutputDevice outputDevice(PIN_9, PIN_4);
CodeDetector codeDetector(PIN_A1, PIN_A2, PIN_A3, PIN_A4, PIN_A5);
PlayerChooser playerChooser(CODE_2, CODE_3, CODE_4, CODE_5);
Game game;

//-----------------------------------------------------------------------------

/**
 * @brief setup setup everything needed for the program to run.
 */
void setup() {
    Log::setup();
    randomSeed(analogRead(PIN_A0)); //PIN 0 needs to be unconnected
    redLed.setup();
    greenLed.setup();
    codeDetector.setup();
    outputDevice.setup();
    playerChooser.setup(&codeDetector, &redLed, &greenLed);
    game.setup(&codeDetector, &outputDevice, &redLed, &greenLed);
    redLed.setOn(true);
    greenLed.setOn(false);

    playerChooser.setNumPlayers(2);
}

//-----------------------------------------------------------------------------

/**
 * @brief loop main loop for the program.
 */
void loop() {

    if (!game.isStarted()) {
        if (playerChooser.detectNumPlayers() && codeDetector.getActiveCode() == CODE_ALL) {
            printf("\nStarting game with %1d players.\n", playerChooser.getNumPlayers());
            game.start(playerChooser.getNumPlayers());
        }
    } else {
        //game running
        game.loop();
    }
}

//-----------------------------------------------------------------------------

