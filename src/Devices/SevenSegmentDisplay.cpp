#include "SevenSegmentDisplay.h"
#include "Constants.h"
#include <Tasks/TaskScheduler.h>

#include <Arduino.h>

//-----------------------------------------------------------------------------

SevenSegmentDisplay* SevenSegmentDisplay::instance = NULL;

//-----------------------------------------------------------------------------



const uint8_t SevenSegmentDisplay::animationP1Floating[] = {B01000000, B01000000, B00000000, B00000000, B00100000, B00100000, CHAR_1};
const uint8_t SevenSegmentDisplay::animationP2Floating[] = {B00011010, B00010000, B00000000, B00011000, B01011010, CHAR_2};
const uint8_t SevenSegmentDisplay::animationP3Floating[] = {B00110010, B00010000, B00000000, B00110000, B01110010, CHAR_3};
const uint8_t SevenSegmentDisplay::animationP4Floating[] = {B00100010, B00100000, B00000000, B00100010, B00100110, CHAR_4};
const uint8_t SevenSegmentDisplay::animationP5Floating[] = {B00110010, B00010000, B00000000, B00110000, B00110110, CHAR_5};

const uint8_t SevenSegmentDisplay::animationError[] = {CHAR_MINUS, 0, CHAR_MINUS, 0};

const uint8_t SevenSegmentDisplay::animation2P[] = {CHAR_2, CHAR_P, 0};
const uint8_t SevenSegmentDisplay::animation3P[] = {CHAR_3, CHAR_P, 0};
const uint8_t SevenSegmentDisplay::animation4P[] = {CHAR_4, CHAR_P, 0};
const uint8_t SevenSegmentDisplay::animation5P[] = {CHAR_5, CHAR_P, 0};

const uint8_t SevenSegmentDisplay::animationCircle[]    = {B10000000, B01000000, B00100000, B00010000, B00001000, B00000100};
const uint8_t SevenSegmentDisplay::animationCircle2[]   = {B11000000, B01100000, B00110000, B00011000, B00001100, B10000100};
const uint8_t SevenSegmentDisplay::animationFill[]      = {B00000001, B00010001, B00111001, B00111011, B01111111, B11111111};
const uint8_t SevenSegmentDisplay::animationSnake[]     = {B00010000, B00011000, B00011010, B01001010, B11000010, B11000100, B10000110, B00100110, B00110010, B00110000};
const uint8_t SevenSegmentDisplay::animationCountdown[] = {CHAR_3, CHAR_2, CHAR_1, CHAR_0};

//-----------------------------------------------------------------------------

SevenSegmentDisplay::SevenSegmentDisplay(uint8_t latchPin, uint8_t clockPin, uint8_t dataPin)
    : PIN_LATCH(latchPin),
      PIN_CLOCK(clockPin),
      PIN_DATA(dataPin),
      currentAnimation(NULL),
      currentSign(1),
      currentFrame(0),
      frameCount(0),
      frameMs(0),
      reverse(false),
      invert(false),
      loops(1),
      frameFinishedMs(0L)
{
}

//-----------------------------------------------------------------------------

void SevenSegmentDisplay::setup() {
    pinMode(PIN_LATCH, OUTPUT);
    pinMode(PIN_CLOCK, OUTPUT);
    pinMode(PIN_DATA,  OUTPUT);
    setCharacter(0);
}

//-----------------------------------------------------------------------------

bool SevenSegmentDisplay::applyChange(unsigned long timeMs) {

    if (currentAnimation != NULL) {

        if (timeMs >= frameFinishedMs) {
            currentFrame += currentSign;

            if (currentFrame >= frameCount || currentFrame == -1) {
                //animation finished
                if (loops != 1) {
                    //restart
                    currentSign = reverse ? -currentSign : currentSign;
                    currentFrame = currentSign == 1 ? -1 : frameCount;
                    frameFinishedMs = 0;
                    loops = (loops != 0) ? loops-1 : 0;
                    return true;
                } else {
                    //stop
                    setCharacter(0);
                    currentAnimation = NULL;
                    return false;
                }
            } else {
                //frame changed
                if (invert) {
                    setCharacter(~(currentAnimation[currentFrame]));
                } else {
                    setCharacter(currentAnimation[currentFrame]);
                }
                frameFinishedMs = timeMs + frameMs;
            }
        }

        return true;

    } else {
        return false;
    }
}

//-----------------------------------------------------------------------------

