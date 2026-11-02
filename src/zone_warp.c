#include "../include/ct2.h"

/*
 * Warp To Zone
 *
 * Puts the player on a zone's respawn point the same way the game's own
 * void-out respawn does (func_8004E3BC, around 0x8004E3E4-0x8004E544),
 * minus the HP loss and the void-out actor's state changes.
 *
 * The menu only queues the warp (ZoneWarpRequest). It's carried out from
 * perFrameCFunction so object resets happen during game logic, not while
 * text is being drawn.
 */

#define ZONE_WARP_MAX_INDEX     80
#define ZONE_WARP_LIST_END_TAG  6
#define ZONE_WARP_GAMEPLAY_MODE 3

// player fields (offsets from the player struct at 0x80187290)
#define PLR_UNK844          0x844
#define PLR_POS             0x870
#define PLR_VEL_88C         0x88C   // 0x88C/0x890/0x894 zeroed on respawn
#define PLR_UNK8D0          0x8D0
#define PLR_STATE           0x92E   // 1 = normal, 5 = just respawned (blinking)
#define PLR_SCALE_940       0x940
#define PLR_UNK944          0x944
#define PLR_UNK946          0x946
#define PLR_RESPAWN_TIMER   0x94E   // frames left in the post-respawn state
#define PLR_UNK952          0x952
#define PLR_VEL_958         0x958   // 0x958/0x95C/0x960 zeroed on respawn

#define CAM_EYE             0x48
#define CAM_AT              0x54

extern s16 gGameMode;
extern u8 gPlayerRaw[];      // 0x80187290, start of the player struct
extern Vec3f gPlayerPosCopy; // 0x80184E28, game keeps a copy of the player position here
extern u8 gCameraCtrl[];     // 0x80162E20, camera controller (eye at +0x48, look-at at +0x54)
extern u8 imageStuffRaw[];   // 0x80187C10, player model scale at +0x5C/+0x60/+0x64

void func_8005564C(void);    // despawns zone objects and clears every zone's "loaded" flag
void func_8003C9DC(void);    // clears a list of active effects/projectiles
void func_800B4B04(void);    // copies the player position into 0x800F0040 and zeroes 0x800F0034

extern u32 gZoneChange;

static s32 sZoneWarpPending = 0;
static s32 sZoneWarpTarget = 0;

#define PLR_F32(off) (*(f32*)(gPlayerRaw + (off)))
#define PLR_S32(off) (*(s32*)(gPlayerRaw + (off)))
#define PLR_S16(off) (*(s16*)(gPlayerRaw + (off)))

// the target must exist in this area's zone table (same rules as the game's zone check)
static s32 ZoneWarpTargetValid(s32 target) {
    s32 i;
    u32 p;

    if (target < 1 || target > ZONE_WARP_MAX_INDEX) {
        return 0;
    }
    for (i = 1; i <= target; i++) {
        p = (u32)gZones[i].unk0;
        if (p < 0x80000000 || p >= 0x80800000) {
            return 0;
        }
        if (*(s16*)p == ZONE_WARP_LIST_END_TAG) {
            return 0;
        }
    }
    return 1;
}

// called from the menu
s32 ZoneWarpRequest(s32 target) {
    sZoneWarpTarget = target;
    sZoneWarpPending = 1;
    return 1;
}

// called every frame from perFrameCFunction
void ZoneWarpUpdate(void) {
    ZoneData* zone;
    f32 x, y, z;
    s32 i;

    if (!sZoneWarpPending) {
        return;
    }
    sZoneWarpPending = 0;

    if (gGameMode != ZONE_WARP_GAMEPLAY_MODE || !ZoneWarpTargetValid(sZoneWarpTarget)) {
        return;
    }

    // the game skips this reset in areas 0x15-0x1A
    if (gCurArea < 0x15 || gCurArea >= 0x1B) {
        func_8005564C();
        func_8003C9DC();
    }

    gRespawnZone = sZoneWarpTarget;
    zone = &gZones[sZoneWarpTarget];
    x = (f32)zone->respawnX;
    y = (f32)zone->respawnY;
    z = (f32)zone->respawnZ;

    PLR_S32(PLR_UNK844) = 0;
    PLR_S32(PLR_UNK8D0) = 0;
    PLR_S16(PLR_UNK946) = 0;
    PLR_S16(PLR_UNK944) = 0;
    PLR_S16(PLR_UNK952) = 0;

    PLR_F32(PLR_POS + 0) = x;
    PLR_F32(PLR_POS + 4) = y;
    PLR_F32(PLR_POS + 8) = z;
    gPlayerPosCopy.x = x;
    gPlayerPosCopy.y = y;
    gPlayerPosCopy.z = z;

    // camera snaps to the same spot a void respawn uses
    *(f32*)(gCameraCtrl + CAM_AT + 0) = x;
    *(f32*)(gCameraCtrl + CAM_AT + 4) = y;
    *(f32*)(gCameraCtrl + CAM_AT + 8) = z;
    *(f32*)(gCameraCtrl + CAM_EYE + 0) = x;
    *(f32*)(gCameraCtrl + CAM_EYE + 4) = (f32)(zone->respawnY + 1000);
    *(f32*)(gCameraCtrl + CAM_EYE + 8) = (f32)(zone->respawnZ + 1000);

    for (i = 0; i < 3; i++) {
        PLR_F32(PLR_VEL_958 + i * 4) = 0.0f;
        PLR_F32(PLR_VEL_88C + i * 4) = 0.0f;
        *(f32*)(imageStuffRaw + 0x5C + i * 4) = 1.0f;
    }
    PLR_F32(PLR_SCALE_940) = 1.0f;

    // same post-respawn state a void out gives (blinking, 90 frames)
    PLR_S16(PLR_STATE) = 5;
    PLR_S16(PLR_RESPAWN_TIMER) = 90;
    func_800B4B04();

    gZoneChange = 1;
}
