#include <Arduino.h>

#include <Led.h>
#include <Game.h>
#include <PlayerChooser.h>

#include <Dummy/CodeDetectorDummy.h>
#include <Dummy/OutputDeviceDummy.h>

#include <Constants.h>

//-----------------------------------------------------------------------------

Led redLed(PIN_12);
Led greenLed(PIN_13);
OutputDeviceDummy outputDevice(PIN_2, PIN_3, PIN_4, PIN_5, PIN_7, PIN_8, PIN_9, PIN_10);
CodeDetectorDummy codeDetector(PIN_1, PIN_2, PIN_3, PIN_4, PIN_5);
PlayerChooser playerChooser(CODE_2, CODE_3, CODE_4, CODE_5);
Game game;

//-----------------------------------------------------------------------------

void setup() {
    Serial.begin(9600);
    redLed.setup();
    greenLed.setup();
    codeDetector.setup();
    outputDevice.setup();
    playerChooser.setup(&codeDetector, &redLed, &greenLed);
    game.setup(&codeDetector, &outputDevice, &redLed, &greenLed);
    redLed.setOn(false);
    greenLed.setOn(false);
}

//-----------------------------------------------------------------------------

void loop() {

    if (!game.isStarted()) {
        if (playerChooser.detectNumPlayers() && codeDetector.getActiveCode() == CODE_NONE) {
            game.start(playerChooser.getNumPlayers());
            Serial.println("Game started");
        }
    } else {
        //game running
        game.loop();
    }
}

//-----------------------------------------------------------------------------

