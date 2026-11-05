/*
 * Application assembly adapted from SDGOODS-ESP32S3 / sdgoods-radio.
 * Copyright (c) 2026 SDGOODS (深圳希德创新网络有限公司)
 * SPDX-License-Identifier: Apache-2.0
 * Required Notice: retain NOTICE and LICENSING.md with redistribution.
 */
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "sdgoods_board.h"
#include "sdgoods_cc.h"
#include "build_version.h"
#ifndef CONFIG_BAMBU_BASELINE
#include "monitor_service.h"
#include "monitor_preferences.h"
#include "ui_monitor.h"
#endif

static void baseline_show(void) {
    static lv_obj_t *screen;
    if (!screen) {
        screen = lv_obj_create(NULL);
        lv_obj_t *label = lv_label_create(screen);
        lv_label_set_text(label, "BambuMonitor\nPlatform baseline");
        lv_obj_center(label);
    }
    lv_scr_load(screen);
    sdgoods_app_shell_bind(screen);
}
void app_main(void) {
    ESP_LOGI("BambuMonitor", "%s / %s / %s", SDGOODS_PROGRAM, SDGOODS_PLATFORM, SDGOODS_VENDOR);
    ESP_LOGI("BambuMonitor", "%s / %s / %s", BUILD_VERSION_STR, SDGOODS_HOMEPAGE, SDGOODS_CONTACT_EMAIL);
    gpio_config_t io = {.pin_bit_mask=1ULL<<BOARD_BAT_CONTROL_GPIO, .mode=GPIO_MODE_OUTPUT};
    ESP_ERROR_CHECK(gpio_config(&io));
    gpio_set_level(BOARD_BAT_CONTROL_GPIO, BOARD_BAT_CONTROL_LATCH_LEVEL);
    sdgoods_lcd_init_panel();
    sdgoods_lcd_apply_vendor_madctl();
    sdgoods_key_init();
    sdg_i18n_init();
    sdgoods_lvgl_init();
    ESP_ERROR_CHECK(sdgoods_touch_init());
    ESP_ERROR_CHECK(sdgoods_hw_info_init());
    ESP_ERROR_CHECK(sdgoods_audio_init());
    sdgoods_app_shell_init();
#ifdef CONFIG_BAMBU_BASELINE
    static const sdgoods_nav_t nav = {baseline_show, baseline_show, baseline_show};
    sdgoods_ui_set_nav(&nav);
    baseline_show();
#else
    (void)baseline_show;
    ESP_ERROR_CHECK(monitor_preferences_init());
    ESP_ERROR_CHECK(monitor_service_init());
    static const sdgoods_nav_t nav = {ui_monitor_start, ui_monitor_start, ui_monitor_start};
    sdgoods_ui_set_nav(&nav);
    sdgoods_apps_set_poll(ui_monitor_poll);
    sdgoods_set_power_short_handler(ui_monitor_back);
    ui_monitor_start();
#endif
    lv_refr_now(NULL);
    sdgoods_cc_settings_restore();
    sdgoods_lcd_set_backlight(sdgoods_cc_brightness_get());
    ESP_LOGI("BambuMonitor", "READY internal=%u largest=%u psram=%u",
        (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT),
        (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT),
        (unsigned)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    sdgoods_lvgl_loop();
}
