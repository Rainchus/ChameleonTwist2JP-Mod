// Tell armips' linker where the assets we declared in sm64.h are located
// .definelabel dmaCopy, 0x80062088
// .definelabel dmaCopy2, 0x80061FE8
// .definelabel sleepVProcess, 0x800635B4
// .definelabel drawDebugText, 0x800622BC
// .definelabel mp1_sprintf, 0x8008C490
// .definelabel mallocPerm, 0x8003B6A4
// .definelabel createBox, 0x80062364
.definelabel osGetTime, 0x800E2CF0
// .definelabel osGetCount, 0x80093780
// .definelabel __osVIIntrCount, 0x800ED608
// .definelabel rng_seed, 0x800C2FF4
// .definelabel GetRandomByte, 0x8001758C
// .definelabel rngSeed, 0x800C2FF4
// .definelabel func_80018B2C, 0x80018B2C
// .definelabel func_8001E5A0, 0x8001E5A0
// .definelabel func_800247FC, 0x800247FC
// .definelabel func_8005E3FC, 0x8005E3FC
// .definelabel boxDrawn, 0x800C59A6
// .definelabel box0Struct, 0x800ED448
// .definelabel minigameIslandLives, 0x800F37BB
// .definelabel func_800672DC, 0x800672DC
// .definelabel func_800635B4, 0x800635B4
// .definelabel func_800672B0, 0x800672B0
// .definelabel D_800D84E4, 0x800D84E4
// .definelabel func_8002B6C8, 0x8002B6C8
// .definelabel func_8001DFC0, 0x8001DFC0
// .definelabel func_80025658, 0x80025658
// .definelabel func_800621D8, 0x800621D8
// .definelabel osContGetReadData, 0x80090120


.definelabel func_80039D3C, 0x80039D3C
.definelabel func_800510A8, 0x800510A8
.definelabel func_80050F54, 0x80050F54
.definelabel func_80050F9C, 0x80050F9C
.definelabel func_80051034, 0x80051034

.definelabel D_800ECFD0, 0x800ECFD0
.definelabel D_800F6410, 0x800F6410
.definelabel D_800F6498, 0x800F6498

.definelabel printText, 0x800BF230
.definelabel unkPrintFunction, 0x80051468
//.definelabel cFile, 0x800C2000
.definelabel gCustomThread, 0x807F0000
.definelabel gCrashScreen, 0x807F6000
.definelabel calls, 0x807FFFF8
.definelabel callsAtPowerupDecision, 0x807FFFFC
.definelabel printDigit, 0x80050DA0
//.definelabel printIcon, 0x800E0804
.definelabel setTextGradient, 0x80051040
.definelabel setTextPosition, 0x80050F38
.definelabel setTextScale, 0x80051020
.definelabel setTextOpacity, 0x80049DE8
.definelabel currentlyHeldButtons, 0x800FF1C0
.definelabel currentlyPressedButtons, 0x800FF1C6
.definelabel D_80160648, 0x80160648
.definelabel D_80187BBC, 0x80187BBC
.definelabel D_80187b20, 0x80187b20

.definelabel func_800505E0, 0x800505E0
.definelabel func_80050694, 0x80050694
.definelabel func_8005070C, 0x8005070C
.definelabel D_80160808, 0x80160808
.definelabel func_80050428, 0x80050428
.definelabel func_800507c8, 0x800507c8
.definelabel powerUpFloatArray, 0x800ECF64
.definelabel gPlayerActor, 0x80187290

.definelabel osMemSize, 0x80000318
.definelabel __osCurrentTime, 0x801CCA84
.definelabel memcpy, 0x800E16D0
.definelabel _Printf, 0x800E0ED0
.definelabel osWritebackDCacheAll, 0x800E80C0
.definelabel osViBlack, 0x800DD0B0
.definelabel osViSwapBuffer, 0x800E2780
.definelabel __osActiveQueue, 0x800F2AFC
.definelabel osSetEventMesg, 0x800E0370

.definelabel osRecvMesg, 0x800D78A0
.definelabel osStopThread, 0x800D89D0
.definelabel osCreateMesgQueue, 0x800E1770
.definelabel osCreateThread, 0x800DBE70
.definelabel osStartThread, 0x800E8630
.definelabel __osDisableInt, 0x800E39E0
.definelabel __osRestoreInt, 0x800E3A00
.definelabel osInvalDCache, 0x800E1820
.definelabel osInvalICache, 0x800E17A0
.definelabel osPiStartDma, 0x800DF560
.definelabel osDestroyThread, 0x800D89D0
.definelabel osSendMesg, 0x800D75B0
.definelabel __osSpDeviceBusy, 0x800E24D0
.definelabel __ossidevicebusy, 0x800E2500
.definelabel rngSeed, 0x800F3FF0
.definelabel func_800E80B0, 0x800E80B0
.definelabel func_80026080, 0x80026080
.definelabel p1ButtonsPressed, 0x800FF1C6
.definelabel printDebugText, 0x80050DA0
.definelabel setDebugTextPosition, 0x80050F38
.definelabel formatText, 0x800E0804
.definelabel drawDebugScreenText, 0x801FFB3C //dynamically loaded, dont call
.definelabel sprintf, 0x800E0804
.definelabel textStyle, 0x80185796
.definelabel _bzero, 0x800DA130