void SevenSegmentDisplay::setCharacter(const uint8_t character) {
    digitalWrite(PIN_LATCH, LOW);
    shiftOut(PIN_DATA, PIN_CLOCK, LSBFIRST, character);
    digitalWrite(PIN_LATCH, HIGH);
}

//-----------------------------------------------------------------------------

void SevenSegmentDisplay::setNumber(const uint8_t number) {
    switch (number) {
        case 0: setCharacter(CHAR_0); break;
        case 1: setCharacter(CHAR_1); break;
        case 2: setCharacter(CHAR_2); break;
        case 3: setCharacter(CHAR_3); break;
        case 4: setCharacter(CHAR_4); break;
        case 5: setCharacter(CHAR_5); break;
        case 6: setCharacter(CHAR_6); break;
        case 7: setCharacter(CHAR_7); break;
        case 8: setCharacter(CHAR_8); break;
        case 9: setCharacter(CHAR_9); break;
        default: setCharacter(0); break;
    }
}

//-----------------------------------------------------------------------------

void SevenSegmentDisplay::setAnimation(DisplayAnimation animation, uint16_t frameMs, bool reverse, bool invert, uint8_t loops) {

    this->frameMs = frameMs;
    this->reverse = reverse;
    this->invert  = invert;
    this->loops  = loops;

    switch(animation) {
        case ANIM_P1_FLOATING:
            currentAnimation = &(animationP1Floating[0]);
            frameCount = sizeof(animationP1Floating)/sizeof(*animationP1Floating);
            break;
        case ANIM_P2_FLOATING:
            currentAnimation = &(animationP2Floating[0]);
            frameCount = sizeof(animationP2Floating)/sizeof(*animationP2Floating);
            break;
        case ANIM_P3_FLOATING:
            currentAnimation = &(animationP3Floating[0]);
            frameCount = sizeof(animationP3Floating)/sizeof(*animationP3Floating);
            break;
        case ANIM_P4_FLOATING:
            currentAnimation = &(animationP4Floating[0]);
            frameCount = sizeof(animationP4Floating)/sizeof(*animationP4Floating);
            break;
        case ANIM_P5_FLOATING:
            currentAnimation = &(animationP5Floating[0]);
            frameCount = sizeof(animationP5Floating)/sizeof(*animationP5Floating);
            break;
        case ANIM_2P:
            currentAnimation = &(animation2P[0]);
            frameCount = sizeof(animation2P)/sizeof(*animation2P);
            break;
        case ANIM_3P:
            currentAnimation = &(animation3P[0]);
            frameCount = sizeof(animation3P)/sizeof(*animation3P);
            break;
        case ANIM_4P:
            currentAnimation = &(animation4P[0]);
            frameCount = sizeof(animation4P)/sizeof(*animation4P);
            break;
        case ANIM_5P:
            currentAnimation = &(animation5P[0]);
            frameCount = sizeof(animation5P)/sizeof(*animation5P);
            break;
        case ANIM_ERROR:
            currentAnimation = &(animationError[0]);
            frameCount = sizeof(animationError)/sizeof(*animationError);
            break;
        case ANIM_CIRCLE:
            currentAnimation = &(animationCircle[0]);
            frameCount = sizeof(animationCircle)/sizeof(*animationCircle);
            break;
        case ANIM_CIRCLE2:
            currentAnimation = &(animationCircle2[0]);
            frameCount = sizeof(animationCircle2)/sizeof(*animationCircle2);
            break;
        case ANIM_SNAKE:
            currentAnimation = &(animationSnake[0]);
            frameCount = sizeof(animationSnake)/sizeof(*animationSnake);
            break;
        case ANIM_FILL:
            currentAnimation = &(animationFill[0]);
            frameCount = sizeof(animationFill)/sizeof(*animationFill);
            break;
        case ANIM_COUNTDOWN:
            currentAnimation = &(animationCountdown[0]);
            frameCount = sizeof(animationCountdown)/sizeof(*animationCountdown);
            break;
        default:
            currentAnimation = NULL;
            frameCount = 0;
            break;
    }

    currentSign     = 1;
    currentFrame    = -1;
    frameFinishedMs = 0;

    TaskScheduler::instance->addTask(this);
}

//-----------------------------------------------------------------------------

void SevenSegmentDisplay::stopAnimation() {
    setCharacter(0);
    currentAnimation = NULL;
    frameCount       =  0;
    currentSign      =  1;
    currentFrame     = -1;
    frameFinishedMs  =  0;
    TaskScheduler::instance->removeTask(this);
}

//-----------------------------------------------------------------------------
