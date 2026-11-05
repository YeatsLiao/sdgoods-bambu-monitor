/* SPDX-License-Identifier: MIT
 * Network lifecycle owner. No LVGL calls; UI only queues requests and reads snapshots.
 */
#include "monitor_service.h"
#include "app_config.h"
#include "bambu_mqtt.h"
#include "wifi_prov.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <string.h>
static QueueHandle_t s_requests;
static portMUX_TYPE s_mux=portMUX_INITIALIZER_UNLOCKED;
static monitor_service_status_t s_status;
static void publish(const monitor_service_status_t *value) {
    portENTER_CRITICAL(&s_mux); s_status=*value; portEXIT_CRITICAL(&s_mux);
}
void monitor_service_status(monitor_service_status_t *out) {
    portENTER_CRITICAL(&s_mux); *out=s_status; portEXIT_CRITICAL(&s_mux);
}
bool monitor_service_provision(bool enable) {
    return s_requests && xQueueSend(s_requests,&enable,0)==pdTRUE;
}
static void service_task(void *arg) {
    (void)arg;
    monitor_service_status_t st={0};
    bool want_prov=!app_config_is_provisioned();
    bool transition=true;
    int64_t trial_deadline=esp_timer_get_time()+90000000;
    while(1) {
        bool next;
        if(xQueueReceive(s_requests,&next,0)==pdTRUE) {want_prov=next; transition=true;}
        if(transition) {
            st.busy=true; publish(&st);
            if(want_prov) {
                bambu_mqtt_stop();
                st.error=wifi_prov_start();
                st.provisioning=st.error==ESP_OK;
                if(st.provisioning) wifi_prov_get_ap_ssid(st.ap_ssid,sizeof(st.ap_ssid));
            } else if(!st.provisioning || wifi_prov_request_cancel()) {
                if(st.provisioning) wifi_prov_stop();
                st.provisioning=false;
                st.error=app_config_is_provisioned()?bambu_mqtt_start():ESP_OK;
            }
            st.busy=false; transition=false;
        }
        if(!st.provisioning) {
            if(app_config_is_provisioned()) {
                esp_err_t err=bambu_mqtt_start();
                if(err!=ESP_OK) st.error=err;
                bambu_mqtt_poll();
            }
            if(app_config_has_trial()) {
                if(bambu_mqtt_has_report()) st.error=app_config_confirm_trial();
                else if(esp_timer_get_time()>trial_deadline) {
                    bambu_mqtt_stop();
                    st.error=app_config_rollback_trial();
                    want_prov=!app_config_is_provisioned(); transition=true;
                }
            }
        }
        bool connected=bambu_mqtt_connected();
        if(connected && !st.connected) {
            ESP_LOGI("bambu_service","CONNECTED internal=%u largest=%u psram=%u",
                (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT),
                (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT),
                (unsigned)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
        }
        st.connected=connected;
        if(connected) st.error=ESP_OK;
        publish(&st);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
esp_err_t monitor_service_init(void) {
    bambu_state_init();
    esp_err_t err=app_config_init(); if(err!=ESP_OK) return err;
    s_requests=xQueueCreate(4,sizeof(bool));
    if(!s_requests) return ESP_ERR_NO_MEM;
    if(xTaskCreate(service_task,"bambu_service",8192,NULL,4,NULL)!=pdPASS) {
        vQueueDelete(s_requests); s_requests=NULL; return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}
