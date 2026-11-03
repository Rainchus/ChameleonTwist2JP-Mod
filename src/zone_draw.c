#include "../include/ct2.h"

/*
 * Zone visualizer
 *
 * Draws every respawn zone in the current area as a translucent box with
 * bright outlines, plus an optional number label that matches the
 * "Zone: XX" HUD readout and the names in zones.c.
 *
 * How the game uses zones (from the routine at 0x80054F10):
 *   - gZones is an array of 0x30-byte ZoneData entries at 0x801857F0
 *   - index 0 is unused; the search starts at index 1 and stops at index 80,
 *     at an entry whose unk0 is NULL, or at an entry whose *(s16*)unk0 == 6
 *   - min/max are truncated to s16 and swapped per axis if reversed
 *   - the player is inside when min <= pos <= max on all three axes
 *   - the first matching index wins and is written to gRespawnZone
 *
 * Rendering is injected between the world pass (0x80036E30) and the
 * orthographic HUD-model pass (0x80036CF0) by replacing the jal at
 * 0x800371D4 with ZoneRenderHook (see patches.asm). The world camera
 * matrices are captured in func_80035E00_Hook and reloaded here.
 */

#define ZONE_MAX_INDEX        80    // game loop runs while index < 0x51
#define ZONE_LIST_END_TAG     6     // *(s16*)unk0 value that ends the list
#define ZONE_SCREEN_MARGIN    1.1f  // "On Screen" mode: widen the view slightly so boxes don't pop at the edges
#define ZONE_EDGE_ZONES_MAX   16    // outlines are drawn for the N closest zones
#define ZONE_LABELS_MAX       10
#define ZONE_TEXT_RESERVE     16    // chars left free for the game's own HUD text
#define ZONE_GAMEPLAY_MODE    3     // gGameMode while playing a stage

#define ZONE_LINE_SCALE       0.0055f // outline half-width per unit of camera distance (~1.3px)
#define ZONE_LINE_MIN         2.0f
#define ZONE_TAN_HALF_FOV     0.57735027f // tan(30 deg), matches guPerspective(60 deg) in the camera hook
#define ZONE_ASPECT           1.3333334f

#define ZONE_FACE_ALPHA       0x40
#define ZONE_FACE_ALPHA_CUR   0x20  // the zone you're standing in tints the whole screen, keep it faint
#define ZONE_EDGE_ALPHA       0xE0

#define FACE_VTX_MAX  (ZONE_MAX_INDEX * 8)
#define EDGE_VTX_MAX  (ZONE_EDGE_ZONES_MAX * 48)
#define ZONE_GFX_MAX  1400

enum ZoneDrawModes {
    ZONE_DRAW_OFF = 0,
    ZONE_DRAW_ALL = 1,
    ZONE_DRAW_ON_SCREEN = 2,
    ZONE_DRAW_MODE_COUNT
};

extern Gfx* gMainGfxPosPtr;
extern s16 gGameMode;
extern u16 gTextCharsMax;
extern u16 gTextCharsUsed;
void func_80036CF0(void);
void osWritebackDCache(void*, s32);
void SetTextWidthAndHeight(f32 width, f32 height);

typedef struct ZoneCamera {
    Mtx* proj;
    Mtx* view;
    u16 perspNorm;
    s32 valid;
    f32 eye[3];
    f32 at[3];
    f32 near;
    f32 far;
} ZoneCamera;

typedef struct ZoneBox {
    s16 min[3];
    s16 max[3];
} ZoneBox;

ZoneCamera gZoneCam = { 0 };

static ZoneBox sZoneBoxes[ZONE_MAX_INDEX + 1];
static s32 sZoneCount = 0; // highest valid index
static u8 sZoneOnScreen[ZONE_MAX_INDEX + 1]; // filled once per frame by UpdateZoneVisibility

