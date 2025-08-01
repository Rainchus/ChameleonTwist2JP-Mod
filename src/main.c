#include "../include/ct2.h"
#include <stdarg.h>

extern Gfx Entity_YellowBlock_Render[];
u32 savestateGameMode = -1;
void print_lag_frames(s32 x, s32 y, TextColor* color);

s32 menuCurRespawnZone = 1;
s32 zoneLockout = 0;
u32 lag_frames = 0;
u32 frame_count = 0;
u8 curFrameTimeIndex = 0;
s32 prevCallCount = 0;
s32 callCopy = 0;
extern u32 __osVIIntrCount;
//80030AA0 texture entities

//func_800D6050 prints debug stuff
//800FF20C removes player control?
//func_8003573C renders world

//80035C68 chaos idea, delete function call here

s32 p1AirborneFrames = 0;
s32 holdDpadDirectionFrames = 0;
s32 prevOption = 0;
s32 pulledParasolTimer = 0;
s32 parasolPullFrame = 0;
f32 parasolPullSpeed = 0.0f;
s32 parasolPulled = 0;
f32 parasolPullAngle = 0.0f;
s32 printPositionBool = 0;

void print_fps(s32 x, s32 y, TextColor* color);

extern Gfx* gMainGfxPosPtr;

typedef struct MenuInstance {
    s16 cursor;
    s16 stageIndex;
    s16 unk_04;
    s16 unk_timer;
    u16 inputs;
} MenuInstance;

MenuInstance* MenuInstancePointer = 0;

typedef struct unkLookatStruct {
    char unk_00[0x48];
    f32 unk48;
    f32 unk4C;
    f32 unk50;
    f32 unk54;
    f32 unk58;
    f32 unk5C;
} unkLookatStruct;

extern s16 gGameMode;
extern s16 debugFlag;
void MainTimerBoss(void);

s32 stateCooldown = 0; //does this work??
s32 savestateCurrentSlot = 0;
extern s32 savestate1Size;
s32 savestate2Size = 0;
s32 savestate3Size = 0;
volatile s32 isSaveOrLoadActive = INACTIVE_STATE;
s32 saveOrLoadStateMode = 0;

TextColor RedOrange = {
    {255, 255, 0, 255},
    {255, 0, 0, 255},
    {255, 0, 0, 255},
    {255, 255, 0, 255},
};

TextColor Cyan = {
    {0x2A, 0xEE, 0xE9, 0xFF},
    {0x00, 0xC0, 0xDA, 0xFF},
    {0x2A, 0xEE, 0xE9, 0xFF},
    {0x00, 0xC0, 0xDA, 0xFF},
};

TextColor Purple = {
    {0xFF, 0x40, 0xFF, 0xFF},
    {0x00, 0x40, 0xFF, 0xFF},
    {0xFF, 0x40, 0xFF, 0xFF},
    {0xFF, 0x40, 0xFF, 0xFF}    
};

TextColor White = {
    {0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF}    
};

TextColor Green = {
    {0x00, 0xFF, 0x00, 0xFF},
    {0x00, 0xFF, 0x00, 0xFF},
    {0x00, 0xFF, 0x00, 0xFF},
    {0x00, 0xFF, 0x00, 0xFF}    
};

#define RANDOM 0
#define SPEED 1
#define X3 2

#define SAVE_MODE 0
#define LOAD_MODE 1

#define ramStartAddr 0x800E87D0 //
#define ramEndAddr 0x80400000 //can probably skip the frame buffer at around 0x80360000 - 0x80400000 approximately

extern CustomThread gCustomThread;
void RemoveTextColor(void);

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

void hookCode(s32* patchAddr, void* jumpLocation) {
    jumpLocation = (void*)(u32)((((u32)jumpLocation & 0x00FFFFFF) >> 2) | 0x08000000);
    patchAddr[0] = (s32)jumpLocation; //write j instruction
    patchAddr[1] = 0; //write nop
}

void patchInstruction(void* patchAddr, s32 patchInstruction) {
    *(s32*)patchAddr = patchInstruction;
}

int __osPiDeviceBusy() {
    register u32 stat = IO_READ(PI_STATUS_REG);
    if (stat & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY))
        return 1;
    return 0;
}

void pauseUntilDMANotBusy(void) {
	volatile s32* dmaBusyAddr = (s32*)0xA4600010;

	if (*dmaBusyAddr & 3) {
		while (*dmaBusyAddr & 3) {}
	}
}

#define ramAddrSavestateDataSlot1 (void*)0x804C0000
#define L_JPAD_CASE 0
#define U_JPAD_CASE 1
#define R_JPAD_CASE 2

u32 callsState = 0;
u32 callsAtPowerupDecisionState = 0;
u32 seedAtPowerupState = 0;
u32 callsEnteringSkylandState = 0;
u32 voidOutCallsState = 0;

void crash_screen_sleep(s32 ms);

void func_800CB430(void);

typedef struct unk4 {
    char unk_00[0x18];
    s32 unk18;
} unk4;

void func_800CB630(void*);
void func_800CBA4C(void*, void*, void*);
void func_800CC270(void*, s32, s32);
void func_800CD5D0(void*);
extern u16 D_800EAE78;
extern unk4 D_800F1C30;
extern u32 D_800F1C54;
extern u32 D_800F1C70;
void func_800CB5C0(void);

// void func_800CB430_Hook(void) {
//     while (isSaveOrLoadActive != INACTIVE_STATE) {}
//     D_800F1C30.unk18 = (s32) D_800EAE78;
//     func_800CBA4C(0x8033E400, 0x51400, &D_800F1C30);
//     func_800CC270(&D_800F1C54, 0x8000, 0);
//     func_800CC270(&D_800F1C54, 0x8000, 1);
//     func_800CD5D0(&D_800F1C70);
//     func_800CB630(&func_800CB5C0);
// }

