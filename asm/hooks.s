copyCallsToPowerupCalls:
LI t7, calls
LI v0, callsAtPowerupDecision
LW t7, 0x0000 (t7)
SW t7, 0x0000 (v0)

LUI t7, 0x800F
J 0x8004FC10
ADDIU t7, t7, 0xCF40


printCustomText:
addiu sp, sp, -0x20
sw ra, 0x0018 (sp)
JAL printCustomTextInC
NOP
LW ra, 0x0018 (sp)
JR RA
addiu sp, sp, 0x20