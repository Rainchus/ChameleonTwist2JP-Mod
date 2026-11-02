#include "../include/ct2.h"

/*
 * Version text on the Japan System Supply logo screen.
 *
 * The game's own debug text can't be used here: the logo screen doesn't
 * load the font texture (0x37), so printDebugText queues the characters
 * but they draw invisible. Instead, the crash screen's 6x7 bitmap font
 * is drawn as solid rectangles (one per horizontal run of lit pixels), so
 * no texture is needed.
 *
 * Called from ZoneRenderHook right after the HUD-model pass.
 * Change the number in PRACTWIST_VERSION (ct2.h).
 */

#define VERSION_TEXT      "PRACTWIST V" PRACTWIST_VERSION
#define VT_SCALE          1     // screen pixels per font pixel (1 = 6x7 glyphs, 2 = 12x14)
#define VT_GLYPH_W        6     // font cell width in pixels (5 px glyph + 1 px gap)
#define VT_GLYPH_H        7
#define VT_Y              196   // top of the text, in the 320x240 framebuffer
#define VT_GFX_MAX        700

#define LOGO_GAME_MODE    18    // game selection step
#define LOGO_SCREEN       1     // gSelectionScreen: 1 = JSS logo, 2 = title screen

extern u32 gCrashScreenFont[];
extern u8 gCrashScreencharToGlyph[];
extern Gfx* gMainGfxPosPtr;
extern s16 gGameMode;
extern s32 gSelectionScreen; // 0x800F0934, which screen the game selection step is on
void osWritebackDCache(void*, s32);

static Gfx sVersionGfx[2][VT_GFX_MAX];
static s32 sVersionBuf = 0;

// same bit layout crash_screen_draw_glyph uses
static Gfx* DrawGlyphRects(Gfx* gfx, Gfx* gfxEnd, s32 x, s32 y, s32 glyph) {
    const u32* data = &gCrashScreenFont[glyph / 5 * 7];
    s32 shift = (glyph % 5) * 6;
    s32 row, col, run;

    for (row = 0; row < VT_GLYPH_H; row++) {
        u32 rowMask = data[row];
        run = -1;
        for (col = 0; col <= VT_GLYPH_W; col++) {
            s32 lit = (col < VT_GLYPH_W) && (rowMask & (0x80000000U >> (shift + col)));
            if (lit && run < 0) {
                run = col;
            } else if (!lit && run >= 0) {
                if (gfx >= gfxEnd) {
                    return gfx;
                }
                gDPFillRectangle(gfx++, x + run * VT_SCALE, y + row * VT_SCALE,
                                 x + col * VT_SCALE, y + (row + 1) * VT_SCALE);
                run = -1;
            }
        }
    }
    return gfx;
}

void DrawVersionText(void) {
    Gfx* gfx;
    Gfx* gfxEnd;
    const char* s = VERSION_TEXT;
    s32 len = 0;
    s32 x;

    if (gGameMode != LOGO_GAME_MODE || gSelectionScreen != LOGO_SCREEN) {
        return;
    }

    while (s[len] != '\0') {
        len++;
    }
    x = (320 - len * VT_GLYPH_W * VT_SCALE) / 2;

    sVersionBuf ^= 1;
    gfx = sVersionGfx[sVersionBuf];
    gfxEnd = gfx + VT_GFX_MAX - 4;

    gDPPipeSync(gfx++);
    gDPSetCycleType(gfx++, G_CYC_1CYCLE);
    gDPSetRenderMode(gfx++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCombineMode(gfx++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, 255);

    for (; *s != '\0'; s++, x += VT_GLYPH_W * VT_SCALE) {
        u8 glyph = gCrashScreencharToGlyph[*s & 0x7F];
        if (glyph != 0xFF) {
            gfx = DrawGlyphRects(gfx, gfxEnd, x, VT_Y, glyph);
        }
    }

    gDPPipeSync(gfx++);
    gSPEndDisplayList(gfx++);

    osWritebackDCache(sVersionGfx[sVersionBuf], sizeof(sVersionGfx[0]));
    gSPDisplayList(gMainGfxPosPtr++, sVersionGfx[sVersionBuf]);
}
