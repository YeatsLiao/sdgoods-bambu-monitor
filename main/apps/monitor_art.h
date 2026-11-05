/* SPDX-License-Identifier: MIT */
#pragma once
#include "lvgl.h"
#include <stddef.h>
typedef struct {
    uint16_t width,height;
    const uint8_t *palette; /* 256 RGB565 entries, high byte first */
    const uint8_t *runs;    /* PackBits: high-bit repeat, otherwise literal */
    uint32_t size;
} monitor_art_t;
/* Bounded lossless expansion of the stored palette indices. */
bool monitor_art_expand(const monitor_art_t *art,uint8_t *pixels,size_t capacity);
bool monitor_art_draw(lv_draw_ctx_t *ctx,const monitor_art_t *art,int x,int y,int width);
