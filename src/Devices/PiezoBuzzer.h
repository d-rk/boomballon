#ifndef PIEZOBUZZER_H
#define PIEZOBUZZER_H

#include <Tasks/Task.h>
#include <Helper/Vector.h>

#include <stdint.h>

class PiezoBuzzer : public Task
{
public:
    enum Melody {M_BEVERLY_HILLS, M_ATEAM};
    enum Rhythm {R_BEVERLY_HILLS, R_ATEAM};

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

    static const uint16_t    aTeamMelody[];
    static const uint8_t     aTeamRhythm[];

    static const uint16_t    beverlyHillsMelody[];
    static const uint8_t     beverlyHillsRhythm[];

    Vector<uint16_t> dynamicMelody;
    Vector<uint8_t>  dynamicRhythm;

public:
    static PiezoBuzzer* instance;
};

#endif // PIEZOBUZZER_H
