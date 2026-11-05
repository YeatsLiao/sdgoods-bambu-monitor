/* SPDX-License-Identifier: MIT */
#pragma once
#include "bambu_state.h"
#include "esp_err.h"
typedef struct {
    bool provisioning, busy, connected;
    esp_err_t error;
    char ap_ssid[32];
} monitor_service_status_t;
esp_err_t monitor_service_init(void);
bool monitor_service_provision(bool enable);
void monitor_service_status(monitor_service_status_t *out);