void loadstateMain(void) {
    u32 mask;
    u32 savestateHeader = (*(u32*)ramAddrSavestateDataSlot1); //not really a header, but savestates always start with 0x4620610944222000

    mask = __osDisableInt();

    // //__osSpDeviceBusy
    // for (;;)
    // {
    //     tmp = IO_READ(SP_STATUS_REG);
    //     if ((tmp & 2) && (tmp & 1) && !(tmp & (SP_STATUS_DMA_BUSY | SP_STATUS_DMA_FULL)))
    //         break;
    // }

    // for (;;)
    // {
    //     tmp = IO_READ(DPC_STATUS_REG);
    //     if (!(tmp & 0x170))
    //         break;
    // }

    // for (;;)
    // {
    //     tmp = IO_READ(AI_STATUS_REG);
    //     if (!(tmp & 0xc0000001))
    //         break;
    // }

    // for (;;)
    // {
    //     tmp = IO_READ(PI_STATUS_REG);
    //     if (!(tmp & 3))
    //         break;
    // }

    // for (;;)
    // {
    //     tmp = IO_READ(SI_STATUS_REG);
    //     if (!(tmp & 3))
    //         break;
    // }

    //wait on rsp
    while (__osSpDeviceBusy() != 0) {}

    //wait on rdp
    while ( __osDpDeviceBusy() != 0) {}

    //wait on SI
    while (__osSiDeviceBusy() != 0) {}

    //wait on PI
    while (__osPiDeviceBusy() != 0) {}

    //invalidate caches
    // osInvalICache((void*)0x80000000, 0x4000);
	osInvalDCache((void*)0x80000000, 0x2000);
    
    //instead of checking savestate size, check savestate header to see if it's valid
    //this will allow the user to load a savestate after crashing
    
    if (savestateHeader == 0x03E00008) {
        customMemCpy((void*)ramStartAddr, ramAddrSavestateDataSlot1, (u32)ramEndAddr - (u32)ramStartAddr);
        calls = callCopy;
        osInvalICache((void*)0x80000000, 0x4000);
    }

    osWritebackDCacheAll();
    __osRestoreInt(mask);
    isSaveOrLoadActive = INACTIVE_STATE; //allow thread to continue
}

void savestateMain(void) {
    u32 mask;
    
    mask = __osDisableInt();

    osWritebackDCacheAll();
    //wait on rsp
    while (__osSpDeviceBusy() != 0) {}

    //wait on rdp
    while ( __osDpDeviceBusy() != 0) {}

    //wait on SI
    while (__osSiDeviceBusy() != 0) {}

    //wait on PI
    while (__osPiDeviceBusy() != 0) {}

    
    //invalidate caches
    osInvalICache((void*)0x80000000, 0x4000);
    osInvalDCache((void*)0x80000000, 0x2000);

    //savestate
    customMemCpy((void*)ramAddrSavestateDataSlot1, (void*)ramStartAddr, ramEndAddr - ramStartAddr);
    callCopy = calls;
    savestateGameMode = gGameMode;
    //savestate1Size = ramEndAddr - ramStartAddr;

    __osRestoreInt(mask);
    isSaveOrLoadActive = INACTIVE_STATE; //allow thread to continue
}

extern u8 D_80160648;
extern f32 D_80187B20; //is 0x20 into gPlayerActors[0]
extern s16 playerHealth;
extern u8 D_800ECFD0[]; //array of characters for integers
extern s32 D_800F6410; //?
extern f32 D_800F6498;
void* crash_screen_copy_to_buf(void* dest, const char* src, u32 size);
void setDebugTextPosition(s32 xPos, s32 yPos, s32 unk);
void formatText(void* buffer, void* string);
void printDebugText(void* string);
extern u32 rngSeed;
void recordCallsAtVoidOut(void);
s32 curPowerupLock = 0;

void _sprintf(void* destination, void* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _Printf((void*)crash_screen_copy_to_buf, destination, fmt, args);
    va_end(args);
}


u32 func_800E0790_Hook(void);

// u32 func_800E0790_Hook(void) {
//     u32 y, z;
//     y = rngSeed * 4 + 2;
//     z = y + 3;
//     y = y * z;
//     rngSeed = y / 4;
//     calls++;
//     return rngSeed;
// }

void copyCallsToPowerupCalls(void);
void printCustomText(void);
void printCustomText2(void);
void printCustomText3(void);
s32 curStringPrintingIndex = 0;

char* MenuStrings0[] = {
    // "PROCESS MENU",
    "Game",
    "Object",
    "Sprite",
    "Texture",
    "Sound",
    "Opening",
    "Ending",
    "Title",
    "PackMenu",
    "FirstPack",
    "StageClr",
    "Minigame",
    "PackTest",
    "Game Over",
    "Intro",
    "Message",
    "Config",
};

TextPosition MenuTextPositions[] = {
// {20, 40},
{20, 45},
{20, 55},
{20, 65},
{20, 75},
{20, 85},
{20, 95},
{20, 105},
{20, 115},
{20, 125},
{20, 135},
{20, 145},
{20, 155},
{20, 165},
{20, 175},
{20, 185},
{20, 195},
{20, 205},
};

char* StageList[] = {
/* 0 */ "Sky Land",
/* 1 */ "Carnival Land",
/* 2 */ "Ice Land",
/* 3 */ "Great Edo Land",
/* 4 */ "Toy Land",
/* 5 */ "Pyramid Land"
/* 6 */ "",
/* 7 */ "",
/* 8 */ "",
/* 9 */ "",
/* 10 */ "",
/* 11 */ "Training Room 1",
/* 12 */ "Training Room 2",
/* 13 */ "Training Room 3",
/* 14 */ "Training Room 4",
/* 15 */ "Billiards",
/* 16 */ "Bowling",
/* 17 */ "Chess",
/* 18 */ "",
/* 19 */ "",
/* 20 */ "",
/* 21 */ "Mushroom Boss",
/* 22 */ "Burger Boss",
/* 23 */ "Wheelrus",
/* 24 */ "Frog Boss",
/* 25 */ "Robot Boss",
/* 26 */ "Sphynx Boss",
/* 27 */ "",
/* 28 */ "",
/* 29 */ "ContPak",
/* 30 */ "Unknown",
/* 31 */ "Credits 0",
/* 32 */ "Credits 1",
/* 33 */ "Credits 2",
/* 34 */ "Credits 3",
/* 35 */ "",
/* 36 */ "",
/* 37 */ "",
/* 38 */ "",
/* 39 */ "",
/* 40 */ "",
/* 41 */ "Sky Land Section 2",
/* 42 */ "Carnival Land Section 2",
/* 43 */ "Ice Hockey",
/* 44 */ "Edo Land Section 2",
/* 45 */ "Toy Land Section 2",
/* 46 */ "Pyramid Land Section 2",
/* 47 */ "",
/* 48 */ "Carnival After Teacup",
/* 49 */ "Ice Land Section 2",
};