static Vtx sFaceVtx[2][FACE_VTX_MAX];
static Vtx sEdgeVtx[2][EDGE_VTX_MAX];
static Gfx sZoneGfx[2][ZONE_GFX_MAX];
static s32 sZoneBuf = 0;

static u8 sZonePalette[][3] = {
    {255,  64,  64}, // red
    { 64, 160, 255}, // blue
    {255, 220,  40}, // yellow
    { 80, 255, 120}, // green
    {255, 110, 230}, // pink
    { 60, 240, 240}, // cyan
    {255, 150,  40}, // orange
    {170, 110, 255}, // purple
    {200, 255,  60}, // lime
    {255, 255, 255}, // white
};

static inline f32 zsqrtf(f32 x) {
    f32 r;
    __asm__("sqrt.s %0, %1" : "=f"(r) : "f"(x));
    return r;
}

static inline s16 clampS16(f32 v) {
    if (v > 32767.0f) return 32767;
    if (v < -32768.0f) return -32768;
    return (s16)v;
}

const u8* ZoneColor(s32 index) {
    return sZonePalette[index % ARRAY_COUNT(sZonePalette)];
}

/* ------------------------------------------------------------------------ */
/* Camera capture (called from func_80035E00_Hook every frame)              */
/* ------------------------------------------------------------------------ */

void ZoneCaptureCamera(Mtx* proj, Mtx* view, u16 perspNorm, f32* eyeAt, f32 near, f32 far) {
    s32 i;
    gZoneCam.proj = proj;
    gZoneCam.view = view;
    gZoneCam.perspNorm = perspNorm;
    for (i = 0; i < 3; i++) {
        gZoneCam.eye[i] = eyeAt[i];
        gZoneCam.at[i] = eyeAt[i + 3];
    }
    gZoneCam.near = near;
    gZoneCam.far = far;
    gZoneCam.valid = 1;
}

/* ------------------------------------------------------------------------ */
/* Zone table                                                               */
/* ------------------------------------------------------------------------ */

static s32 IsRamPointer(void* p) {
    u32 a = (u32)p;
    return a >= 0x80000000 && a < 0x80800000;
}

// mirrors the game's own loop so what you see is what the game tests
static void ReadZoneTable(void) {
    s32 i, axis;
    sZoneCount = 0;

    for (i = 1; i <= ZONE_MAX_INDEX; i++) {
        ZoneData* z = &gZones[i];
        s32* mins;
        s32* maxs;

        if (z->unk0 == NULL || !IsRamPointer(z->unk0)) {
            break;
        }
        if (*(s16*)z->unk0 == ZONE_LIST_END_TAG) {
            break;
        }

        mins = &z->min.x;
        maxs = &z->max.x;
        for (axis = 0; axis < 3; axis++) {
            s16 a = (s16)mins[axis];
            s16 b = (s16)maxs[axis];
            if (a < b) {
                sZoneBoxes[i].min[axis] = a;
                sZoneBoxes[i].max[axis] = b;
            } else {
                sZoneBoxes[i].min[axis] = b;
                sZoneBoxes[i].max[axis] = a;
            }
        }
        sZoneCount = i;
    }
}

static s32 ZoneIsDrawn(s32 index) {
    switch (toggles[TOGGLE_DRAW_ZONES]) {
        case ZONE_DRAW_ALL:
            return 1;
        case ZONE_DRAW_ON_SCREEN:
            return sZoneOnScreen[index];
    }
    return 0;
}

/* ------------------------------------------------------------------------ */
/* View frustum                                                             */
/* ------------------------------------------------------------------------ */

// camera basis: f = forward, s = right, u = up (same as guLookAt with up = +Y)
typedef struct ZoneCamBasis {
    f32 f[3];
    f32 s[3];
    f32 u[3];
} ZoneCamBasis;

