copyCallsToPowerupCalls:
LI t7, calls
LI v0, callsAtPowerupDecision
LW t7, 0x0000 (t7)
SW t7, 0x0000 (v0)
//LI t7, seedAtPowerup
//LI v0, rngSeed
//LW v0, 0x0000 (v0)
//SW v0, 0x0000 (t7)
JAL 0x8003CBAC //randF (gets random float)
SW a2, 0x0034 (sp) //restore from hook
J 0x8004FBD8
NOP

func_800E0790_Hook:
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

printCustomText3:
ADDIU sp, sp, -0x20
SW ra, 0x0014 (sp)

JAL printCustomTextInC
NOP

LW ra, 0x0014 (sp)
JR RA
addiu sp, sp, 0x20


printCustomText4:
//JAL printCustomTextInC
//NOP
//JAL PrintMenuDisplays
//NOP
LUI t3, 0x800F
J 0x80029D68
LH t3, 0xAEEC (t3)





recordCallsAtVoidOut:
LI t0, voidOutCalls
LI t1, calls
LW t1, 0x0000 (t1)
SW t1, 0x0000 (t0) //store calls at void out
JAL 0x8004D410
NOP
J 0x8004E3E4
NOP

hookAt800D5C54:
LI a0, isSaveOrLoadActive
LW a1, 0x0000 (a0)
BNEZ a1, hookAt800D5C54
NOP
J 0x800D5B64
OR a0, s6, r0

hookAt80026750:
LI a0, isSaveOrLoadActive
LW a1, 0x0000 (a0)
BNEZ a1, hookAt80026750
NOP
OR a0, s2, r0
J 0x80026758
OR a1, r0, r0


hookAt800D5B64:
LI a0, isSaveOrLoadActive
LW a1, 0x0000 (a0)
BNEZ a1, hookAt800D5B64
NOP
OR a0, s6, r0
OR a1, s7, r0
JAL osRecvMesg
ADDIU a2, r0, 0x0001
J 0x800D5B70
NOP

hookAt800DE480:
LI a0, isSaveOrLoadActive
LW a1, 0x0000 (a0)
BNEZ a1, hookAt800DE480
NOP
LW t7, 0x0034 (sp)
J 0x800DE488
ADDIU a1, sp, 0x0044

hookAt800E7F98:
LI a0, isSaveOrLoadActive
LW a1, 0x0000 (a0)
BNEZ a1, hookAt800E7F98
NOP
LW a0, 0x000C (s7)
OR a1, s4, r0
JAL osRecvMesg
ADDIU a2, r0, 0x0001
J 0x800E7FA4
NOP

hookAt800D58E8:
LI a0, isSaveOrLoadActive
LW a1, 0x0000 (a0)
BNEZ a1, hookAt800D58E8
NOP
OR a0, s2, r0
OR a1, s3, r0
JAL osRecvMesg
ADDIU a2, r0, 0x0001
J 0x800D58F4
NOP

hookAt800D5CE8:
LI a0, isSaveOrLoadActive
LW a1, 0x0000 (a0)
BNEZ a1, hookAt800D5CE8
NOP
OR a0, s2, r0
OR a1, s3, r0
JAL osRecvMesg
ADDIU a2, r0, 0x0001
J 0x800D5CF4
NOP

hookAt800D6C04:
LI a0, isSaveOrLoadActive
LW a1, 0x0000 (a0)
BNEZ a1, hookAt800D6C04
NOP
OR a0, s4, r0
OR a1, s0, r0
JAL osRecvMesg
OR a2, s1, r0
J 0x800D6C10
NOP

hookAt800D6C10:
LI a0, isSaveOrLoadActive
LW a1, 0x0000 (a0)
BNEZ a1, hookAt800D6C10
NOP
LW t6, 0x0040 (sp)
LH v0, 0x0000 (t6)
J 0x800D6C18
NOP

hookAt800D4240:
LI a0, isSaveOrLoadActive
LW a1, 0x0000 (a0)
BNEZ a1, hookAt800D4240
NOP
OR a0, fp, r0
OR a1, s1, r0
JAL osRecvMesg
ADDIU a2, r0, 0x0001
J 0x800D424C
NOP