void printCallsUntilDecidedPowerup(void) {
    u8 buffer[40];
    TextPosition textPos = {70, 210};

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&RedOrange, textPos.xPos, textPos.yPos);
    _sprintf(buffer, "%d", callsAtPowerupDecision);
    printDebugText(buffer);
}

void printRngCallsPerFrame(void) {
    u8 buffer[40];
    TextPosition textPos = {120, 210};

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&RedOrange, textPos.xPos, textPos.yPos);
    _sprintf(buffer, "%d", calls - prevCallCount);
    printDebugText(buffer);
    prevCallCount = calls;
}

void printCurrentSpeed(void) {
    u8 buffer[40];
    TextPosition textPos = {20, 210};

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&RedOrange, textPos.xPos, textPos.yPos);

    _sprintf(buffer, "%2.2f", gPlayerActors[0].magnitude);
    printDebugText(buffer);
}

void SetTextColor2(u8 red, u8 blue, u8 green, u8 alpha) {
    textRed = red;
    textBlue = blue;
    textGreen = green;
    textOpacity = alpha;
    ifTextColor = 1;
}

extern u32 timerState;

void PrintTimer(void) {
    u8 buffer[40];
    TextPosition textPos = {180, 210};

    if (timerState == 0) {
        return;
    }

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&RedOrange, textPos.xPos, textPos.yPos);
    _sprintf(buffer, "%2.2f", gPlayerActors[0].magnitude);
    printDebugText(buffer);
}

void printCurrentRespawnZone(void) {
    u8 buffer[40];
    TextPosition textPos = {20, 196};

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&RedOrange, textPos.xPos, textPos.yPos);
    _sprintf(buffer, "Zone: %02d", respawnZone);
    printDebugText(buffer);
}



void RemoveTextColor(void) {
    ifTextColor = 0;
}

char* OffOrOnString[] = {
    "OFF",
    "ON"
};

void pageMainDisplay(s32 currPageNo, s32 currOptionNo);

void DisplayDebugMenu(void) {
    u8 buffer[100];
    int i;

    if (MenuInstancePointer == 0) {
        return;
    }

    if (MenuInstancePointer->stageIndex == 1 && prevOption != 0 && MenuInstancePointer->stageIndex != prevOption) {
        MenuInstancePointer->stageIndex = prevOption;
    }

    textStyle = 1;
    textKerning = 1;
    _bzero(buffer, sizeof(buffer));
    setDebugTextPosition(MenuTextPositions[0].xPos, MenuTextPositions[0].yPos - 15, 0x50);
    SetTextWidthAndHeight(0.6f, 0.6f);
    SetTextColor2(255, 255, 255, 255);


    if (MenuInstancePointer->cursor == 0) {
        if (currentlyPressedButtons & L_JPAD) { //dpad left
            MenuInstancePointer->stageIndex--;
        } else if (currentlyPressedButtons & R_JPAD) { //dpad right
            MenuInstancePointer->stageIndex++;
        }

        if (currentlyHeldButtons & L_JPAD || currentlyHeldButtons & R_JPAD) {
            holdDpadDirectionFrames++;
        } else {
            holdDpadDirectionFrames = 0;
        }

        if (holdDpadDirectionFrames > 10) { //if held for 10 frames, quickly scroll
            if (currentlyHeldButtons & 0x200) { //dpad left
                MenuInstancePointer->stageIndex--;
            } else if (currentlyHeldButtons & 0x100) { //dpad right
                MenuInstancePointer->stageIndex++;
            }
        }


        if (currentlyHeldButtons & L_JPAD) { //dpad left
            if (MenuInstancePointer->stageIndex == 10) { //7, 8, 9, 10 are glitchy/crash
                MenuInstancePointer->stageIndex = 6;
            } else if (MenuInstancePointer->stageIndex == 20) {
                MenuInstancePointer->stageIndex = 17;
            } else if (MenuInstancePointer->stageIndex == 40) {
                MenuInstancePointer->stageIndex = 26;
            } else if (MenuInstancePointer->stageIndex == 47) {
                MenuInstancePointer->stageIndex = 46;
            }
        } else if (currentlyHeldButtons & R_JPAD) { //dpad right
            if (MenuInstancePointer->stageIndex == 7) { //8, 9, 10 are glitchy/crash
                MenuInstancePointer->stageIndex = 11;
            } else if (MenuInstancePointer->stageIndex == 18) {
                MenuInstancePointer->stageIndex = 21;
            } else if (MenuInstancePointer->stageIndex == 27) {
                MenuInstancePointer->stageIndex = 41;
            } else if (MenuInstancePointer->stageIndex == 47) {
                MenuInstancePointer->stageIndex = 48;
            }
        }


    }

    if (MenuInstancePointer->stageIndex > 49) {
        MenuInstancePointer->stageIndex = 49;
    }

    if (MenuInstancePointer->stageIndex < 1) {
        MenuInstancePointer->stageIndex = 1;
    }

    prevOption = MenuInstancePointer->stageIndex;

    _bzero(buffer, sizeof(buffer));
    setDebugTextPosition(MenuTextPositions[0].xPos + 105, MenuTextPositions[0].yPos, 0x32);
    SetTextWidthAndHeight(0.6f, 0.6f);
    SetTextColor2(255, 255, 255, 255);

    _sprintf(buffer, "%s", StageList[MenuInstancePointer->stageIndex - 1]);
    printDebugText(buffer);
    
     

    for (i = 0; i < ARRAY_COUNT(MenuStrings0); i++) {
        _bzero(buffer, sizeof(buffer));
        setDebugTextPosition(MenuTextPositions[i].xPos, MenuTextPositions[i].yPos, 0x32);
        SetTextWidthAndHeight(0.6f, 0.6f);
        SetTextColor2(255, 255, 255, 255);
    
        if (MenuInstancePointer->cursor == i) {
            SetTextColor2(255, 255, 0, 255);
        }
        
        if (i == 0) {
            _sprintf(buffer, "%s %s: %d", MenuStrings0[i], "Stage", MenuInstancePointer->stageIndex);
        } else {
            _sprintf(buffer, "%s", MenuStrings0[i]);
        }
        printDebugText(buffer); 
    }

    
    //setting it to 0 here will prevent a dangling pointer from occurring
    MenuInstancePointer = 0; //this gets set back to a pointer by some asm code.

}

