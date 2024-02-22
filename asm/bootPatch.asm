.headersize 0x80024C00
.org 0x80025C00

//normal game entry code
LUI t0, 0x8010
LUI t1, 0x000D
ADDIU t0, t0, 0xAE50
ORI t1, t1, 0x2F50
bssClearLoop:
ADDI t1, t1, 0xFFF8
SD r0, 0x0000 (t0)
//SW r0, 0x0004 (t0)
BNEZ t1, bssClearLoop
ADDI t0, t0, 0x0008
LUI s2, 0x8002
LUI sp, 0x8010
ADDIU s2, s2, 0x5C60
ADDIU sp, sp, -0x2E50

//dma our custom stuff
LUI a0, 0
LUI a1, 0x00C0 //0xC00000 rom addr
LUI a2, 0x8040 //0x80400000 ram addr
JAL osPiRawStartDma
LUI a3, 0x0002

//wait on our dma to finish before continuing
dmaBusyLoop:
LUI t0, 0xA460
LW t1, 0x0010 (t0)
ANDI t1, t1, 0x0001
BNEZ t1, dmaBusyLoop
NOP
JR s2
NOP

.org 0x80026750
JAL BeforeMainLoop
NOP
J ReplacedGameLoopCode
NOP