static s32 ZoneGetCamBasis(ZoneCamBasis* cb) {
    f32 len;
    s32 i;

    for (i = 0; i < 3; i++) {
        cb->f[i] = gZoneCam.at[i] - gZoneCam.eye[i];
    }
    len = zsqrtf(cb->f[0] * cb->f[0] + cb->f[1] * cb->f[1] + cb->f[2] * cb->f[2]);
    if (len < 0.001f) return 0;
    for (i = 0; i < 3; i++) cb->f[i] /= len;

    // s = f x up(0,1,0)
    cb->s[0] = -cb->f[2];
    cb->s[1] = 0.0f;
    cb->s[2] = cb->f[0];
    len = zsqrtf(cb->s[0] * cb->s[0] + cb->s[2] * cb->s[2]);
    if (len < 0.001f) return 0;
    cb->s[0] /= len;
    cb->s[2] /= len;

    // u = s x f
    cb->u[0] = cb->s[1] * cb->f[2] - cb->s[2] * cb->f[1];
    cb->u[1] = cb->s[2] * cb->f[0] - cb->s[0] * cb->f[2];
    cb->u[2] = cb->s[0] * cb->f[1] - cb->s[1] * cb->f[0];
    return 1;
}

// a box is off screen when all 8 corners are outside the same side of the view
// (left, right, top, bottom, near or far). Anything else counts as visible.
static s32 BoxOnScreen(ZoneBox* b, ZoneCamBasis* cb) {
    f32 tx = ZONE_TAN_HALF_FOV * ZONE_ASPECT * ZONE_SCREEN_MARGIN;
    f32 ty = ZONE_TAN_HALF_FOV * ZONE_SCREEN_MARGIN;
    s32 outLeft = 0, outRight = 0, outDown = 0, outUp = 0, outNear = 0, outFar = 0;
    s32 c, i;

    for (c = 0; c < 8; c++) {
        f32 d[3], xv, yv, depth;
        d[0] = ((c & 1) ? b->max[0] : b->min[0]) - gZoneCam.eye[0];
        d[1] = ((c & 2) ? b->max[1] : b->min[1]) - gZoneCam.eye[1];
        d[2] = ((c & 4) ? b->max[2] : b->min[2]) - gZoneCam.eye[2];
        depth = xv = yv = 0.0f;
        for (i = 0; i < 3; i++) {
            depth += d[i] * cb->f[i];
            xv += d[i] * cb->s[i];
            yv += d[i] * cb->u[i];
        }
        if (xv < -depth * tx) outLeft++;
        if (xv > depth * tx) outRight++;
        if (yv < -depth * ty) outDown++;
        if (yv > depth * ty) outUp++;
        if (depth < gZoneCam.near) outNear++;
        if (depth > gZoneCam.far) outFar++;
    }
    return !(outLeft == 8 || outRight == 8 || outDown == 8 || outUp == 8 || outNear == 8 || outFar == 8);
}

static void UpdateZoneVisibility(void) {
    ZoneCamBasis cb;
    s32 i;
    s32 haveBasis = ZoneGetCamBasis(&cb);

    for (i = 1; i <= sZoneCount; i++) {
        sZoneOnScreen[i] = haveBasis ? BoxOnScreen(&sZoneBoxes[i], &cb) : 1;
    }
}

static s32 ZoneDrawingActive(void) {
    if (toggles[TOGGLE_DRAW_ZONES] == ZONE_DRAW_OFF && !toggles[TOGGLE_ZONE_LABELS]) {
        return 0;
    }
    return gGameMode == ZONE_GAMEPLAY_MODE && gZoneCam.valid;
}

// squared distance from the camera to the closest point of a box
static f32 BoxDistSq(ZoneBox* b) {
    f32 d = 0.0f;
    s32 axis;
    for (axis = 0; axis < 3; axis++) {
        f32 p = gZoneCam.eye[axis];
        f32 t = 0.0f;
        if (p < b->min[axis]) t = b->min[axis] - p;
        else if (p > b->max[axis]) t = p - b->max[axis];
        d += t * t;
    }
    return d;
}

/* ------------------------------------------------------------------------ */
/* Geometry                                                                 */
/* ------------------------------------------------------------------------ */

