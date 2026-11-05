/* SPDX-License-Identifier: MIT */
#include "monitor_preferences.h"
#include "nvs.h"
#include "sdkconfig.h"
static monitor_preferences_t s_prefs = {1,5,true,true,true};
esp_err_t monitor_preferences_init(void) {
    nvs_handle_t h;
    esp_err_t err=nvs_open("bmb_prefs",NVS_READWRITE,&h);
    if(err!=ESP_OK) return err;
    monitor_preferences_t p; size_t n=sizeof(p);
    if(nvs_get_blob(h,"v1",&p,&n)==ESP_OK && n==sizeof(p) &&
        p.skin>=1 && p.skin<=CONFIG_BAMBU_SKIN_COUNT &&
        (p.sleep_min==0||p.sleep_min==1||p.sleep_min==5||p.sleep_min==15)) s_prefs=p;
    nvs_close(h); return ESP_OK;
}
const monitor_preferences_t *monitor_preferences_get(void) {return &s_prefs;}
esp_err_t monitor_preferences_save(const monitor_preferences_t *p) {
    if(!p || p->skin<1 || p->skin>CONFIG_BAMBU_SKIN_COUNT ||
        !(p->sleep_min==0||p->sleep_min==1||p->sleep_min==5||p->sleep_min==15)) return ESP_ERR_INVALID_ARG;
    nvs_handle_t h; esp_err_t err=nvs_open("bmb_prefs",NVS_READWRITE,&h);
    if(err!=ESP_OK) return err;
    err=nvs_set_blob(h,"v1",p,sizeof(*p));
    if(err==ESP_OK) err=nvs_commit(h);
    nvs_close(h); if(err==ESP_OK) s_prefs=*p; return err;
}
