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

printCustomText:
addiu sp, sp, -0x20
sw ra, 0x0018 (sp)
JAL printCustomTextInC
NOP
LW ra, 0x0018 (sp)
JR RA
addiu sp, sp, 0x20

recordCallsAtVoidOut:
LI t0, voidOutCalls
LI t1, calls
LW t1, 0x0000 (t1)
SW t1, 0x0000 (t0) //store calls at void out
JAL 0x8004D410
NOP
J 0x8004E3E4
NOP

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


