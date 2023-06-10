#include "types.h"
#include "OS.h"

#define A_BUTTON 0x8000
#define B_BUTTON 0x4000
#define Z_BUTTON 0x2000
#define START 0x1000
#define D_UP 0x0800
#define D_DOWN 0x0400
#define D_LEFT 0x0200
#define D_RIGHT 0x0100
#define L_BUTTON 0x0020
#define R_BUTTON 0x0010

extern u64 __osCurrentTime;
extern u32 osMemSize;
extern s16 p1ButtonsPressed;

typedef volatile unsigned long		vu32;	/* unsigned 32-bit */
#define	PHYS_TO_K1(x)	((u32)(x)|0xA0000000)	/* physical to kseg1 */
#define	IO_READ(addr)		(*(vu32 *)PHYS_TO_K1(addr))

void startDebugThread(void);
void func_80026080(void);
void customMemCpy(void* destination, void* src, s32 size);
extern void dma_copy(void* romAddr, void* ramAddr, u32 dmaSize);
extern void crash_screen_init(void);

typedef struct box {
    s16 activeBool;
    s16 unk_02;
    s32 xPosStart;
    s32 yPosStart;
    s32 width;
    s32 height;
    u32 rgba;
} box;


typedef struct CustomThread {
    /* 0x000 */ OSThread thread;
    /* 0x1B0 */ char stack[0x4100];
    /* 0x9B0 */ OSMesgQueue queue;
    /* 0x9C8 */ OSMesg mesg;
    /* 0x9CC */ u16* frameBuf;
    /* 0x9D0 */ u16 width;
    /* 0x9D2 */ u16 height;
} CustomThread; // size = 0x9D4

extern CustomThread gCustomThread;