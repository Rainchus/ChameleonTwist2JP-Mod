#include "../include/ct2.h"

s32 stateCooldown = 0; //does this work??
s32 savestateCurrentSlot = 0;
s32 savestate1Size = 0;
s32 savestate2Size = 0;
s32 savestate3Size = 0;
s32 printPowerUp = 1;
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
    s32 register status = getStatusRegister();
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
    __osDisableInt();
    switch (savestateCurrentSlot) {
        case DPAD_LEFT_CASE:
            if (savestate1Size != 0 && savestate1Size != -1) {
                //decompress_lz4_ct_default(ramEndAddr - ramStartAddr, savestate1Size, ramAddrSavestateDataSlot1); //always decompresses to `ramStartAddr`
                customMemCpy(ramStartAddr, ramAddrSavestateDataSlot1, savestate1Size);
            }  
        break;
        break;
    }
    setStatusRegister(status);
    __osRestoreInt();
    isSaveOrLoadActive = 0; //allow thread to continue
}
    
void savestateMain(void) {
    //push status
    s32 register status = getStatusRegister();
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

    __osDisableInt();
    switch (savestateCurrentSlot) {
        case DPAD_LEFT_CASE:
            customMemCpy(ramAddrSavestateDataSlot1, ramStartAddr, ramEndAddr - ramStartAddr);
            savestate1Size = ramEndAddr - ramStartAddr;
        break;
    }
    setStatusRegister(status);
    __osRestoreInt();
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

typedef struct unkStructCt2 {
    u8 unk_00; //sp2C
    u8 unk_01; //sp2D
    u8 unk_02; //sp2E
    u8 unk_03; //sp2F
    u8 unk_04; //sp30
    u8 unk_05; //sp31
    s8 unk_06; //sp32
} unkStructCt2;

typedef struct unkStructCt2_arg0 {
    char unk_00[0x48];
    s16 unk_48;
    char unk_4A[0x22];
    Vec3f pos;
} unkStructCt2_arg0;

void func_80039D3C(void);                                  /* extern */
void printDigit(unkStructCt2*, void*);                         /* extern */
void setTextPosition(s32, s32, s32);                     /* extern */
void func_80050F54(s32);                                 /* extern */
void func_80050F9C(s32);                                 /* extern */
void setTextScale(f32, f32);                          /* extern */
void func_80051034(s32);                                 /* extern */
void setTextGradient(s32, u8, u8, s32, s32);                   /* extern */
void func_800510A8(s32, s32, s32, s32);                        /* extern */
void printIcon(unkStructCt2*, s32*);                            /* extern */
extern u8 D_800ECFD0[]; //array of characters for integers
extern s32 D_800F6410; //?
extern f32 D_800F6498;

typedef struct unkTextPointer2 {
    char unk_00[0x0A];
    s16 unk_0A;
} unkTextPointer2;

typedef struct unkTextPointer {
    /* 0x00 */ char unk_00[8];
    /* 0x08 */ unkTextPointer2* unk_08;
    /* 0x0C */ char unk_0C[0x3C];
    /* 0x48 */ s32 unk48;
    /* 0x4C */ char unk_4C[4];
    /* 0x50 */ s32 unk50;
    /* 0x54 */ char unk_54[0x18];
    /* 0x6C */ f32 unk6C;
    /* 0x70 */ f32 unk70;
    /* 0x74 */ char unk_74[0x88];
    /* 0xFC */ s32 unkFC;
} unkTextPointer;

unkTextPointer testingVar = {0};

void func_800505E0(unkTextPointer*);
s32 func_8005070C(unkTextPointer*); //moves text off screen if need be
void func_80050428(unkTextPointer*, s32 value);
s32 func_80050694(unkTextPointer*);
void func_800507C8(unkTextPointer*);
extern s32 D_80160808;

s32 curPowerupLock = 0;

void func_80050770_Hook(unkTextPointer* arg0) {
    if (D_80160808 != 0) {
        func_80050694(arg0);
        func_800505E0(arg0); //prints text
    }
    else if (func_8005070C(arg0) == 0) {
        func_800505E0(arg0);
    }

    if (printPowerUp == 0) {
        return;
    }

    //else print custom
    testingVar.unk48 = 4;   
    testingVar.unkFC = 0;
    testingVar.unk6C = 15.0f; //xpos
    testingVar.unk70 = 205.0f; //ypos

    func_80050428(&testingVar, curPowerupLock);
}

void cBootFunction(void) { //ran once on boot
    crash_screen_init();
    stateCooldown = 0;
    savestateCurrentSlot = 0;
    savestate1Size = 0;
    isSaveOrLoadActive = 0;
    hookCode((void*)0x80050770, &func_80050770_Hook);
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
            printPowerUp ^= 1;
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