// .headersize 0x7FFFF400
// .org 0x8005B6EC
// JAL DisplacedBootCode
// NOP

//0xC0500 ROM
//0x800BF900 RAM

// .headersize 0x7FFFF400
// .org 0x8005B6E4
// LI a0, 0xB9820 //ROM addr to read from
// LI a1, 0x807E3000 //RAM addr to write to
// LI a2, 0x8000 //size to DMA
// JAL dmaCopy2
// NOP
// J DisplacedBootCode
// NOP

//mainThread 8001C81C

.headersize 0x80024C00
.org 0x800266B0
bootHook:

LI a0, 0x7C1400
LI a1, 0x80406100
JAL dma_copy
ADDIU a2, r0, 0x7000
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
J bigLabel


.headersize 0x7FC44D00
.org 0x80406100
copiedHookCode:
JAL cBootFunction
NOP
LUI a1, 0x8010
ADDIU a1, a1, 0xF230
OR a0, s2, r0
JAL osCreateMesgQueue
ADDIU a2, r0, 1

LUI a0, 0x8002
JAL 0x800D4040
ADDIU a0, a0, 0x6604
OR a0, s2, r0
ADDIU a1, r0, 1
JAL osSendMesg
OR a2, r0, r0
JAL 0x80026080
NOP
LUI at, 0x42C8
LUI t6, 0x8000
MTC1 at, f4
LW t6, 0x0300 (t6)
LUI at, 0x8016
LUI t8, 0x800F
BNEZ t6, copiedHookLabel0
SWC1 f4, 0x2E70 (at)
LUI t7, 0x0015
ORI t7, t7, 0x5CC0
LUI at, 0x800F
SW t7, 0xAD40 (at)
copiedHookLabel0:
LW t8, 0xAD40 (t8)
LUI at, 0x800F
LUI s6, 0x8010
LUI s5, 0x800F
LUI s4, 0x800F
LUI s3, 0x800F
LUI s1, 0x8010
LUI s0, 0x8010
ADDIU s0, s0, 0x1418
ADDIU s1, s1, 0xF238
ADDIU s3, s3, 0xAD44
ADDIU s4, s4, 0xAD74
ADDIU s5, s5, 0xAD30
ADDIU s6, s6, 0xF1E0
SW t8, 0xAD3C (at)
bigLabel:
JAL perFrameCFunction
NOP
OR a0, s2, r0
OR a1, r0, r0
JAL osRecvMesg
ADDIU a2, r0, 1
JAL 0x800E2CF0
NOP
SW v0, 0x0000 (s0)
J 0x80026770
SW v1, 0x0004 (s0)
//80026770



//0x800D22E0
//0xAD6E0

//.org 0x800D22E0
//.importobj "obj/dmaCopy.o"

//.org 0x800988C0
//store at 0x80400000

//80400000
//0x7C1400

__osDpDeviceBusy:
LUI t6, 0xA410
LW a0, 0x000C (t6)
ADDIU sp, sp, -0x08
ANDI t7, a0, 0x0100
BEQZ t7, dpRet
NOP
B dpExit
ADDIU v0, r0, 1
dpRet:
OR v0, r0, r0
dpExit:
JR RA
ADDIU sp, sp, 0x08


START:
osViRepeatLine:
ADDIU          SP, SP, -0x18
SW             S0, 0x0010 (SP)
SW             RA, 0x0014 (SP)
JAL            __osDisableInt
ADDU           S0, A0, R0
ANDI           S0, S0, 0x00FF
BEQZ           S0, label1
ADDU           A0, V0, R0
LUI            V1, 0x800F
LW             V1, 0x40C4 (V1)
LHU            V0, 0x0000 (V1)
J              label0
ORI            V0, V0, 0x0040
label1:
LUI            V1, 0x800F
LW             V1, 0x40C4(V1)
LHU            V0, 0x0000 (V1)
ANDI           V0, V0, 0xFFBF
label0:
JAL            __osRestoreInt
SH             V0, 0x0000 (V1)
LW             RA, 0x0014 (SP)
LW             S0, 0x0010 (SP)
JR             RA
ADDIU          SP, SP, 0x18

crash_screen_sleep: //takes arg a0, ms to sleep
ADDIU sp, sp, -0x20
ORI v0, r0, 0xB71B
MULT a0, v0
DADDU a0, r0, r0
DADDU a1, r0, r0
SW ra, 0x0018 (sp)
SW s1, 0x0014 (sp)
SW s0, 0x0010 (sp)
MFHI s0
MFLO s1
JAL osSetTime
NOP
crashScreenLoop:
JAL osGetTime
NOP
DADDU a0, v0, r0
DADDU a1, v1, r0
SLTU v0, a0, s0
BNEZ v0, crashScreenLoop
NOP
BNE s0, a0, exitCrashFunc
SLTU v0, a1, s1
BNEZ v0, crashScreenLoop
NOP
exitCrashFunc:
LW ra, 0x0018 (sp)
LW s1, 0x0014 (sp)
LW s0, 0x0010 (sp)
JR RA
ADDIU sp, sp, 0x20

customMemCpy: //requires 0x08 alignment
BLEZ a2, exitMemCpy
LD t0, 0x0000 (a1)
SD t0, 0x0000 (a0)
ADDIU a1, a1, 8
ADDIU a0, a0, 8
BEQ r0, r0, customMemCpy
ADDIU a2, a2, -8
exitMemCpy:
JR RA
NOP

//so emulator wont close the game upon infinite loop
infiniteLoop:
NOP
J infiniteLoop
NOP

.importobj "obj/crash.o"
.importobj "obj/dmaCopy.o"

END: