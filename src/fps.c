#include "ct2.h"

u32 lag_frames = 0;
u32 frame_count = 0;
u8 curFrameTimeIndex = 0;

#define FRAMETIME_COUNT 30
#define	OS_CPU_COUNTER		(OS_CLOCK_RATE*3/4)

OSTime frameTimes[FRAMETIME_COUNT] = {0};

void cCompareVICount(void) {
    lag_frames = (__osVIIntrCount / 2) - frame_count; //if 0, no lag occurred
    frame_count++;
}

// Call once per frame
f32 calculate_and_update_fps(void) {
    u32 newTime = osGetCount();
    u32 oldTime = frameTimes[curFrameTimeIndex];
    frameTimes[curFrameTimeIndex] = newTime;

    curFrameTimeIndex++;
    if (curFrameTimeIndex >= FRAMETIME_COUNT) {
        curFrameTimeIndex = 0;
    }

    return (1000000.0f * FRAMETIME_COUNT * (OS_CPU_COUNTER/15625.0f) / (1000000.0f/15625.0f)) / (f32)(newTime - oldTime);
}

void print_lag_frames(s32 x, s32 y, TextColor* color) {
    char buffer[16];

    _bzero(buffer, sizeof(buffer));
    _sprintf(buffer, "%d", lag_frames);
    SetDefaultTextParametersWithColor(color, x, y);
    PrintDebugText(buffer);
}

void print_fps(s32 x, s32 y, TextColor* color) {
    f32 fps = calculate_and_update_fps();
    char buffer[16];

    _bzero(buffer, sizeof(buffer));
    _sprintf(buffer, "%2.2f", fps);
    SetDefaultTextParametersWithColor(color, x, y);
    PrintDebugText(buffer);
}
