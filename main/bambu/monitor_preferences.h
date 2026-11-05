/* SPDX-License-Identifier: MIT */
#pragma once
#include "esp_err.h"
#include <stdbool.h>
#include <stdint.h>
typedef struct {
    uint8_t skin, sleep_min;
    bool keep_printing, wake_finish, wake_error;
} monitor_preferences_t;
esp_err_t monitor_preferences_init(void);
const monitor_preferences_t *monitor_preferences_get(void);
esp_err_t monitor_preferences_save(const monitor_preferences_t *prefs);
