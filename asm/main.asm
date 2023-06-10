.n64 // Let armips know we're coding for the N64 architecture
.open "rom/ct2JP.z64", "rom/ct2JP.mod.z64", 0 // Open the ROM file
.include "asm/symbols.asm" // Include mp1.asm to tell armips' linker where to find the game's function(s)
.include "asm/bootPatch.asm" //patch boot routine to DMA our code from ROM
.close // Close the ROM file