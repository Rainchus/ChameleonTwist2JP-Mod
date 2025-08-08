#include "../include/ct2.h"
#include <stdarg.h>

void _sprintf(void* destination, void* fmt, ...);
void SetDefaultTextParametersWithColor(TextColor* color, s32 x, s32 y);
OSTime osClockRate = OS_CLOCK_RATE;

#define NOT_RUNNING 0
#define RUNNING 1
#define PAUSED 2

u64 elapsedMicroseconds = 0;
u64 elapsedCounts = 0;
u32 startingCount = 0;
u32 prevCount = 0;
u32 timerState = 0;



void SetTimerStuff(void) {
    elapsedCounts = 0;
    startingCount = osGetCount();
    prevCount = startingCount;
    elapsedMicroseconds = 0;
    timerState = NOT_RUNNING;
}

s32 afterBossHpSet(s32 arg0) {
    elapsedCounts = 0;
    startingCount = osGetCount();
    prevCount = startingCount;
    elapsedMicroseconds = 0;
    timerState = RUNNING;
    gTimer = 0;

    return arg0; //preserve from hook
}

u32 freezeTimer = 0;
u32 gTimerStored = 0;

void MainTimer(void) {
    TextPosition textPos = {130, 209};
    TextPosition textPos2 = {220, 209};
    char timeString[64];
    char healthString[32];

    u64 curCount = osGetCount();
    u32 elapsedCountThisFrame = 0;

    u32 milliseconds, seconds, minutes;

    u32 gTimeDisplayed;
    f64 totalMilliseconds;

    if (!toggles[TOGGLE_TIMER_MAIN]) {
        return;
    }


    if (toggles[TOGGLE_TIMER_DISPLAY_VOID]) {
        if (gDoingVoidOut) {
            freezeTimer = 90;
            gDoingVoidOut = 0;
            gTimerStored = gTimer;
        }
    }

    if (toggles[TOGGLE_TIMER_DISPLAY_ZONE_CHANGE]) {
        if (gZoneChange) {
            if (gTimer > 1) {
                freezeTimer = 90;
                gZoneChange = 0;
                gTimerStored = gTimer;
            } else {
                gZoneChange = 0;
            }
        }
    }


    if (freezeTimer > 0) {
        freezeTimer--;
        totalMilliseconds = gTimerStored * (1000.0 / 30.0); // total time in ms
        SetDefaultTextParametersWithColor(&Cyan, textPos.xPos, textPos.yPos);
    } else {
        totalMilliseconds = gTimer * (1000.0 / 30.0); // total time in ms
        SetDefaultTextParametersWithColor(&RedOrange, textPos.xPos, textPos.yPos);
    }


    int totalSeconds = (int)(totalMilliseconds / 1000.0);

    minutes = totalSeconds / 60;
    seconds = totalSeconds % 60;
    milliseconds = (int)totalMilliseconds % 1000;



    _bzero(timeString, sizeof(timeString));
    

    _sprintf(timeString, "%02d\'%02d\"%03d", minutes, seconds, milliseconds);
    printDebugText(timeString);

    //print boss hp

    if (gBossHp != 0.0f) {
        // timerState = PAUSED; //paused
        _bzero(healthString, sizeof(healthString));
        SetDefaultTextParametersWithColor(&RedOrange, textPos2.xPos, textPos2.yPos);

        _sprintf(healthString, "HP: %.2f", gBossHp);
        printDebugText(healthString);
    }
}

//boss hp as a float is at 8016063C
//800A058C hook here