/* SPDX-License-Identifier: MIT
 * All view drawing, navigation and preference actions run on the LVGL thread.
 * The vendor control center is never recreated or modified by this application.
 */
#include "ui_monitor.h"
#include "monitor_skin.h"
#include "monitor_preferences.h"
#include "monitor_service.h"
#include "bambu_state.h"
#include "bambu_pages.h"
#include "sdgoods_board.h"
#include "sdgoods_gesture.h"
#include "sdgoods_cc.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

LV_FONT_DECLARE(bambu_font_16);
LV_FONT_DECLARE(bambu_font_20);
LV_FONT_DECLARE(bambu_digits_72);
LV_FONT_DECLARE(bambu_bold_18);LV_FONT_DECLARE(bambu_bold_22);LV_FONT_DECLARE(bambu_bold_26);
LV_FONT_DECLARE(bambu_num_44);LV_FONT_DECLARE(bambu_num_64);LV_FONT_DECLARE(bambu_num_88);LV_FONT_DECLARE(bambu_num_128);
typedef enum {VIEW_MONITOR,VIEW_SETTINGS,VIEW_APPEARANCE,VIEW_SLEEP,VIEW_NETWORK,VIEW_SKIN_PICKER} view_t;
static lv_obj_t *s_screen,*s_surface;
static sdgoods_tap_ctx_t s_tap;
static bambu_state_t s_data;
static monitor_service_status_t s_service;
static view_t s_view;
static int s_page,s_candidate;
static view_t s_picker_return=VIEW_MONITOR,s_press_view;
static int s_drag_candidate;
static bool s_tap_suppressed,s_hold_eligible,s_hold_fired;
static uint32_t s_press_time,s_picker_chrome_time;
static bool s_picker_chrome=true;
#define SKIN_HOLD_MS 700U
#define SKIN_TOUCH_SLOP 12
static int active_skin_id(void){return s_view==VIEW_SKIN_PICKER?s_candidate:monitor_preferences_get()->skin;}
static int wrapped_skin(int id){return (id-1+CONFIG_BAMBU_SKIN_COUNT)%CONFIG_BAMBU_SKIN_COUNT+1;}
static void open_skin_picker(view_t origin);
static bool s_asleep,s_dirty=true,s_pressed,s_wake_touch;
static bool s_gesture_blocked;
static uint32_t s_last_refresh,s_last_touch;
static lv_point_t s_down,s_last_point;
static bambu_print_state_t s_previous_state=BAMBU_STATE_UNKNOWN;
static const monitor_skin_t *s_skin;
static char s_message[80];
#ifdef BAMBU_LAYOUT_AUDIT
static unsigned s_audit_labels,s_audit_truncations;
#endif

static void open_skin_picker(view_t origin){
    s_picker_return=origin;s_candidate=monitor_preferences_get()->skin;
    s_view=VIEW_SKIN_PICKER;s_picker_chrome=true;s_picker_chrome_time=lv_tick_get();s_message[0]=0;s_dirty=true;
}

static void rect(lv_draw_ctx_t *ctx,int x,int y,int w,int h,uint32_t color,int radius) {
    if(w<=0||h<=0)return;
    lv_draw_rect_dsc_t d; lv_draw_rect_dsc_init(&d);
    d.bg_color=lv_color_hex(color); d.radius=radius;
    lv_area_t a={x,y,x+w-1,y+h-1}; lv_draw_rect(ctx,&d,&a);
}
/* All monitor labels are one line. Fit using real glyph advances, then clip
 * to their own line box; an unexpectedly long value cannot cover another row. */
