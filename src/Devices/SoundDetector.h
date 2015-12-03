#ifndef SOUNDDETECTOR_H
#define SOUNDDETECTOR_H

#include <Tasks/Task.h>
#include <Helper/Vector.h>

#include <stdint.h>

class SoundDetector : public Task
{
public:
    SoundDetector(uint8_t pin);

    void setup();

    virtual bool applyChange(unsigned long timeMs);

    void setActive(bool active);

private:
    const uint8_t PIN;
    bool active;
public:
    static SoundDetector* instance;
};

#endif // SOUNDDETECTOR_H
