#ifndef SEVENSEGMENTDISPLAY_H
#define SEVENSEGMENTDISPLAY_H

#include <stdint.h>
#include <Helper/Vector.h>
#include <Tasks/Task.h>

enum DisplayAnimation {ANIM_P1_FLOATING, ANIM_P2_FLOATING, ANIM_P3_FLOATING, ANIM_P4_FLOATING, ANIM_P5_FLOATING,
                       ANIM_2P, ANIM_3P, ANIM_4P, ANIM_5P, ANIM_ERROR, ANIM_CIRCLE, ANIM_CIRCLE2, ANIM_FILL, ANIM_SNAKE, ANIM_COUNTDOWN};

class SevenSegmentDisplay : public Task
{
public:
    SevenSegmentDisplay(uint8_t latchPin, uint8_t clockPin, uint8_t dataPin);

    void setup();

    virtual bool applyChange(unsigned long timeMs);

    void setCharacter(const uint8_t character);
    void setNumber(const uint8_t number);

    void setAnimation(DisplayAnimation animation, uint16_t frameMs, bool reverse, bool invert, uint8_t loops = 1);

    void stopAnimation();

private:
    const uint8_t PIN_LATCH;
    const uint8_t PIN_CLOCK;
    const uint8_t PIN_DATA;

    const uint8_t* currentAnimation;

    int8_t   currentSign;
    uint8_t  currentFrame;
    uint8_t  frameCount;
    uint16_t frameMs;
    bool     reverse;
    bool     invert;
    uint8_t  loops;

    unsigned long frameFinishedMs;

    static const uint8_t    animationP1Floating[];
    static const uint8_t    animationP2Floating[];
    static const uint8_t    animationP3Floating[];
    static const uint8_t    animationP4Floating[];
    static const uint8_t    animationP5Floating[];
    static const uint8_t    animationError[];
    static const uint8_t    animation2P[];
    static const uint8_t    animation3P[];
    static const uint8_t    animation4P[];
    static const uint8_t    animation5P[];
    static const uint8_t    animationCircle[];
    static const uint8_t   animationCircle2[];
    static const uint8_t      animationFill[];
    static const uint8_t     animationSnake[];
    static const uint8_t animationCountdown[];

public:
    static SevenSegmentDisplay* instance;
};

#endif // SEVENSEGMENTDISPLAY_H
