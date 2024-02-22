#include "ct2.h"
#include <stdarg.h>

s32 curPowerup = 1;
s32 initCrashScreen = 0;

void crash_screen_init(void);

TextColor RedOrange = {
    {255, 255, 0, 255},
    {255, 0, 0, 255},
    {255, 0, 0, 255},
    {255, 255, 0, 255},
};

extern f32 powerUpFloatArray[8];

f32 alwaysSpeedUpPowerUp[] = {
    0, 1.0f, 0, 0,
    0, 0, 0, 0
};

f32 always3XPowerUp[] = {
    0, 0, 0, 0,
    1.0f, 0, 0, 0
};

f32 randomPowerUps[] = {
    0.125f, 0.25f, 0.375f, 0.5f,
    0.625f, 0.75f, 0.875f, 1.0f
};

void SetTextColor(TextColor* color) {
    setTextGradient(0, color->zero.r, color->zero.g, color->zero.b, color->zero.a); //args: unk, r, g, b, a
    setTextGradient(1, color->one.r, color->one.g, color->one.b, color->one.a); //args: unk, r, g, b, a
    setTextGradient(2, color->two.r, color->two.g, color->two.b, color->two.a); //args: unk, r, g, b, a
    setTextGradient(3, color->three.r, color->three.g, color->three.b, color->three.a); //args: unk, r, g, b, a
    ifTextColor = 1;
}

void SetTextWidthAndHeight(f32 width, f32 height) {
    textWidth = width;
    textHeight = height;
}

void SetDefaultTextParametersWithColor(TextColor* color, s32 x, s32 y) {
    textStyle = 1;
    textKerning = 1;
    setDebugTextPosition(x, y, 0x32);
    SetTextWidthAndHeight(0.75f, 0.75f);
    SetTextColor(color);
}

void printCustomTextInC(void) {
    char buffer[16];
    TextPosition textPos = {20, 208};

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&RedOrange, textPos.xPos, textPos.yPos);

    switch (curPowerup) {
        case 0:
            _sprintf(buffer, "%s", "Normal");
            memcpy(powerUpFloatArray, randomPowerUps, sizeof(powerUpFloatArray));
            break;
        case 1:
            _sprintf(buffer, "%s", "Speed");
            memcpy(powerUpFloatArray, alwaysSpeedUpPowerUp, sizeof(powerUpFloatArray));
            break;
        case 2:
            _sprintf(buffer, "%s", "X3");
            memcpy(powerUpFloatArray, always3XPowerUp, sizeof(powerUpFloatArray));
            break;
    }
    PrintDebugText(buffer);        
}

void BeforeMainLoop(void) {
    if (initCrashScreen == 0) {
        crash_screen_init();
        initCrashScreen = 1;
    }

    if (currentlyPressedButtons & 0x0400) { //dpad down
        curPowerup++;
        if (curPowerup > 2) {
            curPowerup = 0;
        }
    }
}
