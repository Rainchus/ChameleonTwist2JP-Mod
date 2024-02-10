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

.definelabel func_800CB630, 0x800CB630;
.definelabel func_800CBA4C, 0x800CBA4C;
.definelabel func_800CC270, 0x800CC270;
.definelabel func_800CD5D0, 0x800CD5D0;
.definelabel D_800EAE78, 0x800EAE78;
.definelabel D_800F1C30, 0x800F1C30;
.definelabel D_800F1C54, 0x800F1C54;
.definelabel D_800F1C70, 0x800F1C70;
.definelabel func_800CB5C0, 0x800cb5c0;

//mainThread 8001C81C

.headersize 0x80024C00
.org 0x800266B0
bootHook:

LI a0, 0xC00000
LUI a1, 0x8040
LUI a2, 0x0002
JAL dma_copy
ORI a2, a2, 0x0000
J copiedHookCode
NOP

dma_copy:
ADDIU sp, sp, -0x80
SW ra, 0x007C (sp)
SW s0, 0x0078 (sp)
SW s1, 0x0074 (sp)
SW s2, 0x0070 (sp)

ADDU s0, a0, r0
ADDU s1, a1, r0
ADDU s2, a2, r0

ADDIU a0, sp, 0x38
ADDIU a1, sp, 0x34
JAL osCreateMesgQueue
ADDIU a2, r0, 1

ADDU a0, s0, r0
JAL osInvalDCache
ADDU a1, s2, r0

ADDU a0, s0, r0
JAL osInvalICache
ADDU a1, s2, r0

ADDIU a0, sp, 0x50
ADDIU a1, r0, 0
ADDIU a2, r0, 0
ADDIU a3, sp, 0x38
SW a3, 0x0018 (sp)
ADDU a3, s0, r0 //romAddr
SW s1, 0x0010 (sp) //ramAddr
JAL osPiStartDma
SW s2, 0x0014 (sp) //dmaSize

ADDIU a0, sp, 0x38
ADDU a1, r0, r0
JAL osRecvMesg
ADDIU a2, r0, 1

LW ra, 0x007C (sp)
LW s0, 0x0078 (sp)
LW s1, 0x0074 (sp)
LW s2, 0x0070 (sp)
JR RA
ADDIU sp, sp, 0x80

.org 0x8002691C
J moreReplacedCode

.org 0x80029610
J perFrameASM
NOP

//mushroom boss
.org 0x800A058C
J hookAt800A058C
ADDIU sp, sp, 0x28

//burger boss
.org 0x8009D394
J hookAt8009D394
ADDIU sp, sp, 0x78

//wheelrus
.org 0x8009E9F8
J hookAt8009E9F8
ADDIU sp, sp, 0x28

//frog boss
.org 0x8009B66C
J hookAt8009B66C
NOP

//robot boss
.org 0x800A3150
J hookAt800A3150
ADDIU sp, sp, 0x28

//sphynx boss
.org 0x800A6384
J hookAt800A6384
ADDIU sp, sp, 0x28

//.org 0x800267A0
//J DLWriteHookAsm
//NOP

//.org 0x80031540
//J DLWriteHookAsm2
//ADDIU sp, sp, 0xB0

//.org 0x80030D68
//J DLWriteHookAsm3
//NOP

//.org 0x800260E0
//J DLWriteHookAsm4
//NOP

//.org 0x800269F8
//J DLWriteHookAsm5
//NOP

//.org 0x800261C8
//JAL DLWriteHookAsm6
//NOP

//.org 0x80035F54
//J DLWriteHookAsm7
//ADDIU sp, sp, 0x58

.org 0x80028D28
J stuff
SH t6, 0x7BBC (at)
