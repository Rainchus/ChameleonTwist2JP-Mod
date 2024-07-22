// Tell armips' linker where the assets we declared in sm64.h are located
.definelabel osGetTime, 0x800E2CF0
.definelabel __osViIntrCount, 0x801CCA8C

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
//.definelabel gCrashScreen, 0x807F6000
.definelabel savestate1Size, 0x807FFFE8
.definelabel voidOutCalls, 0x807FFFEC
.definelabel callsEnteringSkyland, 0x807FFFF0
.definelabel seedAtPowerup, 0x807FFFF4
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
.definelabel playerHealth, 0x80187BBC
.definelabel D_80187b20, 0x80187b20

.definelabel func_800505E0, 0x800505E0
.definelabel func_80050694, 0x80050694
.definelabel func_8005070C, 0x8005070C
.definelabel D_80160808, 0x80160808
.definelabel func_80050428, 0x80050428
.definelabel func_800507c8, 0x800507c8
.definelabel powerUpFloatArray, 0x800ECF64
//.definelabel gPlayerActor, 0x80187290
.definelabel gPlayerActors, 0x80187B00

.definelabel osMemSize, 0x80000318
.definelabel __osCurrentTime, 0x801CCA84
.definelabel memcpy, 0x800E16D0
.definelabel _Printf, 0x800E0ED0
.definelabel osWritebackDCacheAll, 0x800E80C0
.definelabel osViBlack, 0x800DD0B0
.definelabel osViSwapBuffer, 0x800E2780
.definelabel __osActiveQueue, 0x800F2AFC
.definelabel osSetEventMesg, 0x800E0370
.definelabel __osAiDeviceBusy, 0x800E8B50
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
.definelabel respawnZone, 0x800EAE00
.definelabel gGameMode, 0x800EAF60
.definelabel debugFlag, 0x800EAE64
.definelabel textRed, 0x801857A8
.definelabel textBlue, 0x801857AA
.definelabel textGreen, 0x801857AC
.definelabel textOpacity, 0x801857AE
.definelabel ifTextColor, 0x801857B0
.definelabel textKerning, 0x801857B2
.definelabel textWidth, 0x801857B4
.definelabel textHeight, 0x801857B8
.definelabel gMainGfxPosPtr, 0x80101440
.definelabel func_8002616C, 0x8002616C
.definelabel p1Airborne, 0x80187B38
.definelabel p1IfParasol, 0x80187B54
.definelabel gZones, 0x801857F0
.definelabel camera, 0x80163030
.definelabel guTranslate, 0x800E5AA8
.definelabel MatrixBuffer, 0x801604E8
.definelabel imageStuff, 0x80187C10
.definelabel D_800F57BC, 0x800F57BC
.definelabel guPerspective, 0x800DCF10
.definelabel guLookAt, 0x800DDB78
.definelabel guMtxCatL, 0x800E0520
.definelabel bossHpBar, 0x8016063C
.definelabel osGetCount, 0x800DF550
.definelabel _strlen, 0x800E16FC
.definelabel D_800EAF60, 0x800EAF60
.definelabel D_800EAF64, 0x800EAF64
.definelabel func_800E8C00, 0x800E8C00
.definelabel func_8003C9DC, 0x8003C9DC
.definelabel func_800D4DE0, 0x800D4DE0
.definelabel D_800EAD5C, 0x800EAD5C
.definelabel D_800FF20E, 0x800FF20E
.definelabel func_800D6160, 0x800D6160
.definelabel func_800293F0, 0x800293F0
.definelabel func_800BE480, 0x800BE480
.definelabel DisplayHealth, 0x80039D3C
.definelabel gCurArea, 0x80160634

.definelabel func_800CB630, 0x800CB630;
.definelabel func_800CBA4C, 0x800CBA4C;
.definelabel func_800CC270, 0x800CC270;
.definelabel func_800CD5D0, 0x800CD5D0;
.definelabel D_800EAE78, 0x800EAE78;
.definelabel D_800F1C30, 0x800F1C30;
.definelabel D_800F1C54, 0x800F1C54;
.definelabel D_800F1C70, 0x800F1C70;
.definelabel func_800CB5C0, 0x800cb5c0;

.definelabel UnlockedCostumes, 0x8016451A;

//mainThread 8001C81C
