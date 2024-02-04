#include "../include/ct2.h"
#include <stdarg.h>

//80030AA0 texture entities

//func_800D6050 prints debug stuff
//800FF20C removes player control?
//func_8003573C renders world

//80035C68 chaos idea, delete function call here

s32 holdDpadDirectionFrames = 0;
s32 prevOption = 0;
s32 infiniteHealthBool = 0;

extern Gfx* gMainGfxPosPtr;

typedef struct MenuInstance {
    s16 cursor;
    s16 stageIndex;
    s16 unk_04;
    s16 unk_timer;
    u16 inputs;
} MenuInstance;

MenuInstance* MenuInstancePointer = 0;

extern s16 unkStep;
extern s16 debugFlag;

s32 stateCooldown = 0; //does this work??
s32 savestateCurrentSlot = 0;
extern s32 savestate1Size;
s32 savestate2Size = 0;
s32 savestate3Size = 0;
s32 boolPrintCustomText = 1;
volatile s32 isSaveOrLoadActive = 0;
s32 saveOrLoadStateMode = 0;

#define RANDOM 0
#define SPEED 1
#define X3 2

extern f32 powerUpFloatArray[8];

extern u16 currentlyPressedButtons;
extern u16 currentlyHeldButtons;
#define SAVE_MODE 0
#define LOAD_MODE 1

#define ramStartAddr 0x800E87D0 //
#define ramEndAddr 0x80400000 //can probably skip the frame buffer at around 0x80360000 - 0x80400000 approximately

extern CustomThread gCustomThread;

void SetTextColor(u8 red, u8 blue, u8 green, u8 alpha);
void RemoveTextColor(void);

void SetTextWidthAndHeight(f32 width, f32 height) {
    textWidth = width;
    textHeight = height;
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


void loadstateMain(void) {
    u32 mask;
    u32 savestateHeader = (*(u32*)ramAddrSavestateDataSlot1); //not really a header, but savestates always start with 0x4620610944222000
    mask = __osDisableInt();
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
    
    //instead of checking savestate size, check savestate header to see if it's valid
    //this will allow the user to load a savestate after crashing
    
    if (savestateHeader == 0x03E00008) {
        customMemCpy((void*)ramStartAddr, ramAddrSavestateDataSlot1, (u32)ramEndAddr - (u32)ramStartAddr);
    }

    osWritebackDCacheAll();
    __osRestoreInt(mask);
    isSaveOrLoadActive = 0; //allow thread to continue
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

    customMemCpy((void*)ramAddrSavestateDataSlot1, (void*)ramStartAddr, ramEndAddr - ramStartAddr);
    //savestate1Size = ramEndAddr - ramStartAddr;

    __osRestoreInt(mask);
    isSaveOrLoadActive = 0; //allow thread to continue
}

void checkInputsForSavestates(void) {
    savestateCurrentSlot = 0;//set to 0

    if (unkStep == 3 || unkStep == 2 || unkStep == 0x12) { //in overworld/bootup/titlescreen
        if (currentlyPressedButtons & L_JPAD) {
            isSaveOrLoadActive = 1;
            osCreateThread(&gCustomThread.thread, 255, (void*)savestateMain, NULL,
                    gCustomThread.stack + sizeof(gCustomThread.stack), 255);
            osStartThread(&gCustomThread.thread);
            stateCooldown = 5;
        } else if (currentlyPressedButtons & R_JPAD) {
            isSaveOrLoadActive = 1;
            osCreateThread(&gCustomThread.thread, 255, (void*)loadstateMain, NULL,
                    gCustomThread.stack + sizeof(gCustomThread.stack), 255);
            osStartThread(&gCustomThread.thread);
            currentlyPressedButtons = 0;
            stateCooldown = 5; 
        }
    }
}

extern u8 D_80160648;
extern f32 D_80187B20; //is 0x20 into gPlayerActors[0]
extern s16 playerHealth;
extern u8 D_800ECFD0[]; //array of characters for integers
extern s32 D_800F6410; //?
extern f32 D_800F6498;
extern s32 D_80160808;
void* crash_screen_copy_to_buf(void* dest, const char* src, u32 size);
void setDebugTextPosition(s32 xPos, s32 yPos, s32 unk);
void formatText(void* buffer, void* string);
void printDebugText(void* string);
extern s16 textStyle;
extern u32 rngSeed;
void recordCallsAtVoidOut(void);
s32 curPowerupLock = 0;

void _Printf(void* destination, void* fmt, ...);

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

typedef struct TextPosition {
    s32 xPos;
    s32 yPos;
} TextPosition;

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
    TextPosition textPos = {61, 196};

    _bzero(buffer, sizeof(buffer));
    textStyle = 1;
    setDebugTextPosition(textPos.xPos, textPos.yPos, 0x32);
    SetTextWidthAndHeight(0.6f, 0.6f);
    SetTextColor(255, 255, 255, 255);
    _sprintf(buffer, "%d", callsAtPowerupDecision);
    printDebugText(buffer);
}