void printCurrentSeed(void) {
    u8 buffer[40];
    TextPosition textPos = {160, 210};

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&White, textPos.xPos, textPos.yPos);
    _sprintf(buffer, "Seed: 0x%08X", rngSeed);
    printDebugText(buffer);
}

void printCallsEnteringSkyland(void) {
    u8 buffer[40];
    TextPosition textPos = {152, 182};

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&White, textPos.xPos, textPos.yPos);
    _sprintf(buffer, "SLCalls: %d", callsEnteringSkyland);
    printDebugText(buffer);
}

void printCallsAfterChosenPowerup(void) {
    u8 buffer[40];
    TextPosition textPos = {155, 196};

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&White, textPos.xPos, textPos.yPos);
    _sprintf(buffer, "Calls: %d", calls);
    printDebugText(buffer);
}

void printVoidOutCalls(void) {
    u8 buffer[40];
    TextPosition textPos = {160, 210};

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&White, textPos.xPos, textPos.yPos);
    _sprintf(buffer, "Void: %d", voidOutCalls);
    printDebugText(buffer);
}

void printCurrentPowerupLock(void) {
    TextPosition textPos = {61, 210};

    SetDefaultTextParametersWithColor(&RedOrange, textPos.xPos, textPos.yPos);

    switch(curPowerupLock) {
        case RANDOM:
            printDebugText("Normal");
            break;
        case SPEED:
            printDebugText("Speed");
            break;
        case X3:
            printDebugText("x3");
            break;
    }
}

void printParasolPulledFrame(void) {
    TextPosition textPos = {20, 184};
    char buffer[32];

    if (pulledParasolTimer == 0) {
        return;
    }

    pulledParasolTimer--;

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&Green, textPos.xPos, textPos.yPos);
    _sprintf(buffer, "Frame %d", parasolPullFrame);
    printDebugText(buffer);

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&Purple, textPos.xPos, textPos.yPos - 12);

    if (parasolPullSpeed == 0.0f) {
        _sprintf(buffer, "Ang: %2.2f", parasolPullAngle);
    } else {
        _sprintf(buffer, "Ang: %2.2f, Spd: %2.2f", parasolPullAngle, parasolPullSpeed);
    }
    printDebugText(buffer);
}

void PrintPosition(void) {
    TextPosition textPos = {20, 87};
    char buffer[100];
    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&Purple, textPos.xPos, textPos.yPos);
    _sprintf(buffer, "X: %2.2f", gPlayerActors[0].pos.x);
    printDebugText(buffer);

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&Purple, textPos.xPos, textPos.yPos + 12);
    _sprintf(buffer, "Y: %2.2f", gPlayerActors[0].pos.y);
    printDebugText(buffer); 

    _bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&Purple, textPos.xPos, textPos.yPos + 24);
    _sprintf(buffer, "Z: %2.2f", gPlayerActors[0].pos.z);
    printDebugText(buffer); 
}

s32 currPageNo = 0;
s32 currOptionNo = 0;
s32 isMenuActive = 0;

void PrintMenuDisplays(void) {
    //dpad down resets lag counter
    if (!(currentlyHeldButtons & R_TRIG) && currentlyPressedButtons & D_JPAD) {
        frame_count = __osVIIntrCount / 2; //reset lag counter
    }

    if (toggles[TOGGLE_DISPLAY_SPEED]) {
        printCurrentSpeed();
    }

    if (toggles[TOGGLE_DISPLAY_POSITION]) {
        PrintPosition();
    }

    if (toggles[TOGGLE_DISPLAY_FPS]) {
        print_fps(260, 207, &Cyan);
    }

    if (toggles[TOGGLE_DISPLAY_LAG_FRAMES]) {
        print_lag_frames(200, 207, &Green);
    }

    if (toggles[TOGGLE_DISPLAY_ZONE]) {
        printCurrentRespawnZone();
    }
}

s32 digitCount = 0;
char globalTextBuffer[256] = {0};

void PrintDigitsForTest(void) {
    if (currentlyPressedButtons & D_JPAD) {
        globalTextBuffer[digitCount] = '\0';
        digitCount--;
        globalTextBuffer[digitCount] = '\0';
    } else if (currentlyPressedButtons & L_JPAD) {
        for (int i = 0; i < 8; i++) {
            globalTextBuffer[digitCount--] = '\0';
        }
        globalTextBuffer[digitCount] = '\0';
    } else if (currentlyPressedButtons & R_JPAD) {
        for (int i = 0; i < 8; i++) {
            globalTextBuffer[digitCount++] = 'A';
        }
        globalTextBuffer[digitCount] = '\0';
    } else if (currentlyPressedButtons & U_JPAD) {
        globalTextBuffer[digitCount++] = 'A';
        globalTextBuffer[digitCount] = '\0';
    }

    if (digitCount < 0) {
        digitCount = 0;
    }

    u8 buffer[256];
    TextPosition textPos = {20, 160};

    //_bzero(buffer, sizeof(buffer));
    SetDefaultTextParametersWithColor(&RedOrange, textPos.xPos, textPos.yPos);
    _sprintf(buffer, "%s", globalTextBuffer);
    printDebugText(buffer);

}

