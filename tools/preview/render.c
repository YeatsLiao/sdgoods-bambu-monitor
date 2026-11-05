/* SPDX-License-Identifier: MIT
 * Native LVGL software renderer. Uses the exact production view and generated fonts.
 * Sample telemetry exists ONLY here, never in device firmware.
 */
#define BAMBU_LAYOUT_AUDIT 1
#include "../../main/apps/ui_monitor.c"
#include <assert.h>
#include "monitor_art.h"
static monitor_preferences_t prefs={1,5,true,true,true};
static bool chinese=true;
static bool test_pressed,test_cc_open,test_save_fail;
static lv_point_t test_point;
static int test_save_count;
static monitor_service_status_t test_service={.connected=true};
static unsigned char framebuffer[360*360*3];
const monitor_preferences_t *monitor_preferences_get(void) {return &prefs;}
esp_err_t monitor_preferences_save(const monitor_preferences_t *p) {if(test_save_fail)return -1;test_save_count++;prefs=*p;return 0;}
void monitor_service_status(monitor_service_status_t *s) {*s=test_service;}
bool monitor_service_provision(bool enable) {(void)enable;return true;}
bool sdg_i18n_is_zh(void) {return chinese;}
void sdg_i18n_toggle(void) {chinese=!chinese;}
void sdgoods_lcd_set_backlight(int n) {(void)n;}
void sdgoods_app_shell_bind(lv_obj_t *obj) {(void)obj;}
void sdgoods_app_on_tap(lv_obj_t *o,sdgoods_tap_ctx_t *c,sdgoods_tap_cb_t cb,void *ud) {(void)o;(void)c;(void)cb;(void)ud;}
bool sdgoods_cc_is_open(void) {return test_cc_open;}
int sdgoods_cc_brightness_get(void) {return 60;}
lv_indev_state_t sdgoods_touch_get_state(void) {return test_pressed?LV_INDEV_STATE_PRESSED:LV_INDEV_STATE_RELEASED;}
void sdgoods_touch_get_point(lv_point_t *p) {*p=test_point;}
static void flush(lv_disp_drv_t *drv,const lv_area_t *a,lv_color_t *pixels) {
    for(int y=a->y1;y<=a->y2;y++) for(int x=a->x1;x<=a->x2;x++) {
        lv_color_t c=*pixels++;
        if(x<0||y<0||x>=360||y>=360)continue;
        uint32_t rgb=lv_color_to32(c);
        int i=(y*360+x)*3;
        framebuffer[i]=rgb>>16;framebuffer[i+1]=rgb>>8;framebuffer[i+2]=rgb;
    }
    lv_disp_flush_ready(drv);
}
static void shot(const char *folder,const char *name) {
    char path[512];snprintf(path,sizeof(path),"%s/%s.ppm",folder,name);
    lv_obj_invalidate(s_surface);lv_refr_now(NULL);
    FILE *f=fopen(path,"wb");assert(f);fprintf(f,"P6\n360 360\n255\n");
    fwrite(framebuffer,1,sizeof(framebuffer),f);fclose(f);
}
static void touch(bool pressed,int x,int y,uint32_t ms){
    test_pressed=pressed;test_point=(lv_point_t){x,y};lv_tick_inc(ms);ui_monitor_poll();
}
/* Real loop ordering: LVGL's clicked callback runs before the app sees release. */
static void tap(int x,int y){touch(true,x,y,20);test_pressed=false;on_tap(NULL);touch(false,x,y,20);}
static void picker_tests(void){
    s_service=test_service;s_view=VIEW_MONITOR;s_page=0;prefs.skin=1;s_pressed=false;s_asleep=false;
    s_tap_suppressed=s_wake_touch=false;
    int saves=test_save_count;
    touch(true,180,180,20);touch(true,180,180,SKIN_HOLD_MS-1);assert(s_view==VIEW_MONITOR);
    touch(true,180,180,1);assert(s_view==VIEW_SKIN_PICKER&&s_candidate==1&&prefs.skin==1);
    /* A long-hold release over Apply cannot commit or close the picker. */
    touch(true,253,317,20);test_pressed=false;on_tap(NULL);touch(false,253,317,20);
    assert(s_view==VIEW_SKIN_PICKER&&test_save_count==saves);
    touch(true,270,180,20);touch(true,210,180,20);assert(s_candidate==2&&prefs.skin==1&&s_view==VIEW_SKIN_PICKER);
    touch(true,270,180,20);assert(s_candidate==1);
    touch(true,330,180,20);assert(s_candidate==CONFIG_BAMBU_SKIN_COUNT);
    touch(false,330,180,20);assert(s_page==0&&test_save_count==saves);
    /* Swipe changes are visible while held; preference is written only at Apply. */
    tap(253,317);assert(s_view==VIEW_MONITOR&&prefs.skin==CONFIG_BAMBU_SKIN_COUNT&&test_save_count==saves+1);
    open_skin_picker(VIEW_MONITOR);tap(334,182);assert(s_candidate==1&&prefs.skin==CONFIG_BAMBU_SKIN_COUNT);
    tap(107,317);assert(s_view==VIEW_MONITOR&&prefs.skin==CONFIG_BAMBU_SKIN_COUNT&&test_save_count==saves+1);
    s_view=VIEW_APPEARANCE;tap(180,130);assert(s_view==VIEW_SKIN_PICKER&&s_picker_return==VIEW_APPEARANCE);
    tap(334,182);assert(s_candidate==1);test_save_fail=true;tap(253,317);
    assert(s_view==VIEW_SKIN_PICKER&&s_message[0]&&prefs.skin==CONFIG_BAMBU_SKIN_COUNT);test_save_fail=false;
    assert(ui_monitor_back()&&s_view==VIEW_APPEARANCE&&prefs.skin==CONFIG_BAMBU_SKIN_COUNT);
    bool lang=chinese;tap(180,210);assert(chinese!=lang);chinese=lang;
    /* Movement cancels hold eligibility permanently even if the finger returns. */
    s_view=VIEW_MONITOR;prefs.skin=1;s_page=0;
    touch(true,180,180,20);touch(true,200,180,200);touch(true,180,180,SKIN_HOLD_MS);
    assert(s_view==VIEW_MONITOR);touch(false,180,180,20);
    /* Vertical/top-edge gestures and non-home pages never enter skin selection. */
    touch(true,180,35,20);touch(true,180,35,SKIN_HOLD_MS+10);assert(s_view==VIEW_MONITOR);touch(false,180,35,20);
    s_page=1;touch(true,180,180,20);touch(true,180,180,SKIN_HOLD_MS+10);assert(s_view==VIEW_MONITOR);touch(false,180,180,20);s_page=0;
    /* Control-center ownership cancels a pending hold; it cannot resume later. */
    touch(true,180,180,20);test_cc_open=true;touch(true,180,180,300);test_cc_open=false;
    touch(true,180,180,SKIN_HOLD_MS);assert(s_view==VIEW_MONITOR);touch(false,180,180,20);
    /* First touch wakes only, including long holds. */
    s_asleep=true;touch(true,180,180,20);touch(true,180,180,SKIN_HOLD_MS+10);assert(!s_asleep&&s_view==VIEW_MONITOR);touch(false,180,180,20);
    s_wake_touch=false;s_tap_suppressed=false;
    touch(true,270,180,20);touch(true,200,180,20);touch(false,200,180,20);assert(s_page==1&&s_view==VIEW_MONITOR);
    /* Picker back restores the original page and settings entry; browsing never saves. */
    s_page=page_count()-1;open_skin_picker(VIEW_APPEARANCE);tap(334,182);assert(ui_monitor_back()&&s_view==VIEW_APPEARANCE&&s_page==page_count()-1);
    s_view=VIEW_MONITOR;
    open_skin_picker(VIEW_MONITOR);touch(false,180,180,3001);assert(!s_picker_chrome);
    int prior=test_save_count;tap(253,317);assert(s_view==VIEW_SKIN_PICKER&&s_picker_chrome&&test_save_count==prior);
    tap(253,317);assert(s_view==VIEW_MONITOR&&test_save_count==prior+1);
    open_skin_picker(VIEW_MONITOR);s_candidate=4;
    bambu_state_t live;bambu_state_copy(&live);live.mc_percent=37;live.valid_mask|=BAMBU_VALID_PROGRESS;live.data_valid=true;bambu_state_commit(&live);
    touch(false,180,180,501);assert(s_data.mc_percent==37&&active_skin_id()==4&&prefs.skin==1&&test_save_count==prior+1);
    bool keep=prefs.keep_printing;prefs.sleep_min=1;prefs.keep_printing=false;
    touch(false,180,180,60001);assert(s_asleep&&s_view==VIEW_SKIN_PICKER);
    tap(253,317);assert(!s_asleep&&s_view==VIEW_SKIN_PICKER&&prefs.skin==1&&test_save_count==prior+1);
    assert(ui_monitor_back());prefs.sleep_min=5;prefs.keep_printing=keep;s_wake_touch=s_tap_suppressed=false;
    puts("PASS: live telemetry continues in candidate preview; sleep preference and wake-only first touch preserved.");
    puts("PASS: 700ms hold, drift rejection, release isolation, live swipe/reversal/wrap, apply/cancel/save failure, language, wake, top/control-center guards, page preservation.");
}
static void art_codec_tests(void){
    uint8_t palette[512]={0},out[10];palette[10]=0x12;palette[11]=0x34;
    const uint8_t good[]={0x82,5,1,4,3};monitor_art_t a={5,1,palette,good,sizeof(good)};
    assert(monitor_art_expand(&a,out,sizeof(out)));assert(out[0]==0x12&&out[1]==0x34&&out[4]==0x12&&out[5]==0x34);
    assert(!monitor_art_expand(&a,out,9));
    const uint8_t short_literal[]={2,1};a.runs=short_literal;a.size=sizeof(short_literal);assert(!monitor_art_expand(&a,out,sizeof(out)));
    const uint8_t overflow[]={0x85,5};a.runs=overflow;a.size=sizeof(overflow);assert(!monitor_art_expand(&a,out,sizeof(out)));
    const uint8_t trailing[]={0x84,5,0};a.runs=trailing;a.size=sizeof(trailing);assert(!monitor_art_expand(&a,out,sizeof(out)));
    a.width=0;assert(!monitor_art_expand(&a,out,sizeof(out)));
    puts("PASS: actual C artwork expansion, palette bytes, truncated/literal/overflow/trailing/capacity/zero-size rejection.");
}
static void round_box_test(int x,int y,int w,int h,int radius){
    for(int py=y;py<y+h;py++)for(int px=x;px<x+w;px++){
        int cx=px<x+radius?x+radius:px>=x+w-radius?x+w-radius-1:px;
        int cy=py<y+radius?y+radius:py>=y+h-radius?y+h-radius-1:py;
        if((px-cx)*(px-cx)+(py-cy)*(py-cy)>radius*radius)continue;
        int dx=2*px-359,dy=2*py-359;assert(dx*dx+dy*dy<=348*348);
    }
}
static void geometry_tests(void){
    for(int i=0;i<4;i++)round_box_test(DESIGN_GRID_X+i%2*DESIGN_GRID_STEP,DESIGN_GRID_Y+i/2*DESIGN_GRID_ROW,DESIGN_GRID_W,DESIGN_GRID_H,0);
    round_box_test(44,181,272,118,23);round_box_test(44,115,272,180,23);round_box_test(78,301,204,26,13);
    const char *numbers[]={"--","0","9","128","256","9999","1000000"};
    for(int pixel=0;pixel<2;pixel++)for(unsigned i=0;i<7;i++)for(unsigned j=0;j<7;j++){
        layer_layout_t p=layer_layout(numbers[i],numbers[j],pixel);assert(p.left&&p.right&&p.slash&&p.gap>=4);
        assert(p.x>=34 && p.x+p.left+2*p.gap+p.slash+p.right<=326);
        assert(pixel?p.cell>0:p.font!=NULL);
    }
    puts("PASS: grid and HT rounded corners inside 6px circle inset; 98 independent number/slash layouts with explicit gaps.");
}
static void layout_tests(void){
    char b[80],d[48];
    for(int lang=0;lang<2;lang++){
        chinese=lang==0;
        for(int minutes=0;minutes<=1000000;minutes=minutes<59?59:minutes<60?60:minutes<1440?1440:1000001){
            duration(d,sizeof(d),minutes);snprintf(b,sizeof(b),SDG_T("剩余 %s","Remaining %s"),d);
            const lv_font_t *f=fit_font(b,sdg_i18n_is_zh()?&bambu_bold_22:&bambu_bold_18,268);
            assert(lv_txt_get_width(b,strlen(b),f,0,0)<=268 && f->line_height<=27);
            assert(!strchr(b,'\n'));
        }
        duration(d,sizeof(d),1000000);snprintf(b,sizeof(b),SDG_T("剩余 %s","Remaining %s"),d);
        assert(lv_txt_get_width(b,strlen(b),fit_font(b,&bambu_bold_22,268),0,0)<=268);
    }
    assert(fit_font(LV_SYMBOL_SETTINGS,&lv_font_montserrat_20,28)==&lv_font_montserrat_20);
    assert(lv_txt_get_width("Noz",3,&bambu_font_16,0,0)<=36);
    const char *layer="1000000/1000000";
    assert(lv_txt_get_width(layer,strlen(layer),fit_font(layer,&bambu_num_44,298),0,0)<=298);
    assert(lv_txt_get_width("辅助 100%",strlen("辅助 100%"),&bambu_font_16,0,0)<=84);
    chinese=true;puts("PASS: real font metrics fit minute/hour/day times, 1,000,000 layers, compact English temperatures and gear glyph.");
}
int main(int argc,char **argv) {
    assert(argc==2);
    lv_init();layout_tests();geometry_tests();art_codec_tests();
    static lv_color_t buf[360*24];static lv_disp_draw_buf_t db;
    lv_disp_draw_buf_init(&db,buf,NULL,360*24);
    static lv_disp_drv_t drv;lv_disp_drv_init(&drv);
    drv.hor_res=drv.ver_res=360;drv.flush_cb=flush;drv.draw_buf=&db;lv_disp_drv_register(&drv);
    bambu_state_init();ui_monitor_start();
    s_data.valid_mask=0x3fff;s_data.data_valid=true;s_data.last_update_ms=1000;
    s_data.state=BAMBU_STATE_RUNNING;s_data.mc_percent=68;s_data.mc_remaining=42;
    s_data.nozzle_temp=s_data.nozzle_target=220;s_data.bed_temp=s_data.bed_target=60;
    s_data.valid_mask&=~BAMBU_VALID_CHAMBER;s_data.layer_num=128;s_data.total_layer=256;
    s_data.spd_mag=100;s_data.cooling_fan=70;s_data.big_fan1=0;
    strcpy(s_data.printer_model,"H2D");
    s_data.unit_count=2;s_data.active_unit_id=0;s_data.active_slot_id=1;
    bambu_ams_unit_t *u=&s_data.units[0];u->id=0;u->kind=BAMBU_AMS_STANDARD;u->slot_count=4;u->humidity=28;u->temperature=26;
    const char *colors[]={"28C8D4","F87D5B","F3CD39","6DCB94"};
    for(int i=0;i<4;i++){u->slots[i].reported=true;strcpy(u->slots[i].type,i==2?"PETG":"PLA");strcpy(u->slots[i].color,colors[i]);u->slots[i].remain=i==0?82:i==1?46:i==2?71:-1;}
    bambu_ams_unit_t *ht=&s_data.units[1];ht->id=128;ht->kind=BAMBU_AMS_HT;ht->slot_count=1;
    ht->humidity=12;ht->temperature=48;ht->dry_minutes=135;ht->dry_state=2;
    ht->slots[0].reported=true;ht->slots[0].remain=46;strcpy(ht->slots[0].type,"PETG");strcpy(ht->slots[0].color,"F87D5B");
    s_data.external_present=true;s_data.vt_tray.reported=true;strcpy(s_data.vt_tray.type,"TPU");strcpy(s_data.vt_tray.color,"39B887");s_data.vt_tray.remain=-1;
    char name[80];
    for(int lang=0;lang<2;lang++) {
        chinese=lang==0;
        for(int skin=1;skin<=CONFIG_BAMBU_SKIN_COUNT;skin++) {
            prefs.skin=skin;
            for(int page=0;page<4;page++) {s_view=VIEW_MONITOR;s_page=page;snprintf(name,sizeof(name),"skin-%02d-page-%d-%s",skin,page+1,chinese?"zh":"en");shot(argv[1],name);}
        }
    }
    chinese=true;prefs.skin=1;
    for(int skin=1;skin<=18;skin++){prefs.skin=skin;s_view=VIEW_MONITOR;s_page=2;snprintf(name,sizeof(name),"skin-%02d-external",skin);shot(argv[1],name);}
    bambu_state_t sample=s_data;
    for(int skin=1;skin<=18;skin++){
        prefs.skin=skin;s_view=VIEW_MONITOR;s_page=0;s_data=sample;s_data.mc_percent=100;s_data.mc_remaining=1000000;strcpy(s_data.printer_model,"H2D Pro");
        snprintf(name,sizeof(name),"skin-%02d-progress-100",skin);shot(argv[1],name);
        s_page=page_count()-1;s_data.layer_num=1000000;s_data.total_layer=1000000;
        snprintf(name,sizeof(name),"skin-%02d-long-layers",skin);shot(argv[1],name);
        s_data=sample;s_page=1;for(int i=0;i<4;i++)s_data.units[0].slots[i].remain=-1;
        snprintf(name,sizeof(name),"skin-%02d-unknown-materials",skin);shot(argv[1],name);
        s_data=sample;s_data.unit_count=1;s_page=1;s_data.units[0].kind=BAMBU_AMS_HT;s_data.units[0].id=128;s_data.units[0].slot_count=1;s_data.active_unit_id=128;s_data.active_slot_id=0;
        snprintf(name,sizeof(name),"skin-%02d-ams-ht",skin);shot(argv[1],name);
    }
    /* Long times/model names in BOTH languages, absence and multi-HT groups. */
    for(int lang=0;lang<2;lang++)for(int skin=1;skin<=18;skin++){
        chinese=lang==0;prefs.skin=skin;s_view=VIEW_MONITOR;s_data=sample;
        strcpy(s_data.printer_model,"H2D Pro");s_data.mc_remaining=1000000;s_data.mc_percent=100;s_page=0;
        snprintf(name,sizeof(name),"edge-%02d-long-time-%s",skin,chinese?"zh":"en");shot(argv[1],name);
        s_page=2;s_data.units[1].dry_minutes=100000;s_data.units[1].dry_state=-1;
        s_data.units[1].temperature=-1;s_data.units[1].humidity=-1;
        snprintf(name,sizeof(name),"edge-%02d-ht-unknown-%s",skin,chinese?"zh":"en");shot(argv[1],name);
        s_data=sample;s_data.unit_count=0;s_page=1;s_data.vt_tray.remain=0;
        assert(page_count()==3);snprintf(name,sizeof(name),"edge-%02d-external-only-%s",skin,chinese?"zh":"en");shot(argv[1],name);
        s_data=sample;s_data.unit_count=3;s_data.units[2]=s_data.units[1];s_data.units[2].id=129;s_page=3;
        assert(page_count()==5&&!bambu_pages_external(&s_data,s_page));
        snprintf(name,sizeof(name),"edge-%02d-second-ht-%s",skin,chinese?"zh":"en");shot(argv[1],name);
    }
    for(int lang=0;lang<2;lang++)for(int skin=1;skin<=18;skin++){
        chinese=lang==0;prefs.skin=skin;s_view=VIEW_MONITOR;s_data=sample;
        s_data.nozzle_temp=s_data.nozzle_target=500;s_data.bed_temp=s_data.bed_target=200;
        s_data.cooling_fan=s_data.big_fan1=100;s_data.spd_mag=1000;
        s_data.units[0].humidity=100;s_data.units[0].temperature=150;
        for(int i=0;i<4;i++){s_data.units[0].slots[i].remain=100;strcpy(s_data.units[0].slots[i].type,"PETG-CF");}
        for(int page=0;page<4;page++){s_page=page;snprintf(name,sizeof(name),"stress-%02d-page-%d-%s",skin,page+1,chinese?"zh":"en");shot(argv[1],name);}
    }
    for(int lang=0;lang<2;lang++)for(int skin=1;skin<=18;skin++){
        chinese=lang==0;prefs.skin=skin;s_view=VIEW_MONITOR;s_data=sample;s_data.active_slot_id=3;
        s_data.units[0].kind=BAMBU_AMS_PRO;
        for(int page=0;page<2;page++){s_page=page;snprintf(name,sizeof(name),"audit-%02d-slot4-page-%d-%s",skin,page+1,chinese?"zh":"en");shot(argv[1],name);}
        s_data=sample;s_service.connected=false;
        for(int page=1;page<=2;page++){s_page=page;snprintf(name,sizeof(name),"audit-%02d-offline-material-%d-%s",skin,page,chinese?"zh":"en");shot(argv[1],name);}
        bambu_state_reset(&s_data);s_service.connected=false;
        for(int page=0;page<2;page++){s_page=page? page_count()-1:0;snprintf(name,sizeof(name),"audit-%02d-offline-page-%d-%s",skin,page+1,chinese?"zh":"en");shot(argv[1],name);}
        s_service.connected=true;
    }
    chinese=true;
    s_data=sample;prefs.skin=5;s_view=VIEW_MONITOR;s_data.mc_percent=37;s_data.mc_remaining=9;
    s_data.nozzle_temp=243;s_data.nozzle_target=140;s_data.bed_temp=s_data.bed_target=70;s_data.chamber_temp=35;s_data.valid_mask|=BAMBU_VALID_CHAMBER;
    s_data.layer_num=0;s_data.total_layer=10;s_data.cooling_fan=0;s_data.big_fan1=39;s_data.active_slot_id=0;
    const char *real_colors[]={"000000","FFFFFF","F5D800","BBCE00"};const int real_remain[]={43,17,7,28};
    for(int i=0;i<4;i++){strcpy(s_data.units[0].slots[i].color,real_colors[i]);strcpy(s_data.units[0].slots[i].type,i==0?"PETG":"PLA");s_data.units[0].slots[i].remain=real_remain[i];}
    s_data.units[0].humidity=34;s_data.units[0].temperature=30;s_data.vt_tray.remain=0;strcpy(s_data.vt_tray.type,"TPU");strcpy(s_data.vt_tray.color,"00D037");
    for(int i=0;i<4;i++){s_page=i;snprintf(name,sizeof(name),"device-replay-night-%d",i);shot(argv[1],name);}
    s_data=sample;
    prefs.skin=1;
    for(int view=VIEW_SETTINGS;view<=VIEW_NETWORK;view++) {s_view=view;snprintf(name,sizeof(name),"settings-%d",view);shot(argv[1],name);}
    chinese=false;for(int view=VIEW_SETTINGS;view<=VIEW_NETWORK;view++){s_view=view;snprintf(name,sizeof(name),"settings-%d-en",view);shot(argv[1],name);}chinese=true;
    static unsigned char home[sizeof(framebuffer)];
    for(int lang=0;lang<2;lang++){
        chinese=lang==0;
        for(int skin=1;skin<=CONFIG_BAMBU_SKIN_COUNT;skin++){
            prefs.skin=skin;s_view=VIEW_MONITOR;s_page=0;lv_obj_invalidate(s_surface);lv_refr_now(NULL);memcpy(home,framebuffer,sizeof(home));
            prefs.skin=1;s_candidate=skin;s_view=VIEW_SKIN_PICKER;s_picker_chrome=false;
            snprintf(name,sizeof(name),"skin-picker-%02d-live-%s",skin,chinese?"zh":"en");shot(argv[1],name);
            for(int y=0;y<280;y++)for(int x=55;x<305;x++)assert(!memcmp(home+(y*360+x)*3,framebuffer+(y*360+x)*3,3));
            s_picker_chrome=true;snprintf(name,sizeof(name),"skin-picker-%02d-%s",skin,chinese?"zh":"en");shot(argv[1],name);
            assert(prefs.skin==1);
        }
    }
    chinese=true;s_view=VIEW_NETWORK;
    s_service.provisioning=true;strcpy(s_service.ap_ssid,"Bambu-A1B2");shot(argv[1],"settings-provisioning");
    s_view=VIEW_MONITOR;s_page=0;bambu_state_reset(&s_data);shot(argv[1],"unknown-overview");
    s_service.connected=false;s_page=page_count()-1;shot(argv[1],"stale-details");
    s_service.connected=true;
    s_page=page_count()-1;s_last_point=(lv_point_t){278,70};s_wake_touch=false;
    on_tap(NULL);assert(s_view==VIEW_SETTINGS);
    s_last_point=(lv_point_t){180,180};on_tap(NULL);assert(s_view==VIEW_SLEEP);
    bool before=prefs.keep_printing;
    s_last_point=(lv_point_t){180,160};on_tap(NULL);assert(prefs.keep_printing!=before);
    before=prefs.keep_printing;s_asleep=true;s_wake_touch=true;
    on_tap(NULL);assert(!s_asleep&&prefs.keep_printing==before);
    s_wake_touch=false;assert(ui_monitor_back()&&s_view==VIEW_SETTINGS);
    assert(ui_monitor_back()&&s_view==VIEW_MONITOR);
    assert(!ui_monitor_back());
    picker_tests();
    s_tap_suppressed=s_wake_touch=false;
    s_view=VIEW_SLEEP;prefs.sleep_min=0;s_last_point=(lv_point_t){180,109};
    on_tap(NULL);assert(prefs.sleep_min==1);on_tap(NULL);assert(prefs.sleep_min==5);on_tap(NULL);assert(prefs.sleep_min==15);on_tap(NULL);assert(prefs.sleep_min==0);
    bool wake=prefs.wake_finish;s_last_point=(lv_point_t){180,201};on_tap(NULL);assert(prefs.wake_finish!=wake);
    wake=prefs.wake_error;s_last_point=(lv_point_t){180,247};on_tap(NULL);assert(prefs.wake_error!=wake);
    puts("PASS: redesigned hit areas, language toggle, sleep cycle 0/1/5/15, independent wake switches.");
    puts("PASS: all 18 full-size live previews match production home pixels; no browsing writes.");
    s_data.unit_count=2;s_data.units[0].kind=s_data.units[1].kind=BAMBU_AMS_STANDARD;s_data.external_present=true;assert(page_count()==5);
    puts("PASS: settings entry, preference toggle, first-tap wake consumption, back navigation, dynamic AMS page count.");
    printf("Rendered %d skin pages, settings, unknown and stale states using production LVGL view.\n",CONFIG_BAMBU_SKIN_COUNT*8);
    printf("Layout audit: %u labels, %u truncations.\n",s_audit_labels,s_audit_truncations);
    assert(!s_audit_truncations);
    return 0;
}
