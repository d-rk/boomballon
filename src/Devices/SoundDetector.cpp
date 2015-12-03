#include "SoundDetector.h"
#include <Arduino.h>
#include <Tasks/TaskScheduler.h>

//-----------------------------------------------------------------------------

SoundDetector* SoundDetector::instance = NULL;

//-----------------------------------------------------------------------------

SoundDetector::SoundDetector(uint8_t pin)
    : PIN(pin),
      active(false)
{
}

//-----------------------------------------------------------------------------

void SoundDetector::setup() {

}

//-----------------------------------------------------------------------------

bool SoundDetector::applyChange(unsigned long timeMs) {
    int value = analogRead(PIN);
    Serial.println(value);
    return true;
}

//-----------------------------------------------------------------------------

void SoundDetector::setActive(bool active) {

    if (this->active == active) return;

    if (active) {
        TaskScheduler::instance->addTask(this);
    } else {
        TaskScheduler::instance->removeTask(this);
    }
    this->active = active;
}

//-----------------------------------------------------------------------------