hookAt800D3FBC:
LI a0, isSaveOrLoadActive
LW a1, 0x0000 (a0)
BNEZ a1, hookAt800D3FBC
NOP
OR a0, s1, r0
OR a1, s2, r0
JAL osRecvMesg
ADDIU a2, r0, 0x0001
J 0x800D3FC8
NOP


hookAt800CBDC0:
LI a0, isSaveOrLoadActive
LW a1, 0x0000 (a0)
BNEZ a1, hookAt800CBDC0
NOP
//LI a0, isOtherThreadActive
//ORI a1, r0, 1
//SW a1, 0x0000 (a0) //prevent savestate thread from running
LUI a0, 0x801A
ADDIU a0, a0, 0xCEB0
J 0x800CBDC8
NOP

hookAt8002AB64:
LI a0, isSaveOrLoadActive
LW a1, 0x0000 (a0)
BNEZ a1, hookAt8002AB64
NOP
OR a0, s5, r0
OR a1, r0, r0
JAL osRecvMesg
ADDIU a2, r0, 0x0001
J 0x8002AB70
NOP


hookAt800D76D8:
LI a0, isSaveOrLoadActive
LW a1, 0x0000 (a0)
BNEZ a1, hookAt8002AB64
NOP
JAL 0x800E3A00
OR a0, s0, r0
J 0x800D76E0
NOP

osPiStartDmaHook:
// at 800DF584
SW s0, 0x0014 (sp)
ADDU s0, a0, r0
LW a0, 0x0038 (sp)
JAL osInvalICache
LW a1, 0x003C (sp) //sizeof DMA

LW a0, 0x0038 (sp)
JAL osInvalDCache
LW a1, 0x003C (sp) //sizeof DMA

LW a0, 0x0028 (sp)
LW a1, 0x002C (sp)
LW a2, 0x0030 (sp)
LW a3, 0x0034 (sp)
LUI t6, 0x800F
LW t6, 0x3FA0 (t6)
BNEZ t6, jumpTo800DF594
NOP
J 0x800DF58C
NOP
jumpTo800DF594:
J 0x800DF594
NOP

osEPiStartDmaHook:
// hook at 800E1590
SW s0, 0x0014 (sp)
ADDU s0, a1, r0
LW a0, 0x0008 (s0)
JAL osInvalICache
LW a1, 0x0010 (s0)

LW a0, 0x0008 (s0)
JAL osInvalDCache
LW a1, 0x0010 (s0)

LW a0, 0x0028 (sp)
LW a1, 0x002C (sp)
LW a2, 0x0030 (sp)

LUI t6, 0x800F
LW t6, 0x3FA0 (t6)
BNEZ t6, jumpTo800E15A0
NOP
J 0x800E1598
NOP

jumpTo800E15A0:
J 0x800E15A0
NOP

hookAt800A058C:
J afterBossHpSet
OR a0, v0, r0 //preserve v0

hookAt8009B66C:
J afterBossHpSet
OR a0, v0, r0 //preserve v0

hookAt8009D394:
J afterBossHpSet
OR a0, v0, r0 //preserve v0

hookAt8009E9F8:
J afterBossHpSet
OR a0, v0, r0 //preserve v0

hookAt800A6384:
J afterBossHpSet
OR a0, v0, r0 //preserve v0

//robot boss
hookAt800A3150:
J afterBossHpSet
OR a0, v0, r0 //preserve v0

//hook 0 for debug menu
//.org 0x800D6050
//J PrintDebugMenu
//ADDU a0, r0, a1

stuff:
JAL SetTimerStuff
NOP
J 0x800293B8
NOP

hookAt80029574:
JAL 0x8003CBAC
NOP
JAL func_800293F0_Hook2
NOP
J 0x8002957C
NOP

zoneSetCheck:
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



.headersize 0x802032D4 - 0x20B844
.org 0x802032D4
J beforePrintSetVariable
NOP

.org 0x802034F4 //remove check and also store pointer for use in our C code
LUI v0, hi(MenuInstancePointer)
SW a1, lo(MenuInstancePointer) (v0)

.org 0x80203590 //remove Z menu from debug menu
ORI t6, r0, 0