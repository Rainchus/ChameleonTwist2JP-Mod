#ifndef _CT2_H_
#define _CT2_H_

#include "ultra64.h"
#include "gfx.h"

#define ALIGNED(n) __attribute__((aligned(n)))
#define ARRAY_COUNT(arr) (s32)(sizeof(arr) / sizeof(arr[0]))

extern u64 __osCurrentTime;
extern u32 osMemSize;
extern s16 p1ButtonsPressed;

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
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ char unk_20[4];
    /* 0x24 */ f32 velocity; //speedWithDirection
} PlayerActor; //unknown size

//extern PlayerActor* gPlayerActor; //80187290

extern PlayerActor gPlayerActors[4];

extern int getStatusRegister(void); //returns status reg
extern int setStatusRegister(s32); //returns status reg

extern void test_lz4(const u8* srcData, int srcSize);
//extern void* my_memcpy(void* dst, const void* src, s32 n);
//extern void* my_memmove(void* dest, const void* src, s32 n);

extern f32 textWidth;
extern f32 textHeight;

extern s16 textRed;
extern s16 textBlue;
extern s16 textGreen;
extern s16 textOpacity;
extern s16 ifTextColor;
extern s16 textKerning;
extern s16 textStyle;
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

typedef struct Vec3w {
    s32 x;
    s32 y;
    s32 z;
} Vec3w;

typedef struct TextPosition {
    s32 xPos;
    s32 yPos;
} TextPosition;

typedef struct RGBA {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} RGBA;

typedef struct TextColor {
    RGBA zero;
    RGBA one;
    RGBA two;
    RGBA three;
} TextColor;

extern TextColor RedOrange;
extern TextColor Cyan;
extern TextColor Purple;
extern TextColor White;
extern TextColor Green;
void SetTextColor(TextColor* color);
void SetTextColor2(u8 red, u8 blue, u8 green, u8 alpha);
void SetDefaultTextParametersWithColor(TextColor* color, s32 x, s32 y);
void _sprintf(void* destination, void* fmt, ...);
s32 _Printf(void* destination, void* fmt, ...);
s32 _strlen(const u8 *s);
extern f32 bossHpBar;

typedef struct ZoneData {
/* 0x00 */ void* unk0;
/* 0x04 */ Vec3w min;
/* 0x10 */ Vec3w max;
/* 0x1C */ s16 zoneID;
/* 0x1E */ s16 numOfZonesToLoad;
/* 0x20 */ s16 unk20;
/* 0x22 */ s16 unk22; //padding?
/* 0x24 */ s32 respawnX;
/* 0x28 */ s32 respawnY;
/* 0x2C */ s32 respawnZ;
} ZoneData;

typedef struct testingstuff {
    char unk_00[0x6C];
    f32 unk_6C;
    f32 unk_70;
} testingstuff;

extern testingstuff imageStuff;
extern ZoneData gZones[64]; //unsure of max

extern f32 powerUpFloatArray[8];
void setTextGradient(s32, s32, s32, s32, s32);
extern u16 currentlyPressedButtons;
extern u16 currentlyHeldButtons;

enum Toggles {
    NO_TOGGLE = 0,
    //page 0
    TOGGLE_DISPLAY_SPEED,
    TOGGLE_DISPLAY_POSITION,
    TOGGLE_POWERUP_LOCK,
    //page 1
    TOGGLE_DISPLAY_FPS,
    TOGGLE_DISPLAY_LAG_FRAMES,
    TOGGLE_DISPLAY_ZONE,
    TOGGLE_INF_HEALTH
};

enum PowerUpIDs {
    POWER_BIG = 0,
    POWER_SPEED_UP = 1,
    POWER_SPEED_DOWN = 2,
    POWER_X2 = 3,
    POWER_X3 = 4,
    POWER_INVICIBLE = 5,
    POWER_SHORT_TONGUE = 6,
    POWER_NOTHING = 7
};


extern s32 currPageNo;
extern s32 currOptionNo;
extern s32 isMenuActive;
void updateMenuInput(void);
extern s8 toggles[];
extern s16 gCurArea;




extern char* SkyLandArea0[32];
extern char* SkyLandArea1[11];

extern char* CarnivalLandArea0[13];
extern char* CarnivalLandArea1[26];
extern char* CarnivalLandArea2[19];

extern char* IceLandArea0[40];
extern char* IceLandArea1[2];
extern char* IceLandArea2[13];

extern char* EdoLandArea0[23];
extern char* EdoLandArea1[35];

extern char* ToyLandArea0[25];
extern char* ToyLandArea1[17];

extern char* PyramidLandArea0[22];
extern char* PyramidLandArea1[21];

#endif