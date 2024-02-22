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