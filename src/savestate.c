#include "ct2.h"
#include "PR/os_internal.h"

#define ramAddrSavestateDataSlot1 (void*)0x804C0000
#define ramStartAddr 0x800E87D0 //
#define ramEndAddr 0x80400000 //can probably skip the frame buffer at around 0x80360000 - 0x80400000 approximately

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

    //load state
    customMemCpy((void*)ramAddrSavestateDataSlot1, (void*)ramStartAddr, ramEndAddr - ramStartAddr);

    __osRestoreInt(mask);
    isSaveOrLoadActive = 0; //allow thread to continue
}

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
	osInvalDCache((void*)0x80000000, 0x2000);
    
    //instead of checking savestate size, check savestate header to see if it's valid
    //this will allow the user to load a savestate after crashing
    
    if (savestateHeader == 0x03E00008) {
        customMemCpy((void*)ramStartAddr, ramAddrSavestateDataSlot1, (u32)ramEndAddr - (u32)ramStartAddr);
        osInvalICache((void*)0x80000000, 0x4000);
    }

    osWritebackDCacheAll();
    __osRestoreInt(mask);
    isSaveOrLoadActive = 0; //allow thread to continue
}