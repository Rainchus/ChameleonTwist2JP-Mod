#ifndef _CT2_H_
#define _CT2_H_

#include "ultra64.h"

/* enums */
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

/* macros */
#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof(arr[0]))

/* structs */
typedef struct CustomThread {
    /* 0x000 */ OSThread thread;
    /* 0x1B0 */ char stack[0x4100];
    /* 0x9B0 */ OSMesgQueue queue;
    /* 0x9C8 */ OSMesg mesg;
    /* 0x9CC */ u16* frameBuf;
    /* 0x9D0 */ u16 width;
    /* 0x9D2 */ u16 height;
} CustomThread; // size = 0x9D4

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

typedef struct Vec3f {
    f32 x;
    f32 y;
    f32 z;
} Vec3f;

typedef struct PlayerActor {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ f32 angle1;
    /* 0x10 */ char unk_10[8];
    /* 0x18 */ f32 magnitude; //speedNoDirection
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ char unk_20[4];
    /* 0x24 */ f32 velocity; //speedWithDirection
} PlayerActor; //unknown size

extern TextColor RedOrange;
extern TextColor Cyan;
extern TextColor Purple;
extern TextColor White;
extern TextColor Green;

/* externed symbols */
extern u16 currentlyPressedButtons;
extern u16 currentlyHeldButtons;
extern u8 D_80160648;
extern s16 textRed;
extern s16 textBlue;
extern s16 textGreen;
extern s16 textOpacity;
extern s16 ifTextColor;
extern s16 textKerning;
extern s16 textStyle;
extern s32 p1Airborne;
extern s16 p1IfParasol;
extern volatile s32 isSaveOrLoadActive;
extern f32 textWidth;
extern f32 textHeight;
extern u64 __osCurrentTime;
extern s16 debugFlag;
extern s16 gGameMode;
extern s16 respawnZone;
extern s16 gCurArea;
extern s32 currPageNo;
extern s32 currOptionNo;
extern s32 isMenuActive;
extern s32 menuCurRespawnZone;
extern u32 callsEnteringSkyland;
extern u32 calls;
extern PlayerActor gPlayerActors[4];
extern s8 toggles[];
extern u32 frame_count;
extern u32 __osVIIntrCount;

/* function signatures */
int __osDpDeviceBusy(void);
int __osSpDeviceBusy(void);
int __osSiDeviceBusy(void);
int __osPiDeviceBusy(void);
void customMemCpy(void* destination, void* src, s32 size);
void _bzero(void* buffer, s32 size);
void SetDebugTextPosition(s32 xPos, s32 yPos, s32 unk);
void _sprintf(void* destination, void* fmt, ...);
s32 _Printf(void* destination, void* fmt, ...);
void SetTextWidthAndHeight(f32 width, f32 height);
void SetTextColor(TextColor* color);
void SetTextColor2(u8 red, u8 blue, u8 green, u8 alpha);
void PrintDebugText(char*);
s32 _strlen(const u8 *s);
void SetTextGradient(s32, s32, s32, s32, s32);
void SetDefaultTextParametersWithColor(TextColor* color, s32 x, s32 y);
f32 calculate_and_update_fps(void);
void print_lag_frames(s32 x, s32 y, TextColor* color);
f32 CalculateSpeed(Vec3f* previousPosition, Vec3f* currentPosition);

//zone strings
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

#endif //_CT2_H_