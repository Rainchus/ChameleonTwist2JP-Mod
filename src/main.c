#include "../include/ct2.h"

s32 stateCooldown = 0; //does this work??
s32 savestateCurrentSlot = 0;
s32 savestate1Size = 0;
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

#define ramStartAddr 0x800E87E0
#define ramEndAddr 0x80400000 //can probably skip the frame buffer at around 0x80360000 - 0x80400000 approximately

#define DPAD_UP 0x0800
#define DPAD_DOWN 0x0400
#define DPAD_LEFT 0x0200
#define DPAD_RIGHT 0x0100
#define L_BUTTON 0x0020
#define R_BUTTON 0x0010
#define B_BUTTON 0x4000
#define A_BUTTON 0x8000

extern CustomThread gCustomThread;

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
#define DPAD_LEFT_CASE 0
#define DPAD_UP_CASE 1
#define DPAD_RIGHT_CASE 2

void loadstateMain(void) {
    u32 mask;
    //wait on rsp
    while (__osSpDeviceBusy() == 1) {}

    //wait on rdp
    while ( __osDpDeviceBusy() == 1) {}

    //wait on SI
    while (__osSiDeviceBusy() == 1) {}

    //wait on PI
    while (__osPiDeviceBusy() == 1) {}

    //invalidate caches
    osInvalICache((void*)0x80000000, 0x2000);
	osInvalDCache((void*)0x80000000, 0x2000);
    mask = __osDisableInt();
    switch (savestateCurrentSlot) {
        case DPAD_LEFT_CASE:
            if (savestate1Size != 0 && savestate1Size != -1) {
                //decompress_lz4_ct_default(ramEndAddr - ramStartAddr, savestate1Size, ramAddrSavestateDataSlot1); //always decompresses to `ramStartAddr`
                customMemCpy(ramStartAddr, ramAddrSavestateDataSlot1, savestate1Size);
            }  
        break;
        break;
    }
    __osRestoreInt(mask);
    isSaveOrLoadActive = 0; //allow thread to continue
}
    
void savestateMain(void) {
    u32 mask;
    //wait on rsp
    while (__osSpDeviceBusy() == 1) {}

    //wait on rdp
    while ( __osDpDeviceBusy() == 1) {}

    //wait on SI
    while (__osSiDeviceBusy() == 1) {}

    //wait on PI
    while (__osPiDeviceBusy() == 1) {}

    //invalidate caches
    osInvalICache((void*)0x80000000, 0x2000);
	osInvalDCache((void*)0x80000000, 0x2000);

    mask = __osDisableInt();
    switch (savestateCurrentSlot) {
        case DPAD_LEFT_CASE:
            customMemCpy(ramAddrSavestateDataSlot1, ramStartAddr, ramEndAddr - ramStartAddr);
            savestate1Size = ramEndAddr - ramStartAddr;
        break;
    }
    __osRestoreInt(mask);
    isSaveOrLoadActive = 0; //allow thread to continue
}

void checkInputsForSavestates(void) {
    savestateCurrentSlot = 0;//set to 0

    if (currentlyPressedButtons & DPAD_LEFT) {
        isSaveOrLoadActive = 1;
        osCreateThread(&gCustomThread.thread, 255, (void*)savestateMain, NULL,
                gCustomThread.stack + sizeof(gCustomThread.stack), 255);
        osStartThread(&gCustomThread.thread);
        stateCooldown = 5;
    } else if (currentlyPressedButtons & DPAD_RIGHT) {
        isSaveOrLoadActive = 1;
        osCreateThread(&gCustomThread.thread, 255, (void*)loadstateMain, NULL,
                gCustomThread.stack + sizeof(gCustomThread.stack), 255);
        osStartThread(&gCustomThread.thread);
        currentlyPressedButtons = 0;
        stateCooldown = 5; 
    }
}

extern u8 D_80160648;
extern f32 D_80187B20; //is 0x20 into gPlayerActors[0]
extern s16 D_80187BBC;
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
s32 curPowerupLock = 0;

void _sprintf(void* destination, void* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _Printf((void*)crash_screen_copy_to_buf, destination, fmt, args);
    va_end(args);
}



u32 func_800E0790_Hook(void) {
    u32 y, z;
    y = rngSeed * 4 + 2;
    z = y + 3;
    y = y * z;
    rngSeed = y / 4;
    calls++;
    return rngSeed;
}

void copyCallsToPowerupCalls(void);
void printCustomText(void);

typedef struct TextPosition {
    s32 xPos;
    s32 yPos;
} TextPosition;

void printCallsUntilDecidedPowerup(void) {
    u8 buffer[40];
    TextPosition textPos = {61, 196};

    _bzero(buffer, sizeof(buffer));
    textStyle = 1;
    setDebugTextPosition(textPos.xPos, textPos.yPos, 0x32);
    _sprintf(buffer, "%d", callsAtPowerupDecision);
    printDebugText(buffer);
}

void printCurrentSpeed(void) {
    u8 buffer[40];
    TextPosition textPos = {20, 210};

    _bzero(buffer, sizeof(buffer));
    textStyle = 1;
    setDebugTextPosition(textPos.xPos, textPos.yPos, 0x32);
    _sprintf(buffer, "%2.2f", gPlayerActor->speed);
    printDebugText(buffer);
}

void printCurrentPowerupLock(void) {
    TextPosition textPos = {61, 210};

    textStyle = 1;
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

    //otherwise, print stuff
    printCallsUntilDecidedPowerup();
    printCurrentPowerupLock();
    printCurrentSpeed();
}

void cBootFunction(void) { //ran once on boot
    crash_screen_init();
    stateCooldown = 0;
    savestateCurrentSlot = 0;
    savestate1Size = 0;
    isSaveOrLoadActive = 0;
    hookCode((void*)0x800E0790, &func_800E0790_Hook); //rng hook to track call total
    hookCode((void*)0x8004FC08, &copyCallsToPowerupCalls); //when powerup is grabbed, copy calls to another var
    hookCode((void*)0x800B3EA8, &printCustomText); //print our custom text when in game
    
    calls = 0;
    callsAtPowerupDecision = 0;
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

void perFrameCFunction(void) {
    D_80160648 = 0x3F; //unlock all levels
    D_80187BBC = 10;
    if (currentlyHeldButtons & L_BUTTON) {
        D_80187B20 = 32.0f;
    }
    
    if (stateCooldown == 0) {
        if (currentlyPressedButtons & DPAD_UP) {
            boolPrintCustomText ^= 1;
        } else {
            checkInputsForSavestates();
        }
    }

    if (currentlyPressedButtons & DPAD_DOWN) {
        curPowerupLock++;
        if (curPowerupLock >= 3) { //if advanced to 3, reset to 0
            curPowerupLock = 0;
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