void printCurrentSpeed(void) {
    u8 buffer[40];
    TextPosition textPos = {20, 210};

    _bzero(buffer, sizeof(buffer));
    textStyle = 1;
    setDebugTextPosition(textPos.xPos, textPos.yPos, 0x32);
    SetTextWidthAndHeight(0.6f, 0.6f);
    RemoveTextColor();
    _sprintf(buffer, "%2.2f", gPlayerActor->speed);
    printDebugText(buffer);
}

void printCurrentRespawnZone(void) {
    u8 buffer[40];
    TextPosition textPos = {20, 196};

    _bzero(buffer, sizeof(buffer));
    textStyle = 1;
    setDebugTextPosition(textPos.xPos, textPos.yPos, 0x32);
    SetTextWidthAndHeight(0.6f, 0.6f);
    SetTextColor(255, 255, 255, 255);
    _sprintf(buffer, "Zone: %02d", respawnZone);
    printDebugText(buffer);
}

void SetTextColor(u8 red, u8 blue, u8 green, u8 alpha) {
    textRed = red;
    textBlue = blue;
    textGreen = green;
    textOpacity = alpha;
    ifTextColor = 1;
}

void RemoveTextColor(void) {
    ifTextColor = 0;
}

char* OffOrOnString[] = {
    "OFF",
    "ON"
};

