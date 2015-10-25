#ifndef PIEZOBUZZER_H
#define PIEZOBUZZER_H

#include <Tasks/Task.h>
#include <Helper/Vector.h>

#include <stdint.h>

class PiezoBuzzer : public Task
{
public:
    enum Melody {M_BEVERLY_HILLS, MELODY_1, MELODY_2};
    enum Rhythm {R_BEVERLY_HILLS, RHYTHM_1, RHYTHM_2};

    PiezoBuzzer(uint8_t pin);

    void setup();

    virtual bool applyChange(unsigned long timeMs);

    void setMelody(Melody melody, Rhythm rhythm);

    void playTone(uint16_t tone, uint8_t toneMs, uint8_t repetitions = 1, uint8_t noteTypePause = 255);

private:
    const uint8_t PIN;
    const uint16_t* currentMelody;
    const uint8_t*  currentRhythm;
    uint8_t melodyLength;
    uint8_t rhythmLength;
    uint8_t currentToneIndex;
    unsigned long toneFinishedMs;

    static const uint16_t     melody1[];
    static const uint8_t     rhythm1[];

    static const uint16_t     melody2[];
    static const uint8_t     rhythm2[];

    static const uint16_t    beverlyHillsMelody[];
    static const uint8_t     beverlyHillsRhythm[];

    Vector<uint16_t> dynamicMelody;
    Vector<uint8_t>  dynamicRhythm;

public:
    static PiezoBuzzer* instance;
};

#endif // PIEZOBUZZER_H