void printCustomTextInC(void) {

    // PrintDigitsForTest();

    // afterBossHpSet(); //activate timer
    //MainTimerBoss(); //display timer
    MainTimerLevel();
    // PrintMenuDisplays();

    // if (currPageNo == 2 && isMenuActive == 1) {
    //     u8 buffer[8];
    //     TextPosition textPos = {130, 45};

    //     _bzero(buffer, sizeof(buffer));
    //     SetDefaultTextParametersWithColor(&Green, textPos.xPos, textPos.yPos);

    //     _sprintf(buffer, "%d", menuCurRespawnZone);
    //     printDebugText(buffer);        
    // }

    // if (isMenuActive == 1) {
    //     updateMenuInput();
    //     pageMainDisplay(currPageNo, currOptionNo);
    // }

    // if (callsEnteringSkyland == 0) {
    //     callsEnteringSkyland = calls;
    // }

    // switch (boolPrintCustomText) {
    //     case 1:
    //         printCurrentRespawnZone();
    //         printCurrentSpeed();
    //         printCurrentPowerupLock();
    //         break;
    //     case 2:
    //         PrintPosition();
    //         break;
    //     case 3:
    //         printParasolPulledFrame();
    //         printCurrentSpeed();
    //         PrintPosition();
    //         break;
    //     case 4:
    //         printParasolPulledFrame();
    //         PrintPosition();
    //         printCurrentRespawnZone();
    //         printCurrentSpeed();
    //         printCurrentPowerupLock();
    //         break;
    // }

    // printCallsUntilDecidedPowerup();
    // printRngCallsPerFrame();
    //printParasolPulledFrame();
    // printCurrentPowerupLock();
    // printCurrentSpeed();
    // //printCurrentSeed();
    // //printCallsAfterChosenPowerup();
    // //printCallsEnteringSkyland();
    // //printVoidOutCalls();
    // printCurrentRespawnZone();
    // PrintPosition();
}

void hookAt800DE480(void);
void hookAt800D4240(void);
void osPiStartDmaHook(void);
void osEPiStartDmaHook(void);
void func_80035E00_Hook(unkLookatStruct* arg0);
void hookAt800D5C54(void);

void func_800293F0(s32);
void func_800293F0_Hook(s32);
void func_800293F0_Hook2(void);

typedef struct Temp {
    char unk_00[0x14];
} Temp; //unk size

void func_8003C9DC(void);
s32 func_800D4DE0(s32);
void func_800D6160(void);
void func_800E8C00(void*, s32, s32);
extern s16 D_800EAD5C;
extern s16 D_800EAF60;
extern s16 D_800EAF64;
extern s16 D_800FF20E;
extern Temp D_80160808;

//load debug menu and set gamemode
// void func_800293F0_Hook(s32 arg0) {
//     D_800EAF60 = arg0;
//     D_800EAF64 = 0;
//     func_800E8C00(&D_80160808, 0, 0x10);
//     func_8003C9DC();
//     if (func_800D4DE0(0) == 0) {
//         func_800D6160();
//     }
//     D_800EAD5C = 0;
//     D_800FF20E = 0;

//     if (stateCooldown == 0) {
//         checkInputsForSavestates();
//     }
// }

void func_800293F0_Hook2(void) {
    if (stateCooldown != 0) {
        return;
    }

    if (gGameMode == 0xC) { //is debug menu
        return;
    }

    if (isMenuActive == 1) {
        return;
    }

    if (currentlyPressedButtons & L_JPAD) {
        isSaveOrLoadActive = SAVE_STATE;
        osCreateThread(&gCustomThread.thread, 255, (void*)savestateMain, NULL,
                gCustomThread.stack + sizeof(gCustomThread.stack), 255);
        osStartThread(&gCustomThread.thread);
        stateCooldown = 5;
    } else if (currentlyPressedButtons & R_JPAD) {
        // D_800EAF60 = savestateGameMode; //set game mode
        // D_800EAF64 = 0;
        // func_800E8C00(&D_80160808, 0, 0x10);
        // func_8003C9DC();
        // // if (func_800D4DE0(0) == 0) {
        // //     func_800D6160();
        // // }
        // D_800EAD5C = 0;
        // D_800FF20E = 0;

        isSaveOrLoadActive = LOAD_STATE;
        osCreateThread(&gCustomThread.thread, 255, (void*)loadstateMain, NULL,
                gCustomThread.stack + sizeof(gCustomThread.stack), 255);
        osStartThread(&gCustomThread.thread);
        currentlyPressedButtons = 0;
        stateCooldown = 5;
        while (isSaveOrLoadActive != INACTIVE_STATE) {}
    }
}

void hookAt80029574(void);
void hookAt80029758(void);
void func_800BE480(void);
void printCustomText4(void);

// ------------- FPS COUNTER ---------------
// To use it, call print_fps(x,y); every frame.
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
    printDebugText(buffer);
}

void print_fps(s32 x, s32 y, TextColor* color) {
    f32 fps = calculate_and_update_fps();
    char buffer[16];

    _bzero(buffer, sizeof(buffer));
    _sprintf(buffer, "%2.2f", fps);
    SetDefaultTextParametersWithColor(color, x, y);
    printDebugText(buffer);
}

void hookAt800CBDC0(void);

void cBootFunction(void) { //ran once on boot
    crash_screen_init();
    stateCooldown = 0;
    savestateCurrentSlot = 0;
    lag_frames = 0;
    //savestate1Size = 0;
    isSaveOrLoadActive = INACTIVE_STATE;
    hookCode((void*)0x800E0790, &func_800E0790_Hook); //rng hook to track call total
    hookCode((void*)0x8004FBD0, &copyCallsToPowerupCalls); //when powerup is grabbed, copy calls to another var

    hookCode((void*)0x8002858C, &printCustomText2); //print our custom text when in game
    //hookCode((void*)0x800507C0, &printCustomText3);

    hookCode((void*)0x80029574, &hookAt80029574);
    
    hookCode((void*)0x8004E3DC, &recordCallsAtVoidOut);

    //hookCode((void*)0x80035E00, &func_80035E00_Hook);
    
    hookCode((void*)0x800D5C54, &hookAt800D5C54);

    //time to randomly guess at fixing savestates wheeeeee
    //hookCode((void*)0x800DE480, &hookAt800DE480);//(adds stability?)
    hookCode((void*)0x800D4240, &hookAt800D4240); //(probably keep this one on, increases stability?)

    hookCode((void*)0x800DF584, &osPiStartDmaHook);
    hookCode((void*)0x800E1590, &osEPiStartDmaHook);

    

    calls = 0;
    callsAtPowerupDecision = 0;
    seedAtPowerup = 0;
    callsEnteringSkyland = 0;
    voidOutCalls = 0;

    callsState = 0;
    callsAtPowerupDecisionState = 0;
    seedAtPowerupState = 0;
    callsEnteringSkylandState = 0;
    voidOutCallsState = 0;
}


