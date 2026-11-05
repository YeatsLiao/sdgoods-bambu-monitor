/* SPDX-License-Identifier: MIT; shared settings from settings-flow.png. */
#define SETTINGS_ORANGE 0xFF623D
#define SETTINGS_INK 0x26231F
static void settings_background(lv_draw_ctx_t *ctx){
    rect(ctx,0,0,360,360,0xFAF4EB,0);
}
static void settings_title(lv_draw_ctx_t *ctx,const char *v){
    stroke(ctx,69,46,62,54,SETTINGS_INK,2);stroke(ctx,62,54,69,62,SETTINGS_INK,2);
    center(ctx,88,38,200,v,sdg_i18n_is_zh()?&bambu_bold_26:&bambu_bold_22,SETTINGS_INK);
}
/* Consistent 24-unit outline icons; settings semantics stay separate from
 * monitor decorations. All strokes are 2 px with round caps. */
static void settings_arc(lv_draw_ctx_t *ctx,int x,int y,int radius,int start,int end){
    lv_draw_arc_dsc_t d;lv_draw_arc_dsc_init(&d);d.width=2;d.rounded=1;d.color=lv_color_hex(SETTINGS_ORANGE);
    lv_point_t p={x,y};lv_draw_arc(ctx,&d,&p,radius,start,end);
}
static void settings_path(lv_draw_ctx_t *ctx,int x,int y,const int8_t (*points)[2],int count){
    for(int i=1;i<count;i++)stroke(ctx,x+points[i-1][0],y+points[i-1][1],x+points[i][0],y+points[i][1],SETTINGS_ORANGE,2);
}
static void settings_icon(lv_draw_ctx_t *ctx,icon_t type,int x,int y){
    uint32_t c=SETTINGS_ORANGE;
    if(type==ICON_PALETTE){
        const int8_t p[][2]={{13,2},{8,3},{4,6},{2,11},{3,16},{6,20},{11,23},{15,23},{17,21},{17,19},{15,17},{15,15},{17,13},{21,13},{23,11},{23,8},{20,4},{16,2},{13,2}};
        settings_path(ctx,x,y,p,sizeof(p)/sizeof(*p));
        rect(ctx,x+7,y+7,3,3,c,2);rect(ctx,x+13,y+5,3,3,c,2);rect(ctx,x+18,y+8,3,3,c,2);rect(ctx,x+5,y+12,3,3,c,2);
    }else if(type==ICON_BELL){
        settings_arc(ctx,x+13,y+10,7,180,360);
        const int8_t p[][2]={{6,10},{6,15},{3,19},{23,19},{20,15},{20,10}};settings_path(ctx,x,y,p,6);
        stroke(ctx,x+13,y+1,x+13,y+3,c,2);settings_arc(ctx,x+13,y+21,2,0,180);
    }else if(type==ICON_WIFI){
        settings_arc(ctx,x+13,y+23,17,226,314);settings_arc(ctx,x+13,y+23,10,226,314);rect(ctx,x+11,y+21,4,4,c,2);
    }else if(type==ICON_SUN){
        frame(ctx,x+8,y+8,11,11,c,6,2);
        const int8_t rays[][4]={{13,1,13,4},{13,22,13,25},{1,13,4,13},{22,13,25,13},{4,4,6,6},{20,20,22,22},{4,22,6,20},{20,6,22,4}};
        for(int i=0;i<8;i++)stroke(ctx,x+rays[i][0],y+rays[i][1],x+rays[i][2],y+rays[i][3],c,2);
    }else if(type==ICON_CLOCK){frame(ctx,x+2,y+2,23,23,c,12,2);stroke(ctx,x+13,y+6,x+13,y+13,c,2);stroke(ctx,x+13,y+13,x+18,y+16,c,2);
    }else if(type==ICON_CHECK){frame(ctx,x+2,y+2,23,23,c,12,2);stroke(ctx,x+7,y+13,x+11,y+17,c,2);stroke(ctx,x+11,y+17,x+19,y+9,c,2);
    }else if(type==ICON_ALERT){const int8_t p[][2]={{12,3},{2,21},{3,23},{23,23},{24,21},{14,3},{12,3}};settings_path(ctx,x,y,p,7);stroke(ctx,x+13,y+9,x+13,y+15,c,2);rect(ctx,x+12,y+18,3,3,c,2);
    }else if(type==ICON_LANGUAGE){frame(ctx,x+2,y+2,23,23,c,12,2);frame(ctx,x+8,y+2,11,23,c,6,1);stroke(ctx,x+3,y+13,x+24,y+13,c,2);
    }else icon(ctx,type,x,y,c);
}
static void settings_row(lv_draw_ctx_t *ctx,int y,icon_t symbol,const char *label){
    rect(ctx,45,y,270,48,0xFFFEFC,16);settings_icon(ctx,symbol,60,y+10);
    text(ctx,101,y+11,186,label,&bambu_bold_22,SETTINGS_INK,LV_TEXT_ALIGN_LEFT);
    stroke(ctx,294,y+19,300,y+24,SETTINGS_INK,2);stroke(ctx,300,y+24,294,y+29,SETTINGS_INK,2);
}
static void settings_switch(lv_draw_ctx_t *ctx,int y,bool enabled){
    rect(ctx,260,y+10,43,25,enabled?SETTINGS_ORANGE:0xD7D1C8,13);rect(ctx,enabled?282:263,y+13,19,19,0xFFFFFF,10);
}
static void settings_button(lv_draw_ctx_t *ctx,int y,const char *v,bool solid){
    if(solid)rect(ctx,104,y,152,38,SETTINGS_ORANGE,20);else frame(ctx,104,y,152,38,SETTINGS_ORANGE,20,2);
    center(ctx,108,y+4,144,v,&bambu_bold_22,solid?0xFFFFFF:SETTINGS_ORANGE);
}
/* The live home renderer is reused at full size; this chrome is the only
 * difference while browsing. Candidate selection never changes preferences. */