void DisplayDebugMenu(void) {
    u8 buffer[100];
    int i;

    if (MenuInstancePointer == 0) {
        return;
    }

    if (MenuInstancePointer->stageIndex == 1 && prevOption != 0 && MenuInstancePointer->stageIndex != prevOption) {
        MenuInstancePointer->stageIndex = prevOption;
    }

    if ((currentlyHeldButtons & R_TRIG) && (currentlyPressedButtons & R_JPAD)) {
        infiniteHealthBool ^=1;
    }

    textStyle = 1;
    textKerning = 1;

    _bzero(buffer, sizeof(buffer));
    setDebugTextPosition(MenuTextPositions[0].xPos, MenuTextPositions[0].yPos - 15, 0x50);
    SetTextWidthAndHeight(0.6f, 0.6f);
    SetTextColor(255, 255, 255, 255);

    _sprintf(buffer, "Inf Hp: %s", OffOrOnString[infiniteHealthBool]);
    printDebugText(buffer);


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
    SetTextColor(255, 255, 255, 255);

    _sprintf(buffer, "%s", StageList[MenuInstancePointer->stageIndex - 1]);
    printDebugText(buffer);
    
     

    for (i = 0; i < ARRAY_COUNT(MenuStrings0); i++) {
        _bzero(buffer, sizeof(buffer));
        setDebugTextPosition(MenuTextPositions[i].xPos, MenuTextPositions[i].yPos, 0x32);
        SetTextWidthAndHeight(0.6f, 0.6f);
        SetTextColor(255, 255, 255, 255);
    
        if (MenuInstancePointer->cursor == i) {
            SetTextColor(255, 255, 0, 255);
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
    textStyle = 1;
    textKerning = 1;
    setDebugTextPosition(textPos.xPos, textPos.yPos, 0x32);
    SetTextWidthAndHeight(0.6f, 0.6f);
    SetTextColor(255, 255, 255, 255);
    _sprintf(buffer, "Seed: 0x%08X", rngSeed);
    printDebugText(buffer);
}

void printCallsEnteringSkyland(void) {
    u8 buffer[40];
    TextPosition textPos = {152, 182};

    _bzero(buffer, sizeof(buffer));
    textStyle = 1;
    textKerning = 1;
    setDebugTextPosition(textPos.xPos, textPos.yPos, 0x32);
    SetTextWidthAndHeight(0.6f, 0.6f);
    SetTextColor(255, 255, 255, 255);
    _sprintf(buffer, "SLCalls: %d", callsEnteringSkyland);
    printDebugText(buffer);
}

void printCallsAfterChosenPowerup(void) {
    u8 buffer[40];
    TextPosition textPos = {155, 196};

    _bzero(buffer, sizeof(buffer));
    textStyle = 1;
    textKerning = 1;
    setDebugTextPosition(textPos.xPos, textPos.yPos, 0x32);
    SetTextWidthAndHeight(0.6f, 0.6f);
    SetTextColor(255, 255, 255, 255);
    _sprintf(buffer, "Calls: %d", calls);
    printDebugText(buffer);
}

void printVoidOutCalls(void) {
    u8 buffer[40];
    TextPosition textPos = {160, 210};

    _bzero(buffer, sizeof(buffer));
    textStyle = 1;
    textKerning = 1;
    setDebugTextPosition(textPos.xPos, textPos.yPos, 0x32);
    SetTextWidthAndHeight(0.6f, 0.6f);
    SetTextColor(255, 255, 255, 255);
    _sprintf(buffer, "Void: %d", voidOutCalls);
    printDebugText(buffer);
}

void printCurrentPowerupLock(void) {
    TextPosition textPos = {61, 210};

    textStyle = 1;
    textKerning = 1;
    SetTextWidthAndHeight(0.6f, 0.6f);
    RemoveTextColor();
    setDebugTextPosition(textPos.xPos, textPos.yPos, 0x32);
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

void printCustomTextInC(void) {
    if (boolPrintCustomText == 0) {
        return;
    }
    if (callsEnteringSkyland == 0) {
        callsEnteringSkyland = calls;
    }

    //otherwise, print stuff
    //printCallsUntilDecidedPowerup();
    printCurrentPowerupLock();
    printCurrentSpeed();
    //printCurrentSeed();
    //printCallsAfterChosenPowerup();
    //printCallsEnteringSkyland();
    //printVoidOutCalls();
    printCurrentRespawnZone();
}

void hookAt80026750(void);
void hookAt800D5B64(void);
void hookAt800DE480(void);
void hookAt800E7F98(void);
void hookAt800D58E8(void);
void hookAt800D5CE8(void);
void hookAt800D6C04(void);
void hookAt800D4240(void);
void hookAt800D3FBC(void);
void hookAt800CBDC0(void);
void hookAt8002AB64(void);
void hookAt800D6C10(void);
void osPiStartDmaHook(void);
void osEPiStartDmaHook(void);
void cBootFunction(void) { //ran once on boot
    crash_screen_init();
    stateCooldown = 0;
    savestateCurrentSlot = 0;
    //savestate1Size = 0;
    isSaveOrLoadActive = 0;
    hookCode((void*)0x800E0790, &func_800E0790_Hook); //rng hook to track call total
    hookCode((void*)0x8004FBD0, &copyCallsToPowerupCalls); //when powerup is grabbed, copy calls to another var
    hookCode((void*)0x800B3EA8, &printCustomText); //print our custom text when in game
    hookCode((void*)0x8004E3DC, &recordCallsAtVoidOut);

    //time to randomly guess at fixing savestates wheeeeee
    //hookCode((void*)0x80026750, &hookAt80026750);//
    //hookCode((void*)0x800D5B64, &hookAt800D5B64);//
    //hookCode((void*)0x800DE480, &hookAt800DE480);//(adds stability?)
    //hookCode((void*)0x800E7F98, &hookAt800E7F98);//
    //hookCode((void*)0x800D58E8, &hookAt800D58E8);//
    //hookCode((void*)0x800D5CE8, &hookAt800D5CE8);//
    //hookCode((void*)0x800D6C04, &hookAt800D6C04);//
    hookCode((void*)0x800D4240, &hookAt800D4240); //(probably keep this one on, increases stability?)
    //hookCode((void*)0x800D3FBC, &hookAt800D3FBC); //(bad hook)
    //hookCode((void*)0x800CBDC0, &hookAt800CBDC0);//
    //hookCode((void*)0x8002AB64, &hookAt8002AB64);

    //more attempts, fun stuff
    //hookCode((void*)0x800D6C10, &hookAt800D6C10);

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


f32 randomPowerUps[] = {
    0.125f, 0.25f, 0.375f, 0.5f,
    0.625f, 0.75f, 0.875f, 1.0f
};

f32 alwaysSpeedPowerUps[] = {
    0, 1.0f, 0, 0,
    0, 0, 0, 0
};

f32 always3XPowerUps[] = {
    0, 0, 0, 0,
    1.0f, 0, 0, 0
};

s32 image[] = {
0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x78009001, 0x90436000, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x78009801, 0xA0019043, 0x88017843, 0x70016800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x7800A843, 0xB843B043, 0xA043A001, 0x88438801, 0x70017843, 0x70016042, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x7000A843, 0xD043C843, 0xB843C043, 0xA843A843, 0x98019043, 0x80017801, 0x70017001, 0x70015800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58008800, 0xD043D043, 0xD843D043, 0xC043C843, 0xB843B843, 0xA8439801, 0x90438001, 0x80437001, 0x70017001, 0x68005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0xA043E043, 0xE043D843, 0xD843D843, 0xD843D043, 0xD043C843, 0xB843B043, 0x98019801, 0x80018043, 0x70017001, 0x70016843, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x5800B043, 0xF043E843, 0xE843E843, 0xE043E843, 0xD843E043, 0xE043D843, 0xC843C843, 0xB843A843, 0x98018843, 0x78017843, 0x70017001, 0x68015800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0xA001F843, 0xF843F043, 0xF043F043, 0xE843E843, 0xE843E843, 0xE843E843, 0xE043D043, 0xD043C043, 0xA843A001, 0x88438001, 0x70018043, 0x70016001, 0x60005800, 0x58005800, 0x58005800, 0x58008042, 0xF885F885, 0xF843F843, 0xF843F843, 0xF843F843, 0xF843F843, 0xF843F043, 0xF043E843, 0xE043D043, 0xC843B843, 0x98438801, 0x78437001, 0x70018043, 0x58005800, 0x58005800, 0x58005800, 0x5800E109, 0xF909F8C7, 0xF8C7F885, 0xF885F843, 0xF843F843, 0xF843F843, 0xF885F843, 0xF843F043, 0xF043E843, 0xD843C043, 0xB8439801, 0x90437801, 0x78017001, 0x70015800, 0x58005800, 0x58005800, 0x8884F98D, 0xF94BF94B, 0xF909F8C7, 0xF8C7F885, 0xF843F843, 0xF885F843, 0xF843F885, 0xF843F885, 0xF843F043, 0xE843E043, 0xC843A843, 0x98018843, 0x78017001, 0x80435800, 0x58005800, 0x58005800, 0xC14BFA11, 0xFA11F98D, 0xF98DF94B, 0xF909F885, 0xF885F843, 0xF843F843, 0xF843F843, 0xF843F843, 0xF843F843, 0xF043E843, 0xD043C043, 0xB0439001, 0x88017843, 0x70017001, 0x58005800, 0x58005800, 0xF253FA53, 0xFA53FA53, 0xF9CFF98D, 0xF94BF8C7, 0xF885F843, 0xF885F843, 0xF885F843, 0xF885F885, 0xF843F843, 0xF843F043, 0xE043D043, 0xB043A843, 0x90438801, 0x70017001, 0x58005800, 0x58005800, 0xFB19FB19, 0xFA95FA95, 0xFA53F9CF, 0xF94BF8C7, 0xF8C7F885, 0xF885F885, 0xF885F8C7, 0xF885F885, 0xF8C7F885, 0xF843F843, 0xE843D843, 0xC843A843, 0xA0018843, 0x88017843, 0x58005800, 0x58006842, 0xFB9DFB9D, 0xFB5BFB5B, 0xFA95FA53, 0xF9CFF98D, 0xF8C7F8C7, 0xF8C7F8C7, 0xF8C7F909, 0xF909F909, 0xF909F8C7, 0xF8C7F885, 0xF843E843, 0xD043C043, 0xA843A001, 0x88438801, 0x58005800, 0x58005800, 0xFBDFFBDF, 0xFC63FC63, 0xFB9DFAD7, 0xFA53F9CF, 0xF94BF94B, 0xF94BF94B, 0xF94BF94B, 0xF98DF98D, 0xF98DF98D, 0xF94BF94B, 0xF885F043, 0xE043D043, 0xB043B043, 0x98018801, 0x58005800, 0x58005800, 0xF3DFFD29, 0xFDEFFDEF, 0xFCE7FBDF, 0xFAD7FA11, 0xF9CFF98D, 0xF98DF98D, 0xF9CFF9CF, 0xFA11FA11, 0xFA11FA11, 0xF9CFF9CF, 0xF94BF885, 0xF043D843, 0xC843B043, 0xB0439843, 0x58005800, 0x58005800, 0xCB19FDEF, 0xFFBDFFBD, 0xFE31FC63, 0xFB19FAD7, 0xFA11FA11, 0xF9CFFA53, 0xFA11FA11, 0xFAD7FA95, 0xFB19FB19, 0xFAD7FA53, 0xF9CFF909, 0xF843E843, 0xD843C843, 0xB0439001, 0x58005800, 0x58005800, 0x99CEFDEF, 0xFFBDFFBD, 0xFE73FCA5, 0xFBDFFAD7, 0xFA53FA53, 0xFA53FA11, 0xFA95FA95, 0xFAD7FB19, 0xFC21FC63, 0xFC21FB19, 0xFA53F94B, 0xF885F043, 0xE043D043, 0xC8436800, 0x58005800, 0x58005800, 0x6884D463, 0xFDEFFE73, 0xFD6BFC63, 0xFB9DFB5B, 0xFA95FA53, 0xFA53FAD7, 0xFA95FB19, 0xFB19FB9D, 0xFCE7FD6B, 0xFD29FB9D, 0xFAD7F9CF, 0xF8C7F843, 0xF043D843, 0xB0435800, 0x58005800, 0x58005800, 0x580078C6, 0xF4A5FD29, 0xFCE7FC21, 0xFBDFFB19, 0xFAD7FA95, 0xFA95FA95, 0xF29588C6, 0x894AF35B, 0xFCE7FE31, 0xFDEFFB9D, 0xFB19FA11, 0xF94BF8C7, 0xF843D843, 0x60006000, 0x58005800, 0x58005800, 0x58005800, 0x6884CB5B, 0xFCA5FC21, 0xFBDFFB5B, 0xFAD7FAD7, 0xFA95B14B, 0x60426000, 0x58007084, 0xBA95FC63, 0xFC63FB9D, 0xFAD7FA53, 0xF9CFF909, 0xD0436800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x8108B211, 0xCA95CA11, 0xA14B88C6, 0x58005800, 0x58005800, 0x58005800, 0x684270C6, 0x914AA18D, 0xCA53C98D, 0xB1098042, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800, 0x58005800
};

s32 pal[] = {
    0x294AFD29, 0xF9C1C9C1, 0xB8C10001, 0xB8016801
};

void func_8002616C(void);

/* Define vertex data for the triangle */
const s32 baseX = 50;
Vtx vertexData[] = {
    { baseX + 0, 0, 0, 0, GPACK_RGBA5551(255, 0, 0, 1), 0, 0 },
    { baseX + 50, 0, 0, 0, GPACK_RGBA5551(0, 255, 0, 1), 0, 0 },
    { baseX + 25, 50, 0, 0, GPACK_RGBA5551(0, 0, 255, 1), 0, 0 }
};

Gfx displayList[] = {
    gsSPVertex(vertexData, 3, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsSPEndDisplayList()
};

void DLWriteHook(void) {
    // gDPSetTextureImage(gMainGfxPosPtr++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, image);
    // gDPSetTile(gMainGfxPosPtr++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_LOADTILE, 0,
    //             G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD,
    //             G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD);
    // gDPLoadSync(gMainGfxPosPtr++);
    // gDPLoadTile(gMainGfxPosPtr++, G_TX_LOADTILE, 0, 0, 31, 31);
    // gDPPipeSync(gMainGfxPosPtr++);
    // gDPSetTile(gMainGfxPosPtr++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0,
    //             G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD,
    //             G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD);
    // gDPSetTileSize(gMainGfxPosPtr++, G_TX_RENDERTILE, 0, 0, 31, 31);

    // gSPVertex(gMainGfxPosPtr++, vertexData, 3, 0);
    // gSP1Triangle(gMainGfxPosPtr++, 0, 1, 2, 0);
    // gDPPipeSync(gMainGfxPosPtr++);

    // gSPVertex(gMainGfxPosPtr++, vertexData, 3, 0);
    // gSP1Triangle(gMainGfxPosPtr++, 0, 1, 2, 0);
    //gSPDisplayList(gMainGfxPosPtr++, displayList);

    // func_8002616C(); //restore from hook
}

void perFrameCFunction(void) {
    //gSP1Triangle(Gfx *gdl, s32 v0, s32 v1, s32 v2, s32 flag)
    
    debugFlag = 1;
    D_80160648 = 0x3F; //unlock all levels

    if (infiniteHealthBool == 1) {
        playerHealth = 10;
    }
    
    if (currentlyHeldButtons & L_TRIG) {
        D_80187B20 = 32.0f;
    }
    
    if (stateCooldown == 0) {
        if (currentlyPressedButtons & U_JPAD) {
            boolPrintCustomText ^= 1;
        } else {
            checkInputsForSavestates();
        }
    }

    if (unkStep == 3) {
        if (currentlyPressedButtons & D_JPAD) {
            curPowerupLock++;
            if (curPowerupLock >= 3) { //if advanced to 3, reset to 0
                curPowerupLock = 0;
            }
        }
    }

    switch (curPowerupLock) {
        case RANDOM:
            memcpy(powerUpFloatArray, randomPowerUps, sizeof(powerUpFloatArray));
            break;
        case SPEED:
            memcpy(powerUpFloatArray, alwaysSpeedPowerUps, sizeof(powerUpFloatArray));
            break;
        case X3:
            memcpy(powerUpFloatArray, always3XPowerUps, sizeof(powerUpFloatArray));
            break;
    }

    if (stateCooldown > 0) {
        stateCooldown--;
    }

    while (isSaveOrLoadActive != 0) {}
}