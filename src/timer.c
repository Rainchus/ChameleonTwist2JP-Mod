#include "../include/ct2.h"
#include <stdarg.h>

extern TextColor RedOrange;
void _sprintf(void* destination, void* fmt, ...);
void SetDefaultTextParametersWithColor(TextColor* color, s32 x, s32 y);
OSTime osClockRate = OS_CLOCK_RATE;

#define NOT_RUNNING 0
#define RUNNING 1
#define PAUSED 2

u64 elapsedMilliseconds = 0;
u64 elapsedCounts = 0;
u32 startingCount = 0;
u32 prevCount = 0;
u32 timerState = 0;

s32 afterBossHpSet(s32 arg0) {
    elapsedCounts = 0;
    startingCount = osGetCount();
    prevCount = startingCount;
    elapsedMilliseconds = 0;
    timerState = RUNNING;

    return arg0; //preserve from hook
}

void MainTimer(void) {
    TextPosition textPos = {130, 209};
    char timeString[64];

    u64 curCount = osGetCount();
    u32 elapsedCountThisFrame = 0;

    u32 milliseconds, seconds, minutes;

    if (timerState == NOT_RUNNING) {
        return;
    }

    if (timerState != PAUSED) {
        if (curCount < prevCount) {
            curCount += 0x100000000;
        }

        elapsedCountThisFrame = curCount - prevCount;
        prevCount = curCount;

        elapsedCounts += elapsedCountThisFrame;
    }

    elapsedMilliseconds = OS_CYCLES_TO_USEC(elapsedCounts);
    milliseconds = (elapsedMilliseconds / 1000) % 1000;
    seconds = (elapsedMilliseconds / 1000000) % 60;
    minutes = (elapsedMilliseconds / 1000000) / 60;

    _bzero(timeString, sizeof(timeString));
    SetDefaultTextParametersWithColor(&RedOrange, textPos.xPos, textPos.yPos);

    _sprintf(timeString, "%02d\'%02d\"%03d", minutes, seconds, milliseconds);
    printDebugText(timeString);

    if (bossHpBar == 0.0f) {
        timerState = PAUSED; //paused
    }
}

//boss hp as a float is at 8016063C
//800A058C hook here