#ifndef _CT2_H_
#define _CT2_H_

#include "ultra64.h"

extern u64 __osCurrentTime;
extern u32 osMemSize;
extern s16 p1ButtonsPressed;

#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof(arr[0]))

typedef volatile unsigned long		vu32;	/* unsigned 32-bit */
#define	PHYS_TO_K1(x)	((u32)(x)|0xA0000000)	/* physical to kseg1 */
#define	IO_READ(addr)		(*(vu32 *)PHYS_TO_K1(addr))

void printText(s32 arg0, s32 arg1, s32 arg2, void* arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7, f32 arg8, f32 arg9);
void startDebugThread(void);
void func_80026080(void);
void customMemCpy(void* destination, void* src, s32 size);
extern void dma_copy(void* romAddr, void* ramAddr, u32 dmaSize);
extern void crash_screen_init(void);
void osInvalICache(void*, s32);
void osInvalDCache(void*, s32);
int __osDpDeviceBusy();
int __osSpDeviceBusy();
int __osSiDeviceBusy();
void* memcpy(void* destination, const void* source, u32 size);

#define decompressBuffer 0x800E87E0

typedef struct box {
    s16 activeBool;
    s16 unk_02;
    s32 xPosStart;
    s32 yPosStart;
    s32 width;
    s32 height;
    u32 rgba;
} box;

typedef struct Vec3f {
               f32 x;
               f32 y;
               f32 z;
} Vec3f;

typedef struct CustomThread {
    /* 0x000 */ OSThread thread;
    /* 0x1B0 */ char stack[0x4100];
    /* 0x9B0 */ OSMesgQueue queue;
    /* 0x9C8 */ OSMesg mesg;
    /* 0x9CC */ u16* frameBuf;
    /* 0x9D0 */ u16 width;
    /* 0x9D2 */ u16 height;
} CustomThread; // size = 0x9D4

// typedef struct PlayerActor {
// /* 0x000 */ char unk_00[0x870];
// /* 0x870 */ Vec3f pos;
// /* 0x87C */ f32 unk_87C;
// /* 0x880 */ f32 unk_880;
// /* 0x884 */ f32 unk_884;
// /* 0x888 */ f32 speed;
// } PlayerActor;

typedef struct PlayerActor {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ f32 angle1;
    /* 0x10 */ char unk_10[8];
    /* 0x18 */ f32 magnitude; //speedNoDirection
    /* 0x1C */ char unk_1C[8];
    /* 0x24 */ f32 velocity; //speedWithDirection
} PlayerActor; //unknown size

//extern PlayerActor* gPlayerActor; //80187290

extern PlayerActor gPlayerActors[4];

extern int getStatusRegister(void); //returns status reg
extern int setStatusRegister(s32); //returns status reg

extern void test_lz4(const u8* srcData, int srcSize);
extern void* my_memcpy(void* dst, const void* src, s32 n);
extern void* my_memmove(void* dest, const void* src, s32 n);

extern f32 textWidth;
extern f32 textHeight;

extern s16 textRed;
extern s16 textBlue;
extern s16 textGreen;
extern s16 textOpacity;
extern s16 ifTextColor;
extern s16 textKerning;

extern s32 p1Airborne;
extern s16 p1IfParasol;

void decompress_lz4_ct_default(int srcSize, int savestateCompressedSize, u8* compressedBuffer);
s32 compress_lz4_ct_default(const u8* srcData, int srcSize, u8* bufferAddr);

extern int LZ4_decompress_safe_withPrefix64k(const char* source, char* dest, int compressedSize, int maxOutputSize);
extern int LZ4_decompress_safe(const char* source, char* dest, int compressedSize, int maxDecompressedSize);
extern int LZ4_compress_fast(const char* src, char* dest, int srcSize, int dstCapacity, int acceleration);
extern int LZ4_compress_default(const char* src, char* dst, int srcSize, int dstCapacity);

extern CustomThread gCustomThread;

u32 __osDisableInt(void);
void __osRestoreInt(u32 mask);
extern u32 calls;
extern u32 callsAtPowerupDecision;
extern u32 seedAtPowerup;
extern u32 callsEnteringSkyland;
extern u32 voidOutCalls;
extern s16 respawnZone;
void setDebugTextPosition(s32 xPos, s32 yPos, s32 unk);
void formatText(void* buffer, void* string);
void printDebugText(void* string);
extern s32 curStringPrintingIndex;
void drawDebugScreenText(void);
void _bzero(void* buffer, s32 size);
#define PI_BASE_REG		0x04600000

/// Linker symbol address, as in `ld_addrs.h`.
typedef u8 Addr[];

#define va_arg(AP,TYPE) (AP = (__gnuc_va_list) ((char *) (AP) + __va_rounded_size (TYPE)), *((TYPE *) (void *) ((char *) (AP) - ((sizeof (TYPE) < __va_rounded_size (char) ? sizeof (TYPE) : __va_rounded_size (TYPE))))))

/* PI DRAM address (R/W): [23:0] starting RDRAM address */
#define PI_DRAM_ADDR_REG	(PI_BASE_REG+0x00)	/* DRAM address */

/* PI pbus (cartridge) address (R/W): [31:0] starting AD16 address */
#define PI_CART_ADDR_REG	(PI_BASE_REG+0x04)

/* PI read length (R/W): [23:0] read data length */
#define PI_RD_LEN_REG		(PI_BASE_REG+0x08)

/* PI write length (R/W): [23:0] write data length */
#define PI_WR_LEN_REG		(PI_BASE_REG+0x0C)

/*
 * PI status (R): [0] DMA busy, [1] IO busy, [2], error
 *           (W): [0] reset controller (and abort current op), [1] clear intr
 */
#define PI_STATUS_REG		(PI_BASE_REG+0x10)

#define WAIT_ON_IOBUSY(stat)                                                                \
    while (stat = IO_READ(PI_STATUS_REG), stat & (PI_STATUS_IO_BUSY | PI_STATUS_DMA_BUSY))  \
        ;                                                                                   \
    (void)0

#endif