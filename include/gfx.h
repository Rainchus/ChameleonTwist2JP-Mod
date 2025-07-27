#ifndef _GFX_H_
#define _GFX_H_

#define     GFX_SIZE 0x7500

#include "ultra64.h"

typedef struct gfx_texture {
    void *data;
    u16 tile_width;
    u16 tile_height;
    u16 x_tiles;
    u16 y_tiles;
    u32 tile_size;
    u32 img_fmt;
    u8 img_size;
} gfx_texture;

typedef struct{
    gfx_texture *texture;
    u16 c_width;
    u16 c_height;
    u16 cx_tile;
    u16 cy_tile;
} gfx_font; //sizeof 0xC

#endif //_GFX_H_