static void draw_skin_picker_controls(lv_draw_ctx_t *ctx){
    int clear_y=design_id()==1?265:design_id()==2?275:design_id()<=6?296:267;
    rect(ctx,0,clear_y,360,360-clear_y,s_skin->bg,0);
    uint32_t bg=mixed(s_skin->bg,s_skin->ink,235);
    rect(ctx,78,295,204,52,bg,26);
    rect(ctx,92,302,30,30,mixed(s_skin->ink,s_skin->bg,26),15);
    stroke(ctx,102,312,112,322,s_skin->ink,2);stroke(ctx,112,312,102,322,s_skin->ink,2);
    rect(ctx,238,302,30,30,s_skin->accent,15);
    uint32_t check=design_id()==5?0x272039:0xFFFFFF;
    stroke(ctx,246,317,251,322,check,2);stroke(ctx,251,322,260,312,check,2);
    const monitor_skin_t *candidate=monitor_skin_get(s_candidate);
    lv_draw_label_dsc_t label;lv_draw_label_dsc_init(&label);label.font=sdg_i18n_is_zh()?&bambu_bold_18:&bambu_font_16;label.color=lv_color_hex(s_skin->ink);label.align=LV_TEXT_ALIGN_CENTER;label.letter_space=sdg_i18n_is_zh()?0:-1;
    lv_area_t name={122,300,237,326};lv_draw_label(ctx,&label,&name,sdg_i18n_is_zh()?candidate->zh:candidate->en,NULL);
    char count[24];snprintf(count,sizeof(count),"%02d / %02d",s_candidate,CONFIG_BAMBU_SKIN_COUNT);center(ctx,140,325,80,count,&bambu_font_16,s_skin->ink);
    for(int side=0;side<2;side++){
        int x=side?321:13;rect(ctx,x,165,26,34,bg,13);
        stroke(ctx,x+(side?10:16),176,x+(side?17:9),182,s_skin->ink,2);stroke(ctx,x+(side?17:9),182,x+(side?10:16),188,s_skin->ink,2);
    }
}
static void appearance_row(lv_draw_ctx_t *ctx,int y,icon_t symbol,const char *label,const char *value){
    rect(ctx,45,y,270,60,0xFFFEFC,16);settings_icon(ctx,symbol,60,y+17);
    text(ctx,101,y+7,183,label,&bambu_bold_22,SETTINGS_INK,LV_TEXT_ALIGN_LEFT);
    text(ctx,102,y+35,180,value,&bambu_font_16,0x74675F,LV_TEXT_ALIGN_LEFT);
    stroke(ctx,294,y+25,300,y+30,SETTINGS_INK,2);stroke(ctx,300,y+30,294,y+35,SETTINGS_INK,2);
}
static void draw_settings(lv_draw_ctx_t *ctx){
    const monitor_preferences_t *p=monitor_preferences_get();char b[96];settings_background(ctx);
    settings_title(ctx,s_view==VIEW_SETTINGS?SDG_T("应用设置","App settings"):s_view==VIEW_APPEARANCE?SDG_T("外观与语言","Appearance"):s_view==VIEW_SLEEP?SDG_T("熄屏设置","Screen & alerts"):SDG_T("网络与打印机","Network setup"));
    if(s_view==VIEW_SETTINGS){
        settings_row(ctx,96,ICON_PALETTE,SDG_T("外观与语言","Appearance"));
        settings_row(ctx,152,ICON_BELL,SDG_T("熄屏与提醒","Screen & alerts"));
        settings_row(ctx,208,ICON_WIFI,SDG_T("网络与打印机","Network setup"));
        center(ctx,48,280,264,SDG_T("亮度、声音请在下拉控制中心调整","Brightness/audio: swipe down"),&bambu_font_16,0x6B6359);
    }else if(s_view==VIEW_SLEEP){
        const char *names[]={SDG_T("自动熄屏","Screen timeout"),SDG_T("打印时常亮","Keep on printing"),SDG_T("完成时亮屏","Wake on finish"),SDG_T("异常时亮屏","Wake on error")};
        icon_t icons[]={ICON_CLOCK,ICON_SUN,ICON_CHECK,ICON_ALERT};
        for(int i=0;i<4;i++){
            int y=89+i*46;rect(ctx,45,y,270,40,0xFFFEFC,14);settings_icon(ctx,icons[i],59,y+7);
            text(ctx,99,y+10,159,names[i],sdg_i18n_is_zh()?&bambu_bold_18:&bambu_font_16,SETTINGS_INK,LV_TEXT_ALIGN_LEFT);
            if(i==0){if(p->sleep_min)snprintf(b,sizeof(b),SDG_T("%d 分钟","%d min"),p->sleep_min);else strcpy(b,SDG_T("永不","Never"));text(ctx,230,y+10,71,b,&bambu_font_16,SETTINGS_INK,LV_TEXT_ALIGN_RIGHT);}
            else settings_switch(ctx,y-2,i==1?p->keep_printing:i==2?p->wake_finish:p->wake_error);
        }
        center(ctx,70,292,220,SDG_T("熄屏后继续监控","Monitoring continues"),&bambu_font_16,0x6B6359);
    }else if(s_view==VIEW_APPEARANCE){
        const monitor_skin_t *current=monitor_skin_get(p->skin);
        snprintf(b,sizeof(b),"%02d · %s",p->skin,sdg_i18n_is_zh()?current->zh:current->en);
        appearance_row(ctx,106,ICON_PALETTE,SDG_T("选择皮肤","Choose skin"),b);
        appearance_row(ctx,184,ICON_LANGUAGE,SDG_T("界面语言","Language"),sdg_i18n_is_zh()?"简体中文":"English");
        center(ctx,61,280,238,SDG_T("主屏长按也可选择皮肤","Or hold the home screen"),&bambu_font_16,0x74675F);
    }else{
        settings_icon(ctx,ICON_WIFI,167,84);
        if(s_service.busy){center(ctx,58,162,244,SDG_T("正在切换网络…","Changing network…"),&bambu_bold_22,SETTINGS_INK);return;}
        if(s_service.provisioning){
            rect(ctx,53,124,254,65,0xFFFEFC,18);center(ctx,60,129,240,SDG_T("连接设备热点","Join device hotspot"),&bambu_bold_18,SETTINGS_INK);
            rect(ctx,99,156,162,27,0xFFE5D7,13);center(ctx,78,156,204,s_service.ap_ssid,&bambu_bold_22,0xE34F2E);
            rect(ctx,53,198,254,65,0xFFFEFC,18);center(ctx,60,203,240,SDG_T("浏览器打开","Open in browser"),&bambu_bold_18,SETTINGS_INK);
            rect(ctx,99,230,162,27,0xFFE5D7,13);center(ctx,94,230,172,"192.168.4.1",&bambu_bold_22,0xE34F2E);
            center(ctx,64,274,232,SDG_T("填写 Wi-Fi 和打印机信息","Enter Wi-Fi & printer details"),&bambu_font_16,0x6B6359);
            settings_button(ctx,305,SDG_T("取消配网","Cancel setup"),false);
        }else{
            center(ctx,50,131,260,state_name(),&bambu_bold_22,SETTINGS_INK);
            center(ctx,65,177,230,SDG_T("通过手机配置 Wi-Fi 和打印机","Set up using your phone"),&bambu_font_16,0x6B6359);
            center(ctx,64,217,232,SDG_T("原配置保留至新连接成功","Keep current setup until ready"),&bambu_font_16,0x6B6359);
            settings_button(ctx,276,SDG_T("开启配网","Start setup"),true);
        }
    }
}