// enum PowerUpIDs {
//     POWER_BIG = 0,
//     POWER_SPEED_UP = 1,
//     POWER_SPEED_DOWN = 2,
//     POWER_X2 = 3,
//     POWER_X3 = 4,
//     POWER_INVICIBLE = 5,
//     POWER_SHORT_TONGUE = 6,
//     POWER_NOTHING = 7
// };

f32 alwaysBigPowerup[] = {
    1.0f, 0, 0, 0,
    0, 0, 0, 0
};

f32 alwaysSpeedUpPowerUp[] = {
    0, 1.0f, 0, 0,
    0, 0, 0, 0
};

f32 alwaysSpeedDownPowerUp[] = {
    0, 0, 1.0f, 0,
    0, 0, 0, 0
};

f32 always2XPowerUp[] = {
    0, 0, 0, 1.0f,
    0, 0, 0, 0
};

f32 always3XPowerUp[] = {
    0, 0, 0, 0,
    1.0f, 0, 0, 0
};

f32 alwaysInvinciblePowerUp[] = {
    0, 0, 0, 0,
    0, 1.0f, 0, 0
};

f32 alwaysShortTonguePowerUp[] = {
    0, 0, 0, 0,
    0, 0, 1.0f, 0
};

f32 alwaysNothingPowerUp[] = {
    0, 0, 0, 0,
    0, 0, 0, 1.0f
};

f32 randomPowerUps[] = {
    0.125f, 0.25f, 0.375f, 0.5f,
    0.625f, 0.75f, 0.875f, 1.0f
};

// s32 image[] = {
// 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x78009001, 0x90436000, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x78009801, 0xA0019043, 0x88017843, 0x70016800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x7800A843, 0xB843B043, 0xA043A001, 0x88438801, 0x70017843, 0x70016042, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x7000A843, 0xD043C843, 0xB843C043, 0xA843A843, 0x98019043, 0x80017801, 0x70017001, 0x70015800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58008800, 0xD043D043, 0xD843D043, 0xC043C843, 0xB843B843, 0xA8439801, 0x90438001, 0x80437001, 0x70017001, 0x68005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0xA043E043, 0xE043D843, 0xD843D843, 0xD843D043, 0xD043C843, 0xB843B043, 0x98019801, 0x80018043, 0x70017001, 0x70016843, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x5800B043, 0xF043E843, 0xE843E843, 0xE043E843, 0xD843E043, 0xE043D843, 0xC843C843, 0xB843A843, 0x98018843, 0x78017843, 0x70017001, 0x68015800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0xA001F843, 0xF843F043, 0xF043F043, 0xE843E843, 0xE843E843, 0xE843E843, 0xE043D043, 0xD043C043, 0xA843A001, 0x88438001, 0x70018043, 0x70016001, 0x60005800, 0x58005800, 0x58005800, 0x58008042, 0xF885F885, 0xF843F843, 0xF843F843, 0xF843F843, 0xF843F843, 0xF843F043, 0xF043E843, 0xE043D043, 0xC843B843, 0x98438801, 0x78437001, 0x70018043, 0x58005800, 0x58005800, 0x58005800, 0x5800E109, 0xF909F8C7, 0xF8C7F885, 0xF885F843, 0xF843F843, 0xF843F843, 0xF885F843, 0xF843F043, 0xF043E843, 0xD843C043, 0xB8439801, 0x90437801, 0x78017001, 0x70015800, 0x58005800, 0x58005800, 0x8884F98D, 0xF94BF94B, 0xF909F8C7, 0xF8C7F885, 0xF843F843, 0xF885F843, 0xF843F885, 0xF843F885, 0xF843F043, 0xE843E043, 0xC843A843, 0x98018843, 0x78017001, 0x80435800, 0x58005800, 0x58005800, 0xC14BFA11, 0xFA11F98D, 0xF98DF94B, 0xF909F885, 0xF885F843, 0xF843F843, 0xF843F843, 0xF843F843, 0xF843F843, 0xF043E843, 0xD043C043, 0xB0439001, 0x88017843, 0x70017001, 0x58005800, 0x58005800, 0xF253FA53, 0xFA53FA53, 0xF9CFF98D, 0xF94BF8C7, 0xF885F843, 0xF885F843, 0xF885F843, 0xF885F885, 0xF843F843, 0xF843F043, 0xE043D043, 0xB043A843, 0x90438801, 0x70017001, 0x58005800, 0x58005800, 0xFB19FB19, 0xFA95FA95, 0xFA53F9CF, 0xF94BF8C7, 0xF8C7F885, 0xF885F885, 0xF885F8C7, 0xF885F885, 0xF8C7F885, 0xF843F843, 0xE843D843, 0xC843A843, 0xA0018843, 0x88017843, 0x58005800, 0x58006842, 0xFB9DFB9D, 0xFB5BFB5B, 0xFA95FA53, 0xF9CFF98D, 0xF8C7F8C7, 0xF8C7F8C7, 0xF8C7F909, 0xF909F909, 0xF909F8C7, 0xF8C7F885, 0xF843E843, 0xD043C043, 0xA843A001, 0x88438801, 0x58005800, 0x58005800, 0xFBDFFBDF, 0xFC63FC63, 0xFB9DFAD7, 0xFA53F9CF, 0xF94BF94B, 0xF94BF94B, 0xF94BF94B, 0xF98DF98D, 0xF98DF98D, 0xF94BF94B, 0xF885F043, 0xE043D043, 0xB043B043, 0x98018801, 0x58005800, 0x58005800, 0xF3DFFD29, 0xFDEFFDEF, 0xFCE7FBDF, 0xFAD7FA11, 0xF9CFF98D, 0xF98DF98D, 0xF9CFF9CF, 0xFA11FA11, 0xFA11FA11, 0xF9CFF9CF, 0xF94BF885, 0xF043D843, 0xC843B043, 0xB0439843, 0x58005800, 0x58005800, 0xCB19FDEF, 0xFFBDFFBD, 0xFE31FC63, 0xFB19FAD7, 0xFA11FA11, 0xF9CFFA53, 0xFA11FA11, 0xFAD7FA95, 0xFB19FB19, 0xFAD7FA53, 0xF9CFF909, 0xF843E843, 0xD843C843, 0xB0439001, 0x58005800, 0x58005800, 0x99CEFDEF, 0xFFBDFFBD, 0xFE73FCA5, 0xFBDFFAD7, 0xFA53FA53, 0xFA53FA11, 0xFA95FA95, 0xFAD7FB19, 0xFC21FC63, 0xFC21FB19, 0xFA53F94B, 0xF885F043, 0xE043D043, 0xC8436800, 0x58005800, 0x58005800, 0x6884D463, 0xFDEFFE73, 0xFD6BFC63, 0xFB9DFB5B, 0xFA95FA53, 0xFA53FAD7, 0xFA95FB19, 0xFB19FB9D, 0xFCE7FD6B, 0xFD29FB9D, 0xFAD7F9CF, 0xF8C7F843, 0xF043D843, 0xB0435800, 0x58005800, 0x58005800, 0x580078C6, 0xF4A5FD29, 0xFCE7FC21, 0xFBDFFB19, 0xFAD7FA95, 0xFA95FA95, 0xF29588C6, 0x894AF35B, 0xFCE7FE31, 0xFDEFFB9D, 0xFB19FA11, 0xF94BF8C7, 0xF843D843, 0x60006000, 0x58005800, 0x58005800, 0x58005800, 0x6884CB5B, 0xFCA5FC21, 0xFBDFFB5B, 0xFAD7FAD7, 0xFA95B14B, 0x60426000, 0x58007084, 0xBA95FC63, 0xFC63FB9D, 0xFAD7FA53, 0xF9CFF909, 0xD0436800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x8108B211, 0xCA95CA11, 0xA14B88C6, 0x58005800, 0x58005800, 0x58005800, 0x684270C6, 0x914AA18D, 0xCA53C98D, 0xB1098042, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800
// };

