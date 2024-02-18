//patches that jump to custom code

.org 0x8002858C
J printCustomText2
NOP

.org 0x800E0790
J guRandHook
NOP

.org 0x800550E8
J ZoneSetCheck
NOP

//try removing osGetTime calls
.org 0x800268F4
NOP
.org 0x80026760
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
