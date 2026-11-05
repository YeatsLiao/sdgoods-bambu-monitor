/* SPDX-License-Identifier: MIT */
#pragma once
#include "lvgl.h"
typedef struct {
    const char *zh, *en;
    uint32_t bg,ink,accent,card;
} monitor_skin_t;
const monitor_skin_t *monitor_skin_get(int id);
bool monitor_skin_has_art(int id);
void monitor_skin_decor(lv_draw_ctx_t *ctx,int id);
void monitor_skin_art(lv_draw_ctx_t *ctx,int id);
void monitor_skin_scene(lv_draw_ctx_t *ctx,int id,int x,int y,int width);
void monitor_skin_art_at(lv_draw_ctx_t *ctx,int id,int x,int y,int width);

void monitor_skin_header(lv_draw_ctx_t *ctx,int id,int x,int y,int width);
