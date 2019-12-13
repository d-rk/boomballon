#include "PiezoBuzzer.h"
#include <Arduino.h>
#include "Constants.h"
#include <Tasks/TaskScheduler.h>

//-----------------------------------------------------------------------------

PiezoBuzzer* PiezoBuzzer::instance = NULL;

//-----------------------------------------------------------------------------

const uint16_t PiezoBuzzer::beverlyHillsMelody[] = {NOTE_PAUSE, NOTE_FS3, NOTE_PAUSE, NOTE_A3, NOTE_PAUSE, NOTE_FS3, NOTE_FS3, NOTE_B3, NOTE_FS3, NOTE_E3,
                                                    NOTE_FS3, NOTE_PAUSE, NOTE_CS4, NOTE_PAUSE,NOTE_FS3, NOTE_FS3, NOTE_D4, NOTE_CS4, NOTE_A3,
                                                    NOTE_FS3, NOTE_CS4, NOTE_FS4, NOTE_FS3, NOTE_E3, NOTE_E3, NOTE_CS3, NOTE_GS3, NOTE_FS3, NOTE_FS3};
const uint8_t  PiezoBuzzer::beverlyHillsRhythm[] = {2, 8, 8, 8, 64, 8, 8, 8, 8, 8,
                                                    8, 8, 8, 64, 8, 8, 8, 8, 8,
                                                    8, 8, 8, 16, 8, 16, 8, 8, 8, 2};

const uint16_t PiezoBuzzer::aTeamMelody[] = {NOTE_G3, NOTE_G3, NOTE_F3, NOTE_G3, NOTE_PAUSE, NOTE_F3, NOTE_PAUSE, NOTE_G3,
                                         NOTE_PAUSE, NOTE_F3, NOTE_PAUSE, NOTE_F3, NOTE_C4, NOTE_AS3,
                                         NOTE_G3, NOTE_G3, NOTE_F3, NOTE_G3, NOTE_PAUSE, NOTE_F3, NOTE_PAUSE, NOTE_DS3,
                                         NOTE_PAUSE, NOTE_DS3, NOTE_PAUSE, NOTE_F3, NOTE_F3};
const uint8_t  PiezoBuzzer::aTeamRhythm[] = {8, 8, 8, 8, 8, 8, 8, 8,
                                         8, 8, 8, 8, 4, 4,
                                         8, 8, 8, 8, 8, 8, 8, 8,
                                         8, 8, 8, 8, 2};

//-----------------------------------------------------------------------------

PiezoBuzzer::PiezoBuzzer(uint8_t pin)
    : PIN(pin),
      currentMelody(NULL),
      currentRhythm(NULL),
      melodyLength(0),
      rhythmLength(0),
      currentToneIndex(0),
      toneFinishedMs(0L),
      dynamicMelody(Vector<uint16_t>(5)),
      dynamicRhythm(Vector<uint8_t>(5))
{
}

//-----------------------------------------------------------------------------

void PiezoBuzzer::setup() {
    noTone(PIN);
}

//-----------------------------------------------------------------------------

bool PiezoBuzzer::applyChange(unsigned long timeMs) {

    if (currentMelody != NULL && currentRhythm != NULL) {

        if (timeMs >= toneFinishedMs) {
            currentToneIndex++;

            if (currentToneIndex >= melodyLength) {
                //finished - stop playing
                noTone(PIN);
                currentMelody = NULL;
                currentRhythm = NULL;
                return false;
            } else {
                // start playing next tone
                // to calculate the note duration, take one second
                // divided by the note type.
                //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
                int noteDuration = 1000 / currentRhythm[currentToneIndex % rhythmLength];
                tone(PIN, currentMelody[currentToneIndex], noteDuration);

                // to distinguish the notes, set a minimum time between them.
                // the note's duration + 30% seems to work well:
                toneFinishedMs = timeMs + noteDuration * 1.30;
                return true;
            }
        }
        return true;
    } else {
        return false;
    }
}

//-----------------------------------------------------------------------------

void PiezoBuzzer::setMelody(Melody melody, Rhythm rhythm) {

    switch(melody) {
        case M_BEVERLY_HILLS:
            currentMelody = &(beverlyHillsMelody[0]);
            melodyLength = sizeof(beverlyHillsMelody)/sizeof(*beverlyHillsMelody);
            break;
        case M_ATEAM:
            currentMelody = &(aTeamMelody[0]);
            melodyLength = sizeof(aTeamMelody)/sizeof(*aTeamMelody);
            break;
        default:
            currentMelody = NULL;
            melodyLength = 0;
            break;
    }

    switch(rhythm) {
        case R_BEVERLY_HILLS:
            currentRhythm = &(beverlyHillsRhythm[0]);
            rhythmLength = sizeof(beverlyHillsRhythm)/sizeof(*beverlyHillsRhythm);
            break;
        case R_ATEAM:
            currentRhythm = &(aTeamRhythm[0]);
            rhythmLength = sizeof(aTeamRhythm)/sizeof(*aTeamRhythm);
            break;
        default:
            currentRhythm = NULL;
            rhythmLength = 0;
            break;
    }

    currentToneIndex = -1;
    toneFinishedMs = 0;

    TaskScheduler::instance->addTask(this);
}

//-----------------------------------------------------------------------------

void PiezoBuzzer::playTone(uint16_t tone, uint8_t noteType, uint8_t repetitions, uint8_t noteTypePause) {

    dynamicMelody.clear();
    dynamicRhythm.clear();

    for (uint8_t i=0; i < repetitions; i++) {
        dynamicMelody.push_back(tone);
        dynamicMelody.push_back(NOTE_PAUSE);
        dynamicRhythm.push_back(noteType);
        dynamicRhythm.push_back(noteTypePause);
    }

    currentMelody = &(dynamicMelody[0]);
    melodyLength = dynamicMelody.size();

    currentRhythm = &(dynamicRhythm[0]);
    rhythmLength = dynamicRhythm.size();

    currentToneIndex = -1;
    toneFinishedMs = 0;

    TaskScheduler::instance->addTask(this);
}

//-----------------------------------------------------------------------------