// s32 pal[] = {
//     0x294AFD29, 0xF9C1C9C1, 0xB8C10001, 0xB8016801
// };

void func_8002616C(void);

/* Define vertex data for the triangle */
const s32 baseX = 50;
Vtx vertexData[] = {
    { baseX + 0, 0, 0, 0, GPACK_RGBA5551(255, 0, 0, 1), 0, 0 },
    { baseX + 50, 0, 0, 0, GPACK_RGBA5551(0, 255, 0, 1), 0, 0 },
    { baseX + 25, 50, 0, 0, GPACK_RGBA5551(0, 0, 255, 1), 0, 0 }
};

// Gfx displayList[] = {
//     gsDPPipeSync(),
//     gsDPSetCycleType(G_CYC_1CYCLE),
//     gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
//     gsSPClearGeometryMode(G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_SHADING_SMOOTH),
//     gsSPSetGeometryMode(G_ZBUFFER | G_CULL_BACK),
//     gsSPVertex(vertexData, 3, 0),
//     gsDPSetPrimColor(0, 0, 255, 255, 255, 255),
//     gsSP1Triangle(0, 1, 2, 0),
//     gsSPEndDisplayList()
// };

Mtx modelview = {0};

extern Mtx* MatrixBuffer;

typedef f32 Matrix4f[4][4]; // size = 0x40

// void DLWriteHook(void) {
//     gSPDisplayList(gMainGfxPosPtr++, Entity_YellowBlock_Render);
// }

void tickAirborneFrames(void) {
    if (p1Airborne == 1) {
        p1AirborneFrames++;
    } else {
        p1AirborneFrames = 0;
    }
}

void ifPullParasolPrint(void) {
    if (p1IfParasol == 1) {
        if (parasolPulled == 0) {
            pulledParasolTimer = 90;
            parasolPullFrame = p1AirborneFrames;
            parasolPullAngle = gPlayerActors[0].angle1;
            parasolPulled = 1;
        }

        if (gPlayerActors[0].magnitude != 0.0f && parasolPullSpeed == 0.0f) {
            parasolPullSpeed = gPlayerActors[0].magnitude;
        }
    } else {
        parasolPulled = 0;
        parasolPullSpeed = 0.0f;
    }
}

extern u8 UnlockedCostumes;

