//custom code jumped to mostly by things specified in "patches.asm"

.headersize 0x7F800000
.org 0x80400000

ReplacedGameLoopCode:
OR a0, s2, r0
OR a1, r0, r0
JAL osRecvMesg
ADDIU a2, r0, 0x0001
J 0x80026760
NOP

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

//so emulator wont close the game upon infinite loop
infiniteLoop:
NOP
J infiniteLoop
NOP

beforePrintSetVariable:
ADDIU sp, sp, -0x18
SW ra, 0x0010 (sp)
SW a0, 0x0014 (sp)
JAL DisplayDebugMenu
NOP
LW ra, 0x0010 (sp)
LW a0, 0x0014 (sp)
ADDIU sp, sp, 0x18
LUI a3, 0x8010
J 0x802032DC
ADDIU a3, a3, 0xF1C0

printCustomText2:
ADDIU sp, sp, -0x18 //restore from hook
SW a0, 0x0018 (sp) //restore from hook

addiu sp, sp, -0x20
SW ra, 0x0018 (sp)
SW ra, 0x0014 (sp)

JAL printCustomTextInC
NOP
LW ra, 0x0018 (sp)
J 0x80028594
addiu sp, sp, 0x20

guRandHook:
LI t0, calls
LW t1, 0x0000 (t0)
ADDIU t1, t1, 1
SW t1, 0x0000 (t0) //update calls

LUI a0, 0x800F
LW a0, 0x3FF0 (a0)
LUI at, 0x800F
SLL t6, a0, 2
ADDIU t7, t6, 0x0002
ADDIU t8, t6, 0x0003
MULTU t7, t8
MFLO a1
SRL v0, a1, 2
JR RA
SW v0, 0x3FF0 (at)

ZoneSetCheck:
LI t5, zoneLockout
LW t6, 0x0000 (t5)
BNEZ t6, skipGameZoneWrite
NOP
SH s3, 0xAE00 (at)
skipGameZoneWrite:
ADDIU t6, t6, -1
BLTZ t6, skipWrite
NOP
SW t6, 0x0000 (t5) //remove zone lockout
skipWrite:
J 0x80055124
NOP