static void SetVtx(Vtx* v, f32 x, f32 y, f32 z, u8 shade) {
    v->v.ob[0] = clampS16(x);
    v->v.ob[1] = clampS16(y);
    v->v.ob[2] = clampS16(z);
    v->v.flag = 0;
    v->v.tc[0] = 0;
    v->v.tc[1] = 0;
    v->v.cn[0] = shade;
    v->v.cn[1] = shade;
    v->v.cn[2] = shade;
    v->v.cn[3] = 0xFF;
}

// corner c: bit0 = x max, bit1 = y max, bit2 = z max
static void BoxCorner(ZoneBox* b, s32 c, f32* out) {
    out[0] = (c & 1) ? b->max[0] : b->min[0];
    out[1] = (c & 2) ? b->max[1] : b->min[1];
    out[2] = (c & 4) ? b->max[2] : b->min[2];
}

static void BuildFaceVtx(Vtx* v, ZoneBox* b) {
    s32 c;
    f32 p[3];
    for (c = 0; c < 8; c++) {
        BoxCorner(b, c, p);
        // top corners lighter than bottom ones so the box reads as 3D
        SetVtx(&v[c], p[0], p[1], p[2], (c & 2) ? 0xFF : 0x90);
    }
}

// a quad facing the camera along an axis-aligned edge; width scales with
// distance so the outline stays roughly the same thickness on screen
static s32 BuildEdgeQuad(Vtx* v, f32* a, f32* b, s32 axis) {
    f32* ends[2];
    f32 off[2][3];
    s32 e, i;

    ends[0] = a;
    ends[1] = b;

    for (e = 0; e < 2; e++) {
        f32 d[3], perp[3], len, dist, hw;
        for (i = 0; i < 3; i++) {
            d[i] = ends[e][i] - gZoneCam.eye[i];
        }
        switch (axis) { // cross(axisDir, d)
            case 0:  perp[0] = 0.0f;  perp[1] = -d[2]; perp[2] = d[1];  break;
            case 1:  perp[0] = d[2];  perp[1] = 0.0f;  perp[2] = -d[0]; break;
            default: perp[0] = -d[1]; perp[1] = d[0];  perp[2] = 0.0f;  break;
        }
        len = zsqrtf(perp[0] * perp[0] + perp[1] * perp[1] + perp[2] * perp[2]);
        if (len < 0.01f) {
            return 0; // looking straight down the edge
        }
        dist = zsqrtf(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
        hw = dist * ZONE_LINE_SCALE;
        if (hw < ZONE_LINE_MIN) {
            hw = ZONE_LINE_MIN;
        }
        for (i = 0; i < 3; i++) {
            off[e][i] = perp[i] / len * hw;
        }
    }

    SetVtx(&v[0], a[0] + off[0][0], a[1] + off[0][1], a[2] + off[0][2], 0xFF);
    SetVtx(&v[1], a[0] - off[0][0], a[1] - off[0][1], a[2] - off[0][2], 0xFF);
    SetVtx(&v[2], b[0] - off[1][0], b[1] - off[1][1], b[2] - off[1][2], 0xFF);
    SetVtx(&v[3], b[0] + off[1][0], b[1] + off[1][1], b[2] + off[1][2], 0xFF);
    return 1;
}

// 12 edges as (corner, corner, axis)
static u8 sBoxEdges[12][3] = {
    {0, 1, 0}, {2, 3, 0}, {4, 5, 0}, {6, 7, 0},
    {0, 2, 1}, {1, 3, 1}, {4, 6, 1}, {5, 7, 1},
    {0, 4, 2}, {1, 5, 2}, {2, 6, 2}, {3, 7, 2},
};

// returns number of verts written (multiple of 4, max 48)
static s32 BuildEdgeVtx(Vtx* v, ZoneBox* b) {
    s32 i, n = 0;
    f32 pa[3], pb[3];
    for (i = 0; i < 12; i++) {
        BoxCorner(b, sBoxEdges[i][0], pa);
        BoxCorner(b, sBoxEdges[i][1], pb);
        if (BuildEdgeQuad(&v[n], pa, pb, sBoxEdges[i][2])) {
            n += 4;
        }
    }
    return n;
}

/* ------------------------------------------------------------------------ */
/* Display list                                                             */
/* ------------------------------------------------------------------------ */

#define QUAD(a, b, c, d) gSP2Triangles(gfx++, a, b, c, 0, a, c, d, 0)

static Gfx* EmitFaces(Gfx* gfx, Vtx* v, s32 index, s32 isCurrent) {
    const u8* col = ZoneColor(index);
    gDPSetPrimColor(gfx++, 0, 0, col[0], col[1], col[2], isCurrent ? ZONE_FACE_ALPHA_CUR : ZONE_FACE_ALPHA);
    gSPVertex(gfx++, v, 8, 0);
    QUAD(0, 2, 6, 4); // x min
    QUAD(1, 3, 7, 5); // x max
    QUAD(0, 1, 5, 4); // y min
    QUAD(2, 3, 7, 6); // y max
    QUAD(0, 1, 3, 2); // z min
    QUAD(4, 5, 7, 6); // z max
    return gfx;
}

static Gfx* EmitEdges(Gfx* gfx, Vtx* v, s32 count, s32 index, s32 isCurrent) {
    const u8* col = ZoneColor(index);
    s32 base, i;

    if (isCurrent) {
        gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, 0xFF);
    } else {
        gDPSetPrimColor(gfx++, 0, 0, col[0], col[1], col[2], ZONE_EDGE_ALPHA);
    }

    // F3DEX2 holds 32 verts, so load 24 (6 quads) at a time
    for (base = 0; base < count; base += 24) {
        s32 n = count - base;
        if (n > 24) {
            n = 24;
        }
        gSPVertex(gfx++, &v[base], n, 0);
        for (i = 0; i < n; i += 4) {
            QUAD(i, i + 1, i + 2, i + 3);
        }
    }
    return gfx;
}

static void DrawZonesBuild(void) {
    Gfx* gfx;
    Gfx* gfxEnd;
    Vtx* faceVtx;
    Vtx* edgeVtx;
    s32 faceUsed = 0;
    s32 edgeUsed = 0;
    s32 cur = gRespawnZone;
    s32 i, k;
    s32 edgeIdx[ZONE_EDGE_ZONES_MAX];
    f32 edgeDist[ZONE_EDGE_ZONES_MAX];
    s32 edgeCount = 0;

    sZoneBuf ^= 1;
    gfx = sZoneGfx[sZoneBuf];
    gfxEnd = gfx + ZONE_GFX_MAX - 16; // room for the closing commands
    faceVtx = sFaceVtx[sZoneBuf];
    edgeVtx = sEdgeVtx[sZoneBuf];

    // pick which zones get outlines: the current zone, then the closest ones
    for (i = 1; i <= sZoneCount; i++) {
        f32 d;
        if (!ZoneIsDrawn(i)) {
            continue;
        }
        d = (i == cur) ? -1.0f : BoxDistSq(&sZoneBoxes[i]);
        if (edgeCount < ZONE_EDGE_ZONES_MAX) {
            edgeIdx[edgeCount] = i;
            edgeDist[edgeCount] = d;
            edgeCount++;
        } else {
            s32 worst = 0;
            for (k = 1; k < edgeCount; k++) {
                if (edgeDist[k] > edgeDist[worst]) worst = k;
            }
            if (d < edgeDist[worst]) {
                edgeIdx[worst] = i;
                edgeDist[worst] = d;
            }
        }
    }

    gDPPipeSync(gfx++);
    gSPPerspNormalize(gfx++, gZoneCam.perspNorm);
    gSPMatrix(gfx++, gZoneCam.proj, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(gfx++, gZoneCam.view, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetCycleType(gfx++, G_CYC_1CYCLE);
    gDPSetRenderMode(gfx++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
    gDPSetCombineLERP(gfx++, PRIMITIVE, 0, SHADE, 0, 0, 0, 0, PRIMITIVE,
                             PRIMITIVE, 0, SHADE, 0, 0, 0, 0, PRIMITIVE);
    gSPTexture(gfx++, 0, 0, 0, G_TX_RENDERTILE, G_OFF);
    gSPLoadGeometryMode(gfx++, G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH); // no culling: see boxes from inside too

    // translucent faces
    for (i = 1; i <= sZoneCount; i++) {
        if (!ZoneIsDrawn(i) || gfx >= gfxEnd - 8 || faceUsed + 8 > FACE_VTX_MAX) {
            continue;
        }
        BuildFaceVtx(&faceVtx[faceUsed], &sZoneBoxes[i]);
        gfx = EmitFaces(gfx, &faceVtx[faceUsed], i, i == cur);
        faceUsed += 8;
    }

    // outlines on top
    for (k = 0; k < edgeCount; k++) {
        s32 n;
        if (gfx >= gfxEnd - 20 || edgeUsed + 48 > EDGE_VTX_MAX) {
            break;
        }
        n = BuildEdgeVtx(&edgeVtx[edgeUsed], &sZoneBoxes[edgeIdx[k]]);
        if (n == 0) {
            continue;
        }
        gfx = EmitEdges(gfx, &edgeVtx[edgeUsed], n, edgeIdx[k], edgeIdx[k] == cur);
        edgeUsed += n;
    }

    // hand back the state the HUD-model pass expects
    gDPPipeSync(gfx++);
    gSPLoadGeometryMode(gfx++, G_ZBUFFER | G_SHADE | G_CULL_BACK);
    gSPEndDisplayList(gfx++);

    osWritebackDCache(sZoneGfx[sZoneBuf], sizeof(sZoneGfx[0]));
    osWritebackDCache(faceVtx, faceUsed * sizeof(Vtx));
    osWritebackDCache(edgeVtx, edgeUsed * sizeof(Vtx));

    gSPDisplayList(gMainGfxPosPtr++, sZoneGfx[sZoneBuf]);
}

// replaces the jal to func_80036CF0 at 0x800371D4 (between world and HUD-model passes)
void ZoneRenderHook(void) {
    if (ZoneDrawingActive()) {
        ReadZoneTable();
        UpdateZoneVisibility();
        if (toggles[TOGGLE_DRAW_ZONES] != ZONE_DRAW_OFF && sZoneCount > 0) {
            DrawZonesBuild();
        }
    }
    gZoneCam.valid = 0;
    func_80036CF0();
    DrawVersionText(); //only draws on the Japan System Supply logo
}

/* ------------------------------------------------------------------------ */
/* Labels (called from the text hook)                                       */
/* ------------------------------------------------------------------------ */

// world -> 320x240 screen; returns 0 if behind the camera
static s32 ZoneProject(f32* p, f32* sx, f32* sy, f32* depthOut) {
    ZoneCamBasis cb;
    f32 d[3], xv, yv, depth;
    s32 i;

    if (!ZoneGetCamBasis(&cb)) return 0;
    depth = xv = yv = 0.0f;
    for (i = 0; i < 3; i++) {
        d[i] = p[i] - gZoneCam.eye[i];
        depth += d[i] * cb.f[i];
        xv += d[i] * cb.s[i];
        yv += d[i] * cb.u[i];
    }
    if (depth < gZoneCam.near || depth > gZoneCam.far) return 0;

    *sx = 160.0f + (xv / (depth * ZONE_TAN_HALF_FOV * ZONE_ASPECT)) * 160.0f;
    *sy = 120.0f - (yv / (depth * ZONE_TAN_HALF_FOV)) * 120.0f;
    *depthOut = depth;
    return 1;
}

void DrawZoneLabels(void) {
    s32 i, k, n = 0;
    s32 idx[ZONE_LABELS_MAX];
    f32 xs[ZONE_LABELS_MAX], ys[ZONE_LABELS_MAX], ds[ZONE_LABELS_MAX];
    s32 cur = gRespawnZone;
    char buf[8];

    if (!toggles[TOGGLE_ZONE_LABELS] || gGameMode != ZONE_GAMEPLAY_MODE || sZoneCount <= 0) {
        return;
    }

    for (i = 1; i <= sZoneCount; i++) {
        ZoneBox* b = &sZoneBoxes[i];
        f32 c[3], sx, sy, depth;

        // labels follow the draw mode; with drawing off, label every zone
        if (toggles[TOGGLE_DRAW_ZONES] != ZONE_DRAW_OFF && !ZoneIsDrawn(i)) {
            continue;
        }

        c[0] = (b->min[0] + b->max[0]) * 0.5f;
        c[1] = (b->min[1] + b->max[1]) * 0.5f;
        c[2] = (b->min[2] + b->max[2]) * 0.5f;
        if (!ZoneProject(c, &sx, &sy, &depth)) continue;
        if (sx < 8.0f || sx > 300.0f || sy < 12.0f || sy > 228.0f) continue;
        if (i == cur) depth = 0.0f; // always label the zone you're in

        // keep the closest ZONE_LABELS_MAX
        if (n < ZONE_LABELS_MAX) {
            k = n++;
        } else {
            s32 worst = 0;
            for (k = 1; k < n; k++) {
                if (ds[k] > ds[worst]) worst = k;
            }
            if (depth >= ds[worst]) continue;
            k = worst;
        }
        idx[k] = i;
        xs[k] = sx;
        ys[k] = sy;
        ds[k] = depth;
    }

    for (k = 0; k < n; k++) {
        s32 len;
        const u8* col = ZoneColor(idx[k]);

        _bzero(buf, sizeof(buf));
        _sprintf(buf, "%d", idx[k]);
        len = _strlen((u8*)buf);

        // don't starve the game's own text of character slots
        if (gTextCharsUsed + len + ZONE_TEXT_RESERVE > gTextCharsMax) {
            break;
        }

        textStyle = 1;
        textKerning = 1;
        setDebugTextPosition((s32)xs[k] - len * 4, (s32)ys[k] - 6, 0x32);
        SetTextWidthAndHeight(0.7f, 0.7f);
        if (idx[k] == cur) {
            SetTextColor(&White);
        } else {
            TextColor tc;
            tc.zero.r = col[0]; tc.zero.g = col[1]; tc.zero.b = col[2]; tc.zero.a = 0xFF;
            tc.one = tc.zero;
            tc.two = tc.zero;
            tc.three = tc.zero;
            SetTextColor(&tc);
        }
        printDebugText(buf);
    }
}

/* ------------------------------------------------------------------------ */
/* Menu                                                                     */
/* ------------------------------------------------------------------------ */

void ZoneSanitizeToggles(void) {
    if ((u8)toggles[TOGGLE_DRAW_ZONES] >= ZONE_DRAW_MODE_COUNT) {
        toggles[TOGGLE_DRAW_ZONES] = ZONE_DRAW_OFF;
    }
    if ((u8)toggles[TOGGLE_ZONE_LABELS] > 1) {
        toggles[TOGGLE_ZONE_LABELS] = 0;
    }
}

s32 toggleDrawZones(void) {
    toggles[TOGGLE_DRAW_ZONES]++;
    if (toggles[TOGGLE_DRAW_ZONES] >= ZONE_DRAW_MODE_COUNT) {
        toggles[TOGGLE_DRAW_ZONES] = ZONE_DRAW_OFF;
    }
    return 1;
}

s32 toggleZoneLabels(void) {
    toggles[TOGGLE_ZONE_LABELS] ^= 1;
    return 1;
}
