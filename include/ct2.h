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
void osInvalICache(void*, s32);
void osInvalDCache(void*, s32);
int __osDpDeviceBusy();
int __osSpDeviceBusy();
int __osSiDeviceBusy();

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


typedef struct CustomThread {
    /* 0x000 */ OSThread thread;
    /* 0x1B0 */ char stack[0x4100];
    /* 0x9B0 */ OSMesgQueue queue;
    /* 0x9C8 */ OSMesg mesg;
    /* 0x9CC */ u16* frameBuf;
    /* 0x9D0 */ u16 width;
    /* 0x9D2 */ u16 height;
} CustomThread; // size = 0x9D4

extern int getStatusRegister(void); //returns status reg
extern int setStatusRegister(s32); //returns status reg

extern void test_lz4(const u8* srcData, int srcSize);
extern void* my_memcpy(void* dst, const void* src, s32 n);
extern void* my_memmove(void* dest, const void* src, s32 n);

void decompress_lz4_ct_default(int srcSize, int savestateCompressedSize, u8* compressedBuffer);
s32 compress_lz4_ct_default(const u8* srcData, int srcSize, u8* bufferAddr);

extern int LZ4_decompress_safe_withPrefix64k(const char* source, char* dest, int compressedSize, int maxOutputSize);
extern int LZ4_decompress_safe(const char* source, char* dest, int compressedSize, int maxDecompressedSize);
extern int LZ4_compress_fast(const char* src, char* dest, int srcSize, int dstCapacity, int acceleration);
extern int LZ4_compress_default(const char* src, char* dst, int srcSize, int dstCapacity);

extern CustomThread gCustomThread;


#define PI_BASE_REG		0x04600000

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

#define PI_STATUS_DMA_BUSY  (1 << 0)
#define PI_STATUS_IO_BUSY   (1 << 1)
#define PI_STATUS_ERROR     (1 << 2)

#define WAIT_ON_IOBUSY(stat)                                                                \
    while (stat = IO_READ(PI_STATUS_REG), stat & (PI_STATUS_IO_BUSY | PI_STATUS_DMA_BUSY))  \
        ;                                                                                   \
    (void)0