void LimitZone(void) {
    switch (gCurArea) {
        case 1:
            if (menuCurRespawnZone >= ARRAY_COUNT(SkyLandArea0) - 1) {
                menuCurRespawnZone = ARRAY_COUNT(SkyLandArea0) - 1;
            }
            break;
        case 0x29:
            if (menuCurRespawnZone >= ARRAY_COUNT(SkyLandArea1) - 1) {
                menuCurRespawnZone = ARRAY_COUNT(SkyLandArea1) - 1;
            }
            break;
        case 2:
            if (menuCurRespawnZone >= ARRAY_COUNT(CarnivalLandArea0) - 1) {
                menuCurRespawnZone = ARRAY_COUNT(CarnivalLandArea0) - 1;
            }
            break;
        case 0x30:
            if (menuCurRespawnZone >= ARRAY_COUNT(CarnivalLandArea1) - 1) {
                menuCurRespawnZone = ARRAY_COUNT(CarnivalLandArea1) - 1;
            }
            break;
        case 0x2A:
            if (menuCurRespawnZone >= ARRAY_COUNT(CarnivalLandArea2)) {
                menuCurRespawnZone = ARRAY_COUNT(CarnivalLandArea2) - 1;
            }
            break;
        case 3:
            if (menuCurRespawnZone >= ARRAY_COUNT(IceLandArea0) - 1) {
                menuCurRespawnZone = ARRAY_COUNT(IceLandArea0) - 1;
            }
            break;
        case 0x2B:
            if (menuCurRespawnZone >= ARRAY_COUNT(IceLandArea1) - 1) {
                menuCurRespawnZone = ARRAY_COUNT(IceLandArea1) - 1;
            }
            break;
        case 0x31:
            if (menuCurRespawnZone >= ARRAY_COUNT(IceLandArea2) - 1) {
                menuCurRespawnZone = ARRAY_COUNT(IceLandArea2) - 1;
            }
            break;
        case 4:
            if (menuCurRespawnZone >= ARRAY_COUNT(EdoLandArea0) - 1) {
                menuCurRespawnZone = ARRAY_COUNT(EdoLandArea0) - 1;
            }
            break;
        case 0x2C:
            if (menuCurRespawnZone >= ARRAY_COUNT(EdoLandArea1) - 1) {
                menuCurRespawnZone = ARRAY_COUNT(EdoLandArea1) - 1;
            }
            break;
        case 5:
            if (menuCurRespawnZone >= ARRAY_COUNT(ToyLandArea0) - 1) {
                menuCurRespawnZone = ARRAY_COUNT(ToyLandArea0) - 1;
            }
            break;
        case 0x2D:
            if (menuCurRespawnZone >= ARRAY_COUNT(ToyLandArea1) - 1) {
                menuCurRespawnZone = ARRAY_COUNT(ToyLandArea1) - 1;
            }
            break;
        case 6:
            if (menuCurRespawnZone >= ARRAY_COUNT(PyramidLandArea0) - 1) {
                menuCurRespawnZone = ARRAY_COUNT(PyramidLandArea0) - 1;
            }
            break;
        case 0x2E:
            if (menuCurRespawnZone >= ARRAY_COUNT(PyramidLandArea1) - 1) {
                menuCurRespawnZone = ARRAY_COUNT(PyramidLandArea1) - 1;
            }
            break;
    }
}

void perFrameCFunction(void) {
    cCompareVICount(); //used for calculating lag frames
    
    debugFlag = 1;
    D_80160648 = 0x3F; //unlock all levels

    tickAirborneFrames();
    ifPullParasolPrint();

    UnlockedCostumes = 0xFE;

    if (currPageNo == 2) { //is zone page
        if (currentlyHeldButtons & R_TRIG && currentlyPressedButtons & R_JPAD) {
            menuCurRespawnZone++;
        } else if (currentlyHeldButtons & R_TRIG && currentlyPressedButtons & L_JPAD) {
            menuCurRespawnZone--;
            if (menuCurRespawnZone == 0) {
                menuCurRespawnZone = 1;
            }
        } else if (currentlyHeldButtons & L_TRIG && currentlyHeldButtons & R_JPAD) {
            menuCurRespawnZone++;
        } else if (currentlyHeldButtons & L_TRIG && currentlyHeldButtons & L_JPAD) {
            menuCurRespawnZone--;
            if (menuCurRespawnZone == 0) {
                menuCurRespawnZone = 1;
            }
        }
        LimitZone();
    }

    if (toggles[TOGGLE_INF_HEALTH]) {
        playerHealth = 10;
    }
    
    if (currentlyHeldButtons & L_TRIG && isMenuActive == 0) {
        D_80187B20 = 32.0f;
    }

    //R + dpad down toggles menu
    if (currentlyHeldButtons & R_TRIG && currentlyPressedButtons & CONT_DOWN) {
        isMenuActive ^= 1;
    }

    switch (toggles[TOGGLE_POWERUP_LOCK]) {
        case POWER_BIG:
            memcpy(powerUpFloatArray, alwaysBigPowerup, sizeof(powerUpFloatArray));
            break;
        case POWER_SPEED_UP:
            memcpy(powerUpFloatArray, alwaysSpeedUpPowerUp, sizeof(powerUpFloatArray));
            break;
        case POWER_SPEED_DOWN:
            memcpy(powerUpFloatArray, alwaysSpeedDownPowerUp, sizeof(powerUpFloatArray));
            break;
        case POWER_X2:
            memcpy(powerUpFloatArray, always2XPowerUp, sizeof(powerUpFloatArray));
            break;
        case POWER_X3:
            memcpy(powerUpFloatArray, always3XPowerUp, sizeof(powerUpFloatArray));
            break;
        case POWER_INVICIBLE:
            memcpy(powerUpFloatArray, alwaysInvinciblePowerUp, sizeof(powerUpFloatArray));
            break;
        case POWER_SHORT_TONGUE:
            memcpy(powerUpFloatArray, alwaysShortTonguePowerUp, sizeof(powerUpFloatArray));
            break;
        case POWER_NOTHING:
            memcpy(powerUpFloatArray, alwaysNothingPowerUp, sizeof(powerUpFloatArray));
            break;
        default:
            memcpy(powerUpFloatArray, randomPowerUps, sizeof(powerUpFloatArray));
            break;
    }

    if (stateCooldown > 0) {
        stateCooldown--;
    }

    while (isSaveOrLoadActive != INACTIVE_STATE) {}
}

// extern f32 D_800F57BC;
// extern Gfx* gMainGfxPosPtr;
// extern Mtx* MatrixBuffer;

// void func_80035E00_Hook(unkLookatStruct* arg0) { //renders the world
//     u16 sp56;
//     f32 var_f0;

//     var_f0 = 50.0f;
//     if ((gGameMode == 0xF) || (gGameMode == 0x10)) {
//         var_f0 = 40.0f;
//     }
//     guPerspective(MatrixBuffer, &sp56, 60.0f, 1.3333334f, var_f0, D_800F57BC, 1.0f);
//     gSPPerspNormalize(gMainGfxPosPtr++, sp56);
//     gSPMatrix(gMainGfxPosPtr++, MatrixBuffer++, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
//     guLookAt(MatrixBuffer, arg0->unk48, arg0->unk4C, arg0->unk50, arg0->unk54, arg0->unk58, arg0->unk5C, 0.0f, 1.0f, 0.0f);
//     gSPMatrix(gMainGfxPosPtr++, MatrixBuffer++, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

//     //added code
//     //gSPDisplayList(gMainGfxPosPtr++, Entity_YellowBlock_Render);
// }

s32 initOverworldTimerC(void) {
    afterBossHpSet();
    return func_800286E8(0x80160634); //hack, this pointer gets passed into the func
}