#include "../include/ct2.h"

s32 stateCooldown = 0; //does this work??
s32 savestateCurrentSlot = 0;
s32 savestate1Size = 0;
s32 savestate2Size = 0;
s32 savestate3Size = 0;
volatile s32 isSaveOrLoadActive = 0;
s32 saveOrLoadStateMode = 0;

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
//#define ramAddrSavestateDataSlot2 (void*)0x805D0000
//#define ramAddrSavestateDataSlot3 (void*)0x806E0000 //hopefully doesn't overflow into 0x807FFFDC (though if it does we were screwed anyway)
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

// void printDigit(char*);
// void setTextPosition(s32, s32, s32);
// void setTextScale(f32 xScale, f32 yScale);
// void setTextOpacity(u8);

extern u8 D_80160648;
extern f32 D_80187B20; //is 0x20 into gPlayerActors[0]
extern s16 D_80187BBC;

typedef struct unkStructCt2 {
    char unk_00[2];
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

// s32 CustomprintDigitTest(unkStructCt2_arg0* arg0, s16 arg1) {
//     unkStructCt2 sp2C;
//     u8* temp_a1;
//     u8* temp_v1;

//     arg0->unk_48 = 4;
//     func_80039D3C();
//     func_800510A8(0x20, 0x10, 0x190, 0xE0);
//     func_80050F54(4);
//     setTextScale(D_800F6498, D_800F6498);
//     func_80050F9C(1);
//     func_80051034(1);
//     setTextGradient(0, 255, 0x80, 0, 255);
//     setTextGradient(3, 255, 0x80, 0, 255);
//     setTextGradient(1, 255, 255, 0, 255);
//     setTextGradient(2, 255, 255, 0, 255);
//     setTextPosition((s32) (arg0->pos.x + 20.0f), (s32) arg0->pos.y, (s32) arg0->pos.z);
//     printIcon(&sp2C, &D_800F6410);
//     temp_v1 = &D_800ECFD0[arg1 / 10];
//     sp2C.unk_06 = 0;
//     sp2C.unk_02 = temp_v1[0];
//     sp2C.unk_03 = temp_v1[1];
//     temp_a1 = &D_800ECFD0[arg1 % 10];
//     sp2C.unk_04 = temp_a1[0];
//     sp2C.unk_05 = temp_a1[1];
//     printDigit(&sp2C, temp_a1);
//     return 0;
// }

// s32 func_80050428_Hook(unkStructCt2_arg0* arg0, s16 arg1) {
//     unkStructCt2 sp2C;
//     u8* temp_a1;
//     u8* temp_v1;
//     char textTest[40];

//     arg0->unk_48 = 4;
//     func_80039D3C();
//     func_800510A8(0x20, 0x10, 0x190, 0xE0);
//     func_80050F54(4);
//     setTextScale(D_800F6498, D_800F6498);
//     func_80050F9C(1);
//     func_80051034(1);
//     setTextGradient(0, 255, 0x80, 0, 255);
//     setTextGradient(3, 255, 0x80, 0, 255);
//     setTextGradient(1, 255, 255, 0, 255);
//     setTextGradient(2, 255, 255, 0, 255);
//     setTextPosition((s32) (arg0->pos.x + 20.0f), (s32) arg0->pos.y, (s32) arg0->pos.z);
//     printIcon(&sp2C, &D_800F6410);
//     temp_v1 = &D_800ECFD0[arg1 / 10];
//     sp2C.unk_06 = 0;
//     sp2C.unk_02 = temp_v1[0];
//     sp2C.unk_03 = temp_v1[1];
//     temp_a1 = &D_800ECFD0[arg1 % 10];
//     sp2C.unk_04 = temp_a1[0];
//     sp2C.unk_05 = temp_a1[1];
//     printDigit(&sp2C, temp_a1);

//     return 0;
// }

void cBootFunction(void) { //ran once on boot
    crash_screen_init();
    stateCooldown = 0;
    savestateCurrentSlot = 0;
    savestate1Size = 0;
    isSaveOrLoadActive = 0;
    //hookCode((void*)0x80050428, &func_80050428_Hook);
}

void unkPrintFunction(void*);

void printCustomText(void) { //a3 is message pointer
    //unkPrintFunction((void*)0x800F0AE0);
    // printText(0x41, 0x42, 0x0C, (void*)0x800F99FC, 0xFF, 0x60, 0, 0xFF, 1.0f, 1.0f);
    printText(0x32, 0x5F, 0x0C, (void*)0x80101174, 0xFF, 0xFF, 0, 0xFF, 1.0f, 1.0f);
}

void perFrameCFunction(void) {
    D_80160648 = 0x3F; //unlock all levels
    D_80187BBC = 10;
    if (currentlyHeldButtons & L_BUTTON) {
        D_80187B20 = 32.0f;
    }

    printCustomText();

    if (stateCooldown == 0) {
        if (currentlyPressedButtons & DPAD_DOWN) {
            //does nothing atm
        } else {
            checkInputsForSavestates();
        }
    }

    if (stateCooldown > 0) {
        stateCooldown--;
    }



    while (isSaveOrLoadActive != 0) {}
}