static const lv_font_t *fit_font(const char *v,const lv_font_t *font,int width) {
    const lv_font_t *choices[]={font,&bambu_bold_26,&bambu_bold_22,&bambu_bold_18,&bambu_font_16};
    for(unsigned i=0;i<sizeof(choices)/sizeof(*choices);i++)
        if(choices[i]->line_height<=font->line_height && lv_txt_get_width(v,strlen(v),choices[i],0,0)<=width) return choices[i];
    return &bambu_font_16;
}
static void text(lv_draw_ctx_t *ctx,int x,int y,int w,const char *value,
                 const lv_font_t *font,uint32_t color,lv_text_align_t align) {
    const int original_height=font->line_height;
    font=fit_font(value,font,w);
    y+=(original_height-font->line_height)/2;
    /* Six-pixel radial inset; use the narrowest chord across the full line. */
    int dy1=abs(y-180),dy2=abs(y+font->line_height-1-180),dy=LV_MAX(dy1,dy2),half=0;
    while((half+1)*(half+1)+dy*dy<=174*174) half++;
    int left=LV_MAX(x,180-half),right=LV_MIN(x+w-1,180+half);
    w=right-left+1;x=left;if(w<=0)return;
    font=fit_font(value,font,w);
    char fitted[192];snprintf(fitted,sizeof(fitted),"%s",value);
    if(lv_txt_get_width(fitted,strlen(fitted),font,0,0)>w){
        size_t n=strlen(fitted);
        while(n && lv_txt_get_width(fitted,strlen(fitted),font,0,0)>w){
            do{n--;}while(n && ((unsigned char)fitted[n]&0xC0)==0x80);
            memcpy(fitted+n,"...",4);
        }
    }
#ifdef BAMBU_LAYOUT_AUDIT
    s_audit_labels++;
    if(strcmp(value,fitted)){s_audit_truncations++;fprintf(stderr,"TRUNCATED skin=%d page=%d view=%d x=%d y=%d w=%d: %s -> %s\n",active_skin_id(),s_page,s_view,x,y,w,value,fitted);}
#endif
    lv_draw_label_dsc_t d;lv_draw_label_dsc_init(&d);
    d.font=font;d.color=lv_color_hex(color);d.align=align;d.flag=LV_TEXT_FLAG_EXPAND;
    lv_area_t a={x,y,x+w-1,y+font->line_height-1},clip;
    const lv_area_t *saved=ctx->clip_area;
    if(!_lv_area_intersect(&clip,saved,&a))return;
    ctx->clip_area=&clip;lv_draw_label(ctx,&d,&a,fitted,NULL);ctx->clip_area=saved;
}
static const char *state_name(void) {
    if(!s_service.connected) return SDG_T("连接中","Connecting");
    if(!bambu_state_fresh(&s_data,(uint32_t)(esp_timer_get_time()/1000)))
        return SDG_T("等待新数据","Awaiting data");
    switch(s_data.state) {
        case BAMBU_STATE_RUNNING:return SDG_T("打印中","Printing");
        case BAMBU_STATE_IDLE:return SDG_T("空闲","Idle");
        case BAMBU_STATE_PREPARE:return SDG_T("准备中","Preparing");
        case BAMBU_STATE_PAUSE:return SDG_T("已暂停","Paused");
        case BAMBU_STATE_FINISH:return SDG_T("已完成","Finished");
        case BAMBU_STATE_FAILED:return SDG_T("打印异常","Print error");
        default:return SDG_T("等待状态","Awaiting state");
    }
}
static void value(char *out,size_t n,uint32_t mask,float v,const char *unit) {
    if(s_data.valid_mask&mask) snprintf(out,n,"%.0f%s",(double)v,unit);
    else snprintf(out,n,"--");
}
static int page_count(void) {return bambu_pages_count(&s_data);}
#include "ui_monitor_design.inc"
#include "ui_monitor_batch2.inc"
#include "ui_monitor_batch3.inc"
#include "ui_monitor_settings.inc"

static void draw_monitor(lv_draw_ctx_t *ctx,int page,bool navigation) {
    s_skin=monitor_skin_get(active_skin_id());rect(ctx,0,0,360,360,s_skin->bg,0);
    if(design_id()<=6) {
        if(page==0) batch1_overview(ctx);
        else if(page==page_count()-1) batch1_details(ctx);
        else {int u=bambu_pages_unit(&s_data,page);
            if(u==BAMBU_PAGE_EXTERNAL || (u>=0 && s_data.units[u].kind==BAMBU_AMS_HT)) batch1_accessories(ctx);
            else batch1_materials(ctx);}
        if(navigation)design_dots(ctx);
    }
#if CONFIG_BAMBU_SKIN_COUNT > 6
    else if(design_id()<=12){
        b2_background(ctx);
        if(page==0)batch2_overview(ctx);
        else if(page==page_count()-1)batch2_details(ctx);
        else {int u=bambu_pages_unit(&s_data,page);
            if(u==BAMBU_PAGE_EXTERNAL || (u>=0 && s_data.units[u].kind==BAMBU_AMS_HT))batch2_accessories(ctx);
            else batch2_materials(ctx);}
        if(navigation)design_dots(ctx);
    }
#endif
#if CONFIG_BAMBU_SKIN_COUNT > 12
    else {
        if(page==0)batch3_overview(ctx);
        else if(page==page_count()-1)batch3_details(ctx);
        else {int u=bambu_pages_unit(&s_data,page);
            if(u==BAMBU_PAGE_EXTERNAL || (u>=0 && s_data.units[u].kind==BAMBU_AMS_HT))batch3_accessories(ctx);
            else batch3_materials(ctx);}
        if(navigation)design_dots(ctx);
    }
#endif
}
static void draw(lv_event_t *e) {
    lv_draw_ctx_t *ctx=lv_event_get_draw_ctx(e);
    s_skin=monitor_skin_get(active_skin_id());
    if(s_view==VIEW_MONITOR) draw_monitor(ctx,s_page,true);
    else if(s_view==VIEW_SKIN_PICKER) {draw_monitor(ctx,0,false);if(s_picker_chrome)draw_skin_picker_controls(ctx);}
    else {s_skin=monitor_skin_get(1);draw_settings(ctx);}
    if(s_hold_eligible&&s_pressed&&!s_hold_fired){
        uint32_t elapsed=lv_tick_get()-s_press_time;
        if(elapsed>=180){
            lv_draw_arc_dsc_t d;lv_draw_arc_dsc_init(&d);d.width=3;d.rounded=1;d.color=lv_color_hex(s_skin->accent);
            lv_draw_arc(ctx,&d,&s_down,21,270,270+(elapsed>SKIN_HOLD_MS?360:elapsed*360/SKIN_HOLD_MS));
        }
    }
    if(s_message[0]) {rect(ctx,60,263,240,28,0xFFDDD3,10);center(ctx,65,267,230,s_message,&bambu_font_16,0x7D271A);}
}
static void awake(void) {
    if(s_asleep) {sdgoods_lcd_set_backlight(sdgoods_cc_brightness_get());s_asleep=false;}
    s_last_touch=lv_tick_get();
}
bool ui_monitor_back(void) {
    if(sdgoods_cc_is_open()) return false;
    if(s_asleep) {awake();return true;}
    if(s_view==VIEW_SKIN_PICKER) {s_view=s_picker_return;s_candidate=monitor_preferences_get()->skin;s_message[0]=0;s_dirty=true;return true;}
    if(s_view==VIEW_NETWORK && (s_service.provisioning||s_service.busy)) {
        monitor_service_provision(false);return true;
    }
    if(s_view!=VIEW_MONITOR) {s_view=s_view==VIEW_SETTINGS?VIEW_MONITOR:VIEW_SETTINGS;s_dirty=true;return true;}
    return false;
}
static void on_tap(void *arg) {
    (void)arg;
    lv_point_t pt=s_last_point;
    if(s_asleep||s_wake_touch) {awake();return;}
    awake();
    if(s_tap_suppressed||sdgoods_cc_is_open()) return;
    if(s_service.busy) return;
    monitor_preferences_t p=*monitor_preferences_get();
    bool save=false;
    if(s_view==VIEW_MONITOR) {
        if(s_page==page_count()-1 && detail_settings_hit(pt)) {s_view=VIEW_SETTINGS;s_candidate=p.skin;}
    } else if(s_view==VIEW_SKIN_PICKER) {
        s_picker_chrome_time=lv_tick_get();
        if(pt.y>=295&&pt.y<342 && pt.x>=82&&pt.x<128) {ui_monitor_back();return;}
        if(pt.y>=295&&pt.y<342 && pt.x>=232&&pt.x<278) {
            p.skin=s_candidate;
            if(monitor_preferences_save(&p)==ESP_OK){s_view=s_picker_return;s_message[0]=0;}
            else snprintf(s_message,sizeof(s_message),"%s",SDG_T("保存失败，请重试","Save failed; retry"));
        } else if(pt.y>=130&&pt.y<230&&(pt.x<55||pt.x>305))s_candidate=wrapped_skin(s_candidate+(pt.x<55?-1:1));
        s_dirty=true;return;
    } else if(pt.x>=42 && pt.x<92 && pt.y>=28 && pt.y<78) ui_monitor_back();
    else if(s_view==VIEW_SETTINGS) {
        if(pt.y>=96&&pt.y<144) s_view=VIEW_APPEARANCE;
        else if(pt.y>=152&&pt.y<200) s_view=VIEW_SLEEP;
        else if(pt.y>=208&&pt.y<256) s_view=VIEW_NETWORK;
    } else if(s_view==VIEW_APPEARANCE) {
        if(pt.y>=106&&pt.y<166)open_skin_picker(VIEW_APPEARANCE);
        else if(pt.y>=184&&pt.y<244)sdg_i18n_toggle();
    } else if(s_view==VIEW_SLEEP) {
        if(pt.y>=89&&pt.y<129) {p.sleep_min=p.sleep_min==0?1:p.sleep_min==1?5:p.sleep_min==5?15:0;save=true;}
        else if(pt.y>=135&&pt.y<175) {p.keep_printing=!p.keep_printing;save=true;}
        else if(pt.y>=181&&pt.y<221) {p.wake_finish=!p.wake_finish;save=true;}
        else if(pt.y>=227&&pt.y<267) {p.wake_error=!p.wake_error;save=true;}
    } else if(s_view==VIEW_NETWORK) {
        if(s_service.provisioning && pt.y>=305&&pt.y<345) monitor_service_provision(false);
        else if(!s_service.provisioning && pt.y>=276&&pt.y<315) monitor_service_provision(true);
    }
    if(save && monitor_preferences_save(&p)!=ESP_OK)
        snprintf(s_message,sizeof(s_message),"%s",SDG_T("保存失败，请重试","Save failed; retry"));
    else s_message[0]=0;
    s_dirty=true;
}
void ui_monitor_start(void) {
    if(!s_screen) {
        s_screen=lv_obj_create(NULL);lv_obj_clear_flag(s_screen,LV_OBJ_FLAG_SCROLLABLE);
        s_surface=lv_obj_create(s_screen);lv_obj_remove_style_all(s_surface);
        lv_obj_set_size(s_surface,360,360);lv_obj_clear_flag(s_surface,LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(s_surface,draw,LV_EVENT_DRAW_MAIN,NULL);
        s_last_touch=lv_tick_get();s_candidate=monitor_preferences_get()->skin;
        int previous_unit=bambu_pages_unit(&s_data,s_page);
        int previous_id=previous_unit>=0?s_data.units[previous_unit].id:previous_unit==BAMBU_PAGE_EXTERNAL?254:-1;
        monitor_service_status(&s_service);bambu_state_copy(&s_data);
        if(previous_unit==BAMBU_PAGE_DETAILS) s_page=page_count()-1;
        else if(previous_id>=0) s_page=bambu_pages_find(&s_data,previous_id);
        if(s_service.provisioning) s_view=VIEW_NETWORK;
        sdgoods_app_on_tap(s_screen,&s_tap,on_tap,NULL);
    }
    lv_scr_load(s_screen);sdgoods_app_shell_bind(s_screen);
    s_dirty=true;
}
void ui_monitor_poll(void) {
    if(!s_screen || lv_scr_act()!=s_screen) return;
    uint32_t now=lv_tick_get();
    bool pressed=sdgoods_touch_get_state()==LV_INDEV_STATE_PRESSED;
    if(pressed) sdgoods_touch_get_point(&s_last_point);
    if(pressed&&!s_pressed) {
        s_down=s_last_point;s_wake_touch=s_asleep;
        s_gesture_blocked=s_wake_touch||s_service.busy||sdgoods_cc_is_open();
        s_tap_suppressed=s_gesture_blocked;
        s_press_time=now;s_press_view=s_view;s_drag_candidate=s_candidate;s_hold_fired=false;s_dirty=true;
        if(s_view==VIEW_SKIN_PICKER&&!s_picker_chrome){s_picker_chrome=true;s_picker_chrome_time=now;s_tap_suppressed=true;}
        s_hold_eligible=!s_wake_touch&&!s_service.busy&&!sdgoods_cc_is_open()&&s_view==VIEW_MONITOR&&s_page==0&&
            s_down.y>=80&&s_down.y<=270&&s_down.x>=40&&s_down.x<=320;
        if(s_asleep)awake();
    }
    if(pressed) {
        s_last_touch=now;
        int dx=s_last_point.x-s_down.x,dy=s_last_point.y-s_down.y;
        // A drag remains a drag even after reversing or returning to its start.
        // LVGL can dispatch CLICKED before this poll sees the release.
        if(abs(dx)>SKIN_TOUCH_SLOP||abs(dy)>SKIN_TOUCH_SLOP)s_tap_suppressed=true;
        if(sdgoods_cc_is_open()||s_service.busy){s_gesture_blocked=true;s_tap_suppressed=true;}
        if(abs(dx)>SKIN_TOUCH_SLOP||abs(dy)>SKIN_TOUCH_SLOP||sdgoods_cc_is_open()||s_view!=s_press_view||s_service.busy){if(s_hold_eligible)s_dirty=true;s_hold_eligible=false;}
        if(s_hold_eligible&&now-s_press_time>=SKIN_HOLD_MS){
            open_skin_picker(VIEW_MONITOR);s_hold_fired=true;s_hold_eligible=false;s_tap_suppressed=true;
            s_press_view=VIEW_SKIN_PICKER;s_drag_candidate=s_candidate;s_down=s_last_point;
        }else if(s_hold_eligible&&now-s_press_time>=180)s_dirty=true;
        if(!s_gesture_blocked&&!s_wake_touch&&!sdgoods_cc_is_open()&&s_press_view==VIEW_SKIN_PICKER&&s_view==VIEW_SKIN_PICKER&&s_down.y>=80&&s_down.y<295){
            int step=abs(dx)>abs(dy)*2?(dx<=-48?1:dx>=48?-1:0):0;
            if(step)s_tap_suppressed=true;
            int candidate=wrapped_skin(s_drag_candidate+step);
            if(s_candidate!=candidate){s_candidate=candidate;s_picker_chrome=false;s_dirty=true;}
        }
    }
    if(!pressed&&s_pressed) {
        int dx=s_last_point.x-s_down.x,dy=s_last_point.y-s_down.y;
        if(!s_gesture_blocked&&!s_wake_touch&&!s_hold_fired&&!sdgoods_cc_is_open()&&s_press_view==VIEW_MONITOR&&s_view==VIEW_MONITOR&&
            s_down.y>80&&s_down.y<270&&abs(dx)>45&&abs(dx)>abs(dy)*2){
            s_page=(s_page+(dx<0?1:page_count()-1))%page_count();s_tap_suppressed=true;s_dirty=true;
        }
        if(!s_gesture_blocked&&!s_wake_touch&&!s_hold_fired&&!sdgoods_cc_is_open()&&!s_service.busy&&
            s_press_view>=VIEW_SETTINGS&&s_press_view<=VIEW_NETWORK&&s_view==s_press_view&&
            s_down.y>80&&s_down.y<315&&dx>48&&dx>abs(dy)*2){
            s_tap_suppressed=true;ui_monitor_back();s_dirty=true;
        }
        if(s_view==VIEW_SKIN_PICKER&&s_press_view==VIEW_SKIN_PICKER){s_picker_chrome=true;s_picker_chrome_time=now;}
        s_hold_eligible=false;s_dirty=true;
    }
    s_pressed=pressed;
    if(s_view==VIEW_SKIN_PICKER&&s_picker_chrome&&!pressed&&now-s_picker_chrome_time>=3000U){s_picker_chrome=false;s_dirty=true;}
    if(now-s_last_refresh>=500) {
        bool was_prov=s_service.provisioning;
        int previous_unit=bambu_pages_unit(&s_data,s_page);
        int previous_id=previous_unit>=0?s_data.units[previous_unit].id:previous_unit==BAMBU_PAGE_EXTERNAL?254:-1;
        monitor_service_status(&s_service);bambu_state_copy(&s_data);
        if(previous_unit==BAMBU_PAGE_DETAILS) s_page=page_count()-1;
        else if(previous_id>=0) s_page=bambu_pages_find(&s_data,previous_id);
        if(s_service.provisioning&&!was_prov) s_view=VIEW_NETWORK;
        if(!s_service.provisioning&&was_prov) s_view=VIEW_SETTINGS;
        if(s_page>=page_count()) s_page=0;
        if(s_data.state!=s_previous_state) {
            const monitor_preferences_t *p=monitor_preferences_get();
            if((s_data.state==BAMBU_STATE_FINISH&&p->wake_finish)||
                (s_data.state==BAMBU_STATE_FAILED&&p->wake_error)) awake();
            s_previous_state=s_data.state;
        }
        s_last_refresh=now;s_dirty=true;
    }
    const monitor_preferences_t *p=monitor_preferences_get();
    bool printing=s_service.connected&&bambu_state_fresh(&s_data,(uint32_t)(esp_timer_get_time()/1000))&&
        (s_data.state==BAMBU_STATE_RUNNING||s_data.state==BAMBU_STATE_PREPARE);
    if(!s_asleep && p->sleep_min && !s_service.provisioning && !s_service.busy &&
        !sdgoods_cc_is_open() && !(p->keep_printing&&printing) && now-s_last_touch>p->sleep_min*60000U) {
        sdgoods_lcd_set_backlight(0);s_asleep=true;
    }
    if(sdgoods_cc_is_open()) {awake();return;}
    if(s_dirty) {lv_obj_invalidate(s_surface);s_dirty=false;}
}
