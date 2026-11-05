/* SPDX-License-Identifier: MIT */
#pragma once
#include "lvgl.h"
#include <stdbool.h>
typedef struct {int unused;} sdgoods_tap_ctx_t;
typedef void (*sdgoods_tap_cb_t)(void *);
bool sdg_i18n_is_zh(void);
void sdg_i18n_toggle(void);
#define SDG_T(zh,en) (sdg_i18n_is_zh()?(zh):(en))
void sdgoods_lcd_set_backlight(int n);
void sdgoods_app_shell_bind(lv_obj_t *obj);
lv_indev_state_t sdgoods_touch_get_state(void);
void sdgoods_touch_get_point(lv_point_t *pt);
