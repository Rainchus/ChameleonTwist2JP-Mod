#include "../include/ct2.h"

s32 stateCooldown = 0; //does this work??
s32 savestateCurrentSlot = 0;
s32 savestate1Size = 0;
s32 savestate2Size = 0;
s32 savestate3Size = 0;
volatile s32 isSaveOrLoadActive = 0;
s32 saveOrLoadStateModeToggleCooldown = 0;
s32 saveOrLoadStateMode = 0;

extern u16 currentlyPressedButtons;

#define SAVE_MODE 0
#define LOAD_MODE 1

#define ramStartAddr 0x800E87E0
#define ramEndAddr 0x80400000

#define DPAD_UP 0x0800
#define DPAD_DOWN 0x0400
#define DPAD_LEFT 0x0200
#define DPAD_RIGHT 0x0100
#define L_BUTTON 0x0020
#define R_BUTTON 0x0010
#define B_BUTTON 0x4000
#define A_BUTTON 0x8000

extern CustomThread gCustomThread;

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
#define ramAddrSavestateDataSlot2 (void*)0x805D0000
#define ramAddrSavestateDataSlot3 (void*)0x806E0000 //hopefully doesn't overflow into 0x807FFFDC (though if it does we were screwed anyway)
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
    //decompress_lz4_ct_default(ramEndAddr - ramStartAddr, savestate1Size, ramAddrSavestateDataSlot1); //always decompresses to `ramStartAddr`
    switch (savestateCurrentSlot) {
        case DPAD_LEFT_CASE:
            if (savestate1Size != 0 && savestate1Size != -1) {
                decompress_lz4_ct_default(ramEndAddr - ramStartAddr, savestate1Size, ramAddrSavestateDataSlot1); //always decompresses to `ramStartAddr`
            }  
        break;
        case DPAD_UP_CASE:
            if (savestate2Size != 0 && savestate2Size != -1) {
                decompress_lz4_ct_default(ramEndAddr - ramStartAddr, savestate2Size, ramAddrSavestateDataSlot2); //always decompresses to `ramStartAddr`
            }
        break;
        case DPAD_RIGHT_CASE:
            if (savestate3Size != 0 && savestate3Size != -1) {
                decompress_lz4_ct_default(ramEndAddr - ramStartAddr, savestate3Size, ramAddrSavestateDataSlot3); //always decompresses to `ramStartAddr`
            }  
        break;
    }
    setStatusRegister(status);
    __osRestoreInt();
    isSaveOrLoadActive = 0; //allow thread 3 to continue
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
            savestate1Size = compress_lz4_ct_default(ramStartAddr, ramEndAddr - ramStartAddr, ramAddrSavestateDataSlot1);
        break;
        case DPAD_UP_CASE:
            savestate2Size = compress_lz4_ct_default(ramStartAddr, ramEndAddr - ramStartAddr, ramAddrSavestateDataSlot2);
        break;
        case DPAD_RIGHT_CASE:
            savestate3Size = compress_lz4_ct_default(ramStartAddr, ramEndAddr - ramStartAddr, ramAddrSavestateDataSlot3);
        break;
    }
    setStatusRegister(status);
    __osRestoreInt();
    isSaveOrLoadActive = 0; //allow thread 3 to continue
}

void checkInputsForSavestates(void) {
    savestateCurrentSlot = -1;//set to invalid

    if (currentlyPressedButtons & DPAD_LEFT) {
        savestateCurrentSlot = 0;
    }

    if (currentlyPressedButtons & DPAD_UP) {
        savestateCurrentSlot = 1;
    }

    if (currentlyPressedButtons & DPAD_RIGHT) {
        savestateCurrentSlot = 2;
    }

    if (savestateCurrentSlot == -1 || stateCooldown != 0){
        return;
    }

 
    if (saveOrLoadStateMode == SAVE_MODE) {
        isSaveOrLoadActive = 1;
        osCreateThread(&gCustomThread.thread, 255, (void*)savestateMain, NULL,
                gCustomThread.stack + sizeof(gCustomThread.stack), 255);
        osStartThread(&gCustomThread.thread);
        stateCooldown = 5;
    } else {
        isSaveOrLoadActive = 1;
        osCreateThread(&gCustomThread.thread, 255, (void*)loadstateMain, NULL,
                gCustomThread.stack + sizeof(gCustomThread.stack), 255);
        osStartThread(&gCustomThread.thread);
        currentlyPressedButtons = 0;
        stateCooldown = 5;            
    }

}

#define DPAD_UP 0x0800
#define DPAD_DOWN 0x0400
#define DPAD_LEFT 0x0200
#define DPAD_RIGHT 0x0100
#define L_BUTTON 0x0020

void printText(char*);
void setTextPosition(s32, s32, s32);
void setTextScale(f32 xScale, f32 yScale);
void setTextOpacity(u8);

//8003CBAC

void cBootFunction(void) { //ran once on boot
    crash_screen_init();
    stateCooldown = 0; //does this work??
    savestateCurrentSlot = 0;
    savestate1Size = 0;
    savestate2Size = 0;
    savestate3Size = 0;
    isSaveOrLoadActive = 0;
    saveOrLoadStateModeToggleCooldown = 0;
    saveOrLoadStateMode = 0;
}

void perFrameCFunction(void) {
    if (stateCooldown == 0) {
        if (saveOrLoadStateModeToggleCooldown == 0) {
            if (currentlyPressedButtons & DPAD_DOWN) {
                saveOrLoadStateMode ^= 1;
            } else {
                checkInputsForSavestates();
            }
        }
    }

    if (stateCooldown > 0) {
        stateCooldown--;
    }

    if (saveOrLoadStateModeToggleCooldown > 0) {
        saveOrLoadStateModeToggleCooldown--;
    }

    while (isSaveOrLoadActive != 0) {}
}
