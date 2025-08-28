.headersize 0x802032D4 - 0x20B844
.org 0x802032D4
    J beforePrintSetVariable
    NOP

.org 0x802034F4 //remove check and also store pointer for use in our C code
    LUI v0, hi(MenuInstancePointer)
    SW a1, lo(MenuInstancePointer) (v0)

.org 0x80203590 //remove Z menu from debug menu
    ORI t6, r0, 0

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

.org 0x800550E8
    J zoneSetCheck
    NOP

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

//create custom timer tick
.org 0x800B7088
    J stageTimerTick
    NOP

.org 0x800B3ED4
    J resetStageTimer
    NOP

.org 0x8002956C
    J func_8002956C_Hook
    NOP

.org 0x800D6290
    J func_800D6290_Hook
    NOP