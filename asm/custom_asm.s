copyCallsToPowerupCalls:
LI t7, calls
LI v0, callsAtPowerupDecision
LW t7, 0x0000 (t7)
SW t7, 0x0000 (v0)
//LI t7, gSeedAtPowerup
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
LI t0, gVoidOutCalls
LI t1, calls
LW t1, 0x0000 (t1)
SW t1, 0x0000 (t0) //store calls at void out
LI t0, gDoingVoidOut
ORI t1, r0, 1
SW t1, 0x0000 (t0)
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
    //s3 holds current zone
    LI t5, gCurZone
    LW t6, 0x000 (t5)
    BNE s3, t6, newZone
    NOP
    afterNewZoneCheck:
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

    newZone:
    SW s3, 0x0000 (t5)
    LI t5, gZoneChange
    ORI t6, r0, 1
    J afterNewZoneCheck
    SW t6, 0x0000 (t5)



copiedHookCode:
    //gInputMode

    LI a0, D_8019CFC0
    JAL osContStartReadData
    NOP
    LI a0, D_8019CFC0
    ADDU a1, r0, r0
    JAL osRecvMesg
    ADDIU a2, r0, 1

    JAL func_800D498C
    NOP
    LI a0, 0x801C1BD0
    JAL osContGetReadData
    NOP
    LI s5, 0x801C1BD0
    LHU s6, 0x0000 (s5) //inputs pressed on boot

    JAL func_800D49B8
    NOP
    ORI t0, r0, 0x0010 //R button
    BEQ t0, s6, setRecordingState
    NOP

    ORI t0, r0, 0x0020 //L button
    BEQ t0, s6, setPlaybackState
    NOP

    ORI t0, r0, 0x2000 //Z button
    BEQ t0, s6, setAppendState
    NOP




    afterInputCheck:
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
    moreReplacedCode:
    OR a0, s2, r0
    OR a1, r0, r0
    JAL osRecvMesg
    ADDIU a2, r0, 1
    JAL 0x800E2CF0
    NOP
    SW v0, 0x0000 (s0)
    J 0x80026770
    SW v1, 0x0004 (s0)

    setRecordingState:
    LI t0, gInputMode
    ORI t1, r0, 1
    SW t1, 0x0000 (t0) //set gInputMode to recording
    J afterInputCheck
    NOP

    setPlaybackState:
    LI t0, gInputMode
    ORI t1, r0, 2
    SW t1, 0x0000 (t0) //set gInputMode to playback
    J afterInputCheck
    NOP

    setAppendState:
    LI t0, gInputMode
    ORI t1, r0, 3
    SW t1, 0x0000 (t0) //set gInputMode to append
    J afterInputCheck
    NOP    

perFrameASM:
JAL perFrameCFunction
NOP
LUI t6, 0x800F
J 0x80029618
LHU t6, 0xAF60 (t6)

//80026770



//0x800D22E0
//0xAD6E0

//.org 0x800D22E0
//.importobj "obj/dmaCopy.o"

//.org 0x800988C0
//store at 0x80400000

//80400000
//0x7C1400

//DLWriteHookAsm:
//JAL DLWriteHook
//NOP
//J 0x800267A8
//NOP

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

getStatusRegister:
MFC0 v0, $12
NOP
NOP
JR RA
NOP

setStatusRegister:
MTC0 a0, $12
NOP
NOP
JR RA
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

stageTimerTick:
    JAL TimerTickMainC
    NOP
    LUI t1, 0x8016
    LW t1, 0x07FC (t1)
    J 0x800B7090
    NOP

resetStageTimer:
    JAL TimerResetMainC
    NOP
    LW a0, 0x0018 (sp)
    J 0x800B3EDC
    ADDIU v0, r0, 0x0001

checkFramebufferUpdate:
    LI t0, g_freeze_frames
    LW t1, 0x0000 (t0)
    ADDIU t1, t1, -1
    BLEZL t1, setZero
    ADDU t1, r0, r0
    setZero:
    SW t1, 0x0000 (t0) //set new g_freeze_frames


    BNEZ t1, skipFrameBufferUpdate
     NOP
    //otherwise, framebuffer updates
    JAL 0x800D4644
     NOP
    LUI a0, 0x800F
    JAL 0x800262E8
     LW a0, 0xAD60 (a0)
    skipFrameBufferUpdate:
    J 0x800265E8
     NOP
