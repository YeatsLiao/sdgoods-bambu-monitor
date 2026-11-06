/* SPDX-License-Identifier: MIT
 * Batch 01: distinct layouts from docs/design, with live telemetry only.
 * Included by ui_monitor.c to keep ownership of LVGL state in one translation unit.
 */
LV_FONT_DECLARE(bambu_bold_18);LV_FONT_DECLARE(bambu_bold_22);LV_FONT_DECLARE(bambu_bold_26);
LV_FONT_DECLARE(bambu_num_44);LV_FONT_DECLARE(bambu_num_64);LV_FONT_DECLARE(bambu_num_88);LV_FONT_DECLARE(bambu_num_128);

static int design_id(void){return active_skin_id();}
static uint32_t mixed(uint32_t a,uint32_t b,int amount){
    return lv_color_to32(lv_color_mix(lv_color_hex(a),lv_color_hex(b),amount))&0xFFFFFF;
}
static void stroke(lv_draw_ctx_t *ctx,int x1,int y1,int x2,int y2,uint32_t color,int width){
    lv_draw_line_dsc_t d;lv_draw_line_dsc_init(&d);d.color=lv_color_hex(color);d.width=width;d.round_start=d.round_end=1;
    lv_point_t a={x1,y1},b={x2,y2};lv_draw_line(ctx,&d,&a,&b);
}
static void frame(lv_draw_ctx_t *ctx,int x,int y,int w,int h,uint32_t color,int radius,int width){
    lv_draw_rect_dsc_t d;lv_draw_rect_dsc_init(&d);d.bg_opa=LV_OPA_TRANSP;d.border_color=lv_color_hex(color);d.border_width=width;d.radius=radius;
    lv_area_t a={x,y,x+w-1,y+h-1};lv_draw_rect(ctx,&d,&a);
}
static void center(lv_draw_ctx_t *ctx,int x,int y,int w,const char *v,const lv_font_t *font,uint32_t color){
    text(ctx,x,y,w,v,font,color,LV_TEXT_ALIGN_CENTER);
}
static void dashline(lv_draw_ctx_t *ctx,int x,int y,int w,uint32_t color){
    for(int n=0;n<w;n+=7)rect(ctx,x+n,y,3,2,color,0);
}
typedef enum{ICON_PRINT,ICON_BED,ICON_TEMP,ICON_SPEED,ICON_WIFI,ICON_CLOCK,ICON_BELL,ICON_CHECK,ICON_ALERT,ICON_PALETTE,ICON_SUN,ICON_LANGUAGE} icon_t;
static void icon(lv_draw_ctx_t *ctx,icon_t type,int x,int y,uint32_t c){
    if(type==ICON_PRINT){frame(ctx,x+2,y+8,22,13,c,3,2);frame(ctx,x+6,y,14,9,c,1,2);stroke(ctx,x+7,y+25,x+19,y+25,c,2);stroke(ctx,x+7,y+18,x+7,y+25,c,2);stroke(ctx,x+19,y+18,x+19,y+25,c,2);rect(ctx,x+19,y+11,3,3,c,2);}
    else if(type==ICON_BED){stroke(ctx,x+1,y+22,x+24,y+22,c,2);for(int i=0;i<3;i++){int a=x+5+i*7;stroke(ctx,a,y+2,a-2,y+7,c,2);stroke(ctx,a-2,y+7,a+1,y+12,c,2);stroke(ctx,a+1,y+12,a-1,y+17,c,2);}}
    else if(type==ICON_TEMP){frame(ctx,x+9,y,7,20,c,4,2);rect(ctx,x+7,y+16,11,11,c,6);stroke(ctx,x+12,y+7,x+12,y+21,c,2);}
    else if(type==ICON_SPEED){lv_draw_arc_dsc_t d;lv_draw_arc_dsc_init(&d);d.width=2;d.color=lv_color_hex(c);lv_point_t p={x+13,y+16};lv_draw_arc(ctx,&d,&p,12,180,360);stroke(ctx,x+13,y+16,x+20,y+7,c,2);stroke(ctx,x+2,y+17,x+24,y+17,c,2);}
    else if(type==ICON_WIFI){lv_draw_arc_dsc_t d;lv_draw_arc_dsc_init(&d);d.width=3;d.rounded=1;d.color=lv_color_hex(c);lv_point_t p={x+13,y+22};lv_draw_arc(ctx,&d,&p,18,222,318);lv_draw_arc(ctx,&d,&p,11,222,318);rect(ctx,x+10,y+19,6,6,c,3);}
    else if(type==ICON_CLOCK){frame(ctx,x+1,y+1,24,24,c,12,2);stroke(ctx,x+13,y+6,x+13,y+13,c,2);stroke(ctx,x+13,y+13,x+19,y+13,c,2);}
    else if(type==ICON_CHECK){frame(ctx,x+1,y+1,24,24,c,12,2);stroke(ctx,x+7,y+13,x+11,y+17,c,2);stroke(ctx,x+11,y+17,x+20,y+8,c,2);}
    else if(type==ICON_BELL){frame(ctx,x+5,y+5,17,17,c,8,2);rect(ctx,x+3,y+20,21,3,c,2);rect(ctx,x+11,y+25,5,3,c,3);rect(ctx,x+12,y+1,3,4,c,2);}
    else if(type==ICON_ALERT){stroke(ctx,x+13,y+1,x+1,y+24,c,2);stroke(ctx,x+1,y+24,x+25,y+24,c,2);stroke(ctx,x+25,y+24,x+13,y+1,c,2);stroke(ctx,x+13,y+9,x+13,y+15,c,2);rect(ctx,x+12,y+19,3,3,c,2);}
    else {frame(ctx,x+1,y+1,25,24,c,12,2);for(int i=0;i<4;i++)rect(ctx,x+5+(i%2)*9,y+5+(i/2)*8,4,4,c,3);}
}
/* Detail telemetry glyphs share a 24px box and a 2px optical weight.
 * Keep the document/printer symbols used by navigation out of these cards. */
static void metric_icon(lv_draw_ctx_t *ctx,int x,int y,int metric,uint32_t c){
    if(metric==0){
        frame(ctx,x+8,y+1,9,5,c,1,2);
        frame(ctx,x+4,y+8,17,9,c,2,2);
        stroke(ctx,x+8,y+18,x+12,y+23,c,2);stroke(ctx,x+12,y+23,x+17,y+18,c,2);
        stroke(ctx,x+9,y+12,x+16,y+12,c,2);
    }else if(metric==1){
        lv_draw_arc_dsc_t d;lv_draw_arc_dsc_init(&d);d.width=2;d.rounded=1;d.color=lv_color_hex(c);
        for(int i=0;i<2;i++){
            lv_point_t a={x+7+i*9,y+3},b={x+7+i*9,y+8};
            lv_draw_arc(ctx,&d,&a,3,270,90);lv_draw_arc(ctx,&d,&b,3,90,270);
        }
        frame(ctx,x+2,y+14,21,5,c,2,2);
        stroke(ctx,x+6,y+22,x+19,y+22,c,2);
    }else if(metric==2){
        frame(ctx,x+2,y+2,21,21,c,4,2);
        frame(ctx,x+11,y+6,4,9,c,2,2);rect(ctx,x+9,y+13,8,7,c,4);
        stroke(ctx,x+6,y+7,x+7,y+7,c,1);stroke(ctx,x+6,y+11,x+7,y+11,c,1);
    }else{
        lv_draw_arc_dsc_t d;lv_draw_arc_dsc_init(&d);d.width=2;d.rounded=1;d.color=lv_color_hex(c);
        lv_point_t p={x+12,y+15};lv_draw_arc(ctx,&d,&p,10,180,360);
        stroke(ctx,x+2,y+18,x+22,y+18,c,2);
        stroke(ctx,x+12,y+15,x+17,y+8,c,2);rect(ctx,x+10,y+13,5,5,c,3);
    }
}
static void unknown_bar(lv_draw_ctx_t *ctx,int x,int y,int w,int h,uint32_t c){
    // Unknown is not an empty spool: short, evenly inset dashes on the midline.
    int count=w>=70?5:3,step=10,total=(count-1)*step+4;
    for(int i=0;i<count;i++)rect(ctx,x+(w-total)/2+i*step,y+(h-2)/2,4,2,c,1);
}
static void cat_peek(lv_draw_ctx_t *ctx,int x,int y,int width){
#if CONFIG_BAMBU_SKIN_COUNT > 1
    LV_IMG_DECLARE(bambu_cat_peek);
    lv_draw_img_dsc_t d;lv_draw_img_dsc_init(&d);d.antialias=1;
    d.zoom=width*256/96;d.pivot=(lv_point_t){0,0};
    lv_area_t area={x,y,x+95,y+63};lv_draw_img(ctx,&d,&area,&bambu_cat_peek);
#else
    (void)ctx;(void)x;(void)y;(void)width;
#endif
}
#define DETAIL_SETTINGS_CX 287
#define DETAIL_SETTINGS_CY 65
#define DETAIL_SETTINGS_RADIUS 14
static bool detail_settings_hit(lv_point_t p){
    int dx=p.x-DETAIL_SETTINGS_CX,dy=p.y-DETAIL_SETTINGS_CY;
    return dx*dx+dy*dy<=22*22;
}
static void design_gear(lv_draw_ctx_t *ctx,int cx,int cy,uint32_t c){
    // All geometry shares one center; font side bearings cannot shift the gear.
    rect(ctx,cx-14,cy-14,29,29,mixed(s_skin->bg,s_skin->ink,235),15);
    frame(ctx,cx-14,cy-14,29,29,mixed(s_skin->ink,s_skin->bg,65),15,1);
    static const int8_t teeth[][4]={{0,-8,0,-10},{6,-6,7,-7},{8,0,10,0},{6,6,7,7},
        {0,8,0,10},{-6,6,-7,7},{-8,0,-10,0},{-6,-6,-7,-7}};
    for(unsigned i=0;i<8;i++)stroke(ctx,cx+teeth[i][0],cy+teeth[i][1],cx+teeth[i][2],cy+teeth[i][3],c,3);
    lv_draw_arc_dsc_t d;lv_draw_arc_dsc_init(&d);d.width=3;d.color=lv_color_hex(c);
    lv_point_t center={cx,cy};lv_draw_arc(ctx,&d,&center,8,0,360);
}
static const char *matrix_pattern(char c){
    static const char *p[]={"01110100011000110001100011000101110","00100011000010000100001000010001110","01110100010000100010001000100011111","11110000010000101110000010000111110","00010001100101010010111110001000010","11111100001000011110000010000111110","01110100001000011110100011000101110","11111000010001000100010000100001000","01110100011000101110100011000101110","01110100011000101111000010000101110"};
    if(c>='0'&&c<='9')return p[c-'0'];
    if(c=='A')return "01110100011000111111100011000110001";
    if(c=='M')return "10001110111010110101100011000110001";
    if(c=='S')return "01111100001000001110000010000111110";
    if(c=='H')return "10001100011000111111100011000110001";
    if(c=='T')return "11111001000010000100001000010000100";
    if(c==' ')return "00000000000000000000000000000000000";
    if(c=='%')return "11001110100001000100010001001110011";
    if(c=='/')return "00001000100001000100010000100010000";
    return "00000000000000011111000000000000000";
}
static void matrix(lv_draw_ctx_t *ctx,int x,int y,int width,const char *v,int cell,uint32_t c){
    int len=strlen(v),total=len*6*cell-cell;x+=(width-total)/2;
    for(int n=0;n<len;n++){const char *p=matrix_pattern(v[n]);for(int j=0;j<7;j++)for(int i=0;i<5;i++)if(p[j*5+i]=='1'){
        int px=x+n*6*cell+i*cell,py=y+j*cell;
        if(cell>=10){int dot=cell/2;for(int dy=0;dy<2;dy++)for(int dx=0;dx<2;dx++)rect(ctx,px+dx*dot,py+dy*dot,dot-1,dot-1,c,0);}
        else rect(ctx,px,py,cell-1,cell-1,c,0);
    }}
}
static void design_pill(lv_draw_ctx_t *ctx,int x,int y,int width,const char *v,uint32_t bg,uint32_t ink){
    rect(ctx,x,y,width,26,bg,13);center(ctx,x,y+2,width,v,&bambu_bold_18,ink);
}
static int design_band_width(int y,int height){
    int dy=LV_MAX(abs(y-180),abs(y+height-1-180)),half=0;
    while((half+1)*(half+1)+dy*dy<=174*174)half++;
    return LV_MAX(0,2*half-4);
}
static void design_progress(lv_draw_ctx_t *ctx,int y,const lv_font_t *font,uint32_t color){
    char b[16];if(s_data.valid_mask&BAMBU_VALID_PROGRESS)snprintf(b,sizeof(b),"%d",s_data.mc_percent);else strcpy(b,"--");
    int baseline=y+font->line_height-font->base_line,bw=0,uw=0,py=y;
    const lv_font_t *unit=&bambu_bold_26;
    for(int attempt=0;attempt<4;attempt++){
        unit=font==&bambu_num_128?&bambu_num_64:font==&bambu_num_88?&bambu_num_44:&bambu_bold_26;
        bw=lv_txt_get_width(b,strlen(b),font,0,0);uw=lv_txt_get_width("%",1,unit,0,0);py=baseline-font->line_height+font->base_line;
        if(bw+uw+3<=LV_MIN(292,design_band_width(py,font->line_height)))break;
        font=font==&bambu_num_128?&bambu_num_88:font==&bambu_num_88?&bambu_num_64:&bambu_num_44;
    }
    int x=(360-bw-uw-3)/2;
    text(ctx,x,py,bw+2,b,font,color,LV_TEXT_ALIGN_LEFT);
    text(ctx,x+bw+3,baseline-unit->line_height+unit->base_line,uw+2,"%",unit,color,LV_TEXT_ALIGN_LEFT);
}
static const char *printer_model(void){return s_data.printer_model[0]?s_data.printer_model:"BAMBU";}
static void duration(char *b,size_t size,int minutes){
    if(minutes<0)snprintf(b,size,"--");
    else if(minutes>=1440)snprintf(b,size,SDG_T("%d天 %02d时 %02d分","%dd %02dh %02dm"),minutes/1440,minutes/60%24,minutes%60);
    else if(minutes>=60)snprintf(b,size,SDG_T("%d小时 %02d分","%dh %02dm"),minutes/60,minutes%60);
    else snprintf(b,size,SDG_T("%d分钟","%d min"),minutes);
}
static void design_remaining(lv_draw_ctx_t *ctx,int y){
    char b[80],time[48];duration(time,sizeof(time),s_data.valid_mask&BAMBU_VALID_REMAIN?s_data.mc_remaining:-1);
    snprintf(b,sizeof(b),SDG_T("剩余 %s","Remaining %s"),time);
    center(ctx,46,y,268,b,sdg_i18n_is_zh()?&bambu_bold_22:&bambu_bold_18,s_skin->ink);
}
static void design_temps(lv_draw_ctx_t *ctx,int y,bool stacked){
    char a[20],b[20];value(a,sizeof(a),BAMBU_VALID_NOZZLE,s_data.nozzle_temp,"°");value(b,sizeof(b),BAMBU_VALID_BED,s_data.bed_temp,"°");
    uint32_t c=design_id()==5?s_skin->ink:design_id()==2?s_skin->ink:0xF15B3E;
    if(stacked){center(ctx,83,y,85,SDG_T("喷嘴","Nozzle"),&bambu_bold_18,s_skin->ink);center(ctx,193,y,85,SDG_T("热床","Bed"),&bambu_bold_18,s_skin->ink);center(ctx,75,y+19,100,a,&bambu_bold_26,c);center(ctx,185,y+19,100,b,&bambu_bold_26,c);stroke(ctx,180,y+3,180,y+46,s_skin->card,1);}
    else{center(ctx,69,y+3,42,SDG_T("喷嘴","Noz"),&bambu_font_16,s_skin->ink);center(ctx,111,y,60,a,&bambu_bold_22,c);stroke(ctx,180,y+2,180,y+23,s_skin->card,1);center(ctx,188,y+3,42,SDG_T("热床","Bed"),&bambu_font_16,s_skin->ink);center(ctx,232,y,58,b,&bambu_bold_22,c);}
}
#include "ui_monitor_layout.inc"
static uint32_t tray_color(const bambu_ams_tray_t *t){
    if(!t->reported||strlen(t->color)<6)return 0xA09C9B;
    uint32_t c=strtoul(t->color,NULL,16);return strlen(t->color)>6?c>>8:c;
}
static void spool(lv_draw_ctx_t *ctx,int x,int y,int w,int h,uint32_t color){
    uint32_t rim=0x303A45;int cap=h/4;if(cap<4)cap=4;if(cap>14)cap=14;
    rect(ctx,x+3,y+cap/2,w-6,h-cap,mixed(color,0x101522,200),3);
    rect(ctx,x+4,y+cap/2+2,w-8,h-cap-4,color,2);
    for(int j=y+cap+2;j<y+h-cap-2;j+=4)stroke(ctx,x+5,j,x+w-6,j,mixed(color,0xFFFFFF,205),1);
    rect(ctx,x,y,w,cap+3,rim,(cap+3)/2);rect(ctx,x+3,y+2,w-6,cap/2,0x627380,3);rect(ctx,x+w/2-w/7,y+2,w/3,cap/2,0x111822,3);
    rect(ctx,x,y+h-cap-2,w,cap+2,rim,(cap+2)/2);stroke(ctx,x+4,y+h-4,x+w-5,y+h-4,0x65717D,1);
}
static void side_spool(lv_draw_ctx_t *ctx,int x,int y,uint32_t color){
    rect(ctx,x,y,21,48,0x20292A,11);rect(ctx,x+3,y+3,15,42,0xB7BEB7,9);
    rect(ctx,x+10,y+5,27,38,color,3);for(int j=0;j<5;j++)stroke(ctx,x+13+j*4,y+8,x+13+j*4,y+39,mixed(color,0xFFFFFF,205),1);
    rect(ctx,x+30,y,21,48,0x20292A,11);rect(ctx,x+33,y+3,15,42,0xC4C7BC,9);rect(ctx,x+37,y+12,8,25,0x263132,5);
}
static void design_dots(lv_draw_ctx_t *ctx){
    int count=page_count();if(count>6){char b[32];snprintf(b,sizeof(b),"%d / %d",s_page+1,count);center(ctx,130,329,100,b,&bambu_font_16,s_skin->ink);return;}
    for(int i=0;i<count;i++)rect(ctx,180-count*10+i*20+5,334,10,10,i==s_page?(design_id()==5&&s_page==0?0xBFA0EA:s_skin->accent):mixed(s_skin->ink,s_skin->bg,70),5);
}
static void batch1_overview(lv_draw_ctx_t *ctx){
    int id=design_id(),progress=s_data.valid_mask&BAMBU_VALID_PROGRESS?s_data.mc_percent:-1;
    if(id==1){
        center(ctx,100,20,160,printer_model(),&bambu_bold_26,s_skin->ink);
        rect(ctx,111,59,15,15,s_skin->accent,1);center(ctx,131,51,136,state_name(),&bambu_bold_26,s_skin->ink);
        design_progress(ctx,77,&bambu_num_128,s_skin->ink);
        rect(ctx,48,204,264,18,s_skin->card,9);if(progress>=0)rect(ctx,48,204,264*progress/100,18,s_skin->accent,9);
        design_remaining(ctx,228);design_temps(ctx,265,true);
    }else if(id==2){
        rect(ctx,109,32,14,14,s_skin->accent,7);center(ctx,129,24,127,printer_model(),&bambu_bold_22,s_skin->ink);
        icon(ctx,ICON_WIFI,264,52,s_service.connected?0x24D638:0x717568);
        center(ctx,72,56,216,state_name(),&bambu_bold_26,s_skin->ink);dashline(ctx,49,96,262,s_skin->ink);
        char b[12];if(progress>=0)snprintf(b,sizeof(b),"%d%%",progress);else strcpy(b,"--%");
        matrix(ctx,30,113,300,b,strlen(b)>3?10:13,s_skin->ink);dashline(ctx,49,215,262,s_skin->ink);
        design_remaining(ctx,229);design_temps(ctx,280,false);
    }else if(id>=3&&id<=5){
        int w=id==3?192:id==4?264:288;
        monitor_skin_scene(ctx,id,(360-w)/2,id==3?15:id==4?23:12,w);
        int mx=id==3?128:id==4?222:196,my=id==3?51:id==4?65:91,mw=id==3?104:id==4?74:92;
        rect(ctx,mx,my,mw,22,id==5?0x302D40:0x183F36,3);
        center(ctx,mx+2,my+1,mw-4,printer_model(),&bambu_bold_18,0xFFF4D5);
        design_progress(ctx,172,&bambu_num_88,id==5?s_skin->accent:s_skin->ink);
        uint32_t c=id==5?0xBFA0EA:s_skin->accent;
        rect(ctx,58,251,244,13,s_skin->card,7);if(progress>=0)rect(ctx,58,251,244*progress/100,13,c,7);
        design_remaining(ctx,270);design_temps(ctx,296,false);
        if(s_data.state!=BAMBU_STATE_RUNNING||!s_service.connected)design_pill(ctx,104,142,152,state_name(),s_skin->card,s_skin->ink);
    }else{
        center(ctx,80,23,120,printer_model(),&bambu_bold_22,s_skin->ink);design_pill(ctx,206,25,66,s_data.unit_count?"AMS":s_data.external_present?"EXT":"LAN",s_skin->accent,0xFFFFFF);
        design_progress(ctx,53,&bambu_num_128,s_skin->ink);design_remaining(ctx,180);
        int slots=s_data.unit_count?s_data.units[0].slot_count:s_data.external_present?1:0;
        if(!slots)center(ctx,70,236,220,state_name(),&bambu_bold_22,s_skin->ink);
        for(int i=0;i<slots;i++){
            int x=slots==1?155:43+i*73;const bambu_ams_tray_t *t=s_data.unit_count?&s_data.units[0].slots[i]:&s_data.vt_tray;
            if(s_data.unit_count&&s_data.active_unit_id==s_data.units[0].id&&s_data.active_slot_id==i)frame(ctx,x-9,212,68,81,s_skin->ink,17,2);
            spool(ctx,x,224,49,44,t?tray_color(t):0xA09C9B);
            char n[12];snprintf(n,sizeof(n),"%d",i+1);center(ctx,x,269,49,n,&bambu_bold_18,s_skin->ink);
        }
        stroke(ctx,62,293,298,293,s_skin->card,1);design_temps(ctx,296,false);
    }
}
static void materials_footer(lv_draw_ctx_t *ctx,const bambu_ams_unit_t *u){
    char b[80],rh[16],temp[16];
    if(u){
        if(u->humidity>=0)snprintf(rh,sizeof(rh),"%d%%",u->humidity);else strcpy(rh,"--");
        if(u->temperature>=0)snprintf(temp,sizeof(temp),"%.0f°C",(double)u->temperature);else strcpy(temp,"--");
        snprintf(b,sizeof(b),"%s %s  ·  %s",SDG_T("湿度","Humidity"),rh,temp);
        center(ctx,64,290,232,b,&bambu_bold_18,s_skin->ink);
    }
}
/* The same sidebar+chamber structure is recolored by the active theme.
 * Device identity, usage, sensors and drying belong to their own hardware. */
static const char *dry_name(int state){
    switch(state){
        case 0:return SDG_T("待机","Standby");case 1:return SDG_T("检查中","Checking");
        case 2:return SDG_T("烘干中","Drying");case 3:return SDG_T("冷却中","Cooling");
        case 4:return SDG_T("停止中","Stopping");case 5:case 6:return SDG_T("烘干异常","Drying error");
        default:return SDG_T("烘干 --","Drying --");
    }
}
static void ht_chamber(lv_draw_ctx_t *ctx,int x,int y,uint32_t filament){
    uint32_t edge=design_id()==5?0xE1CBF8:s_skin->ink;
    rect(ctx,x,y,84,77,mixed(s_skin->card,s_skin->bg,170),27);
    frame(ctx,x,y,84,77,edge,27,2);
    side_spool(ctx,x+19,y+14,filament);
    rect(ctx,x-2,y+58,88,20,s_skin->card,3);frame(ctx,x-2,y+58,88,20,edge,3,2);
    for(int i=0;i<3;i++){int a=x+30+i*9;stroke(ctx,a,y+62,a-2,y+67,s_skin->accent,2);stroke(ctx,a-2,y+67,a+1,y+72,s_skin->accent,2);}
}
static void tray_reading(char *out,size_t size,const bambu_ams_tray_t *t,bool external){
    /* External zero is a frequent unsupported estimate; do not claim empty. */
    if(t->reported && t->remain>=0 && (!external || t->remain>0)) snprintf(out,size,"%.0f%%",(double)t->remain);
    else snprintf(out,size,"--");
}
static void batch1_accessories(lv_draw_ctx_t *ctx){
    int index=bambu_pages_unit(&s_data,s_page),id=design_id();
    const bambu_ams_unit_t *u=index>=0?&s_data.units[index]:NULL;
    bool ext=bambu_pages_external(&s_data,s_page);
    char row[96],remain[24],rh[20],temp[20],time[48];
    if(id>=13)center(ctx,123,30,140,ext&&u?SDG_T("外置 + HT","Ext + HT"):u?"AMS HT":SDG_T("外置耗材","External spool"),&bambu_bold_22,s_skin->ink);
    else if(id>=7&&id<=11)center(ctx,126,32,144,ext&&u?SDG_T("外置 + HT","Ext + HT"):u?"AMS HT":SDG_T("外置耗材","External spool"),&bambu_bold_22,s_skin->ink);
    else center(ctx,52,33,256,ext&&u?SDG_T("外置 / AMS HT","External / AMS HT"):u?"AMS HT":SDG_T("外置耗材","External spool"),&bambu_bold_26,s_skin->ink);
    if(ext){
        int offset=id>=13?12:0;
        const bambu_ams_tray_t *t=&s_data.vt_tray;
        if(id==5){rect(ctx,40,82,280,66,0x49415D,18);frame(ctx,40,82,280,66,0xAC94C7,18,1);}
        side_spool(ctx,72,88+offset,tray_color(t));stroke(ctx,140,89+offset,140,136+offset,s_skin->card,1);
        text(ctx,154,87+offset,81,SDG_T("外置料架","Spool"),&bambu_bold_18,s_skin->ink,LV_TEXT_ALIGN_LEFT);
        tray_reading(remain,sizeof(remain),t,true);
        snprintf(row,sizeof(row),"%s  %s",t->reported&&t->type[0]?t->type:"--",remain);
        text(ctx,154,116+offset,154,row,&bambu_bold_18,s_skin->ink,LV_TEXT_ALIGN_LEFT);
        bool active=s_data.active_tray==254;
        design_pill(ctx,240,85+offset,77,active?SDG_T("使用中","In use"):SDG_T("未使用","Idle"),active?s_skin->accent:s_skin->card,active?0xFFFFFF:s_skin->ink);
        if(id==2)dashline(ctx,39,157,282,s_skin->ink);else stroke(ctx,42,157+offset,318,157+offset,s_skin->card,1);
    }
    if(u){
        int top=ext?181:115;
        rect(ctx,44,top,272,ext?118:180,id==4?0xFFF9E8:id==5?0x49415D:s_skin->bg,23);
        frame(ctx,44,top,272,ext?118:180,id==5?0xB89DDF:id==2||id==6?s_skin->accent:s_skin->ink,23,2);
        if(id==4)cat_peek(ctx,66,top-44,72);
        ht_chamber(ctx,56,top+11,tray_color(&u->slots[0]));
        snprintf(row,sizeof(row),"AMS HT %d",bambu_pages_ordinal(&s_data,index));
        if(id==2)matrix(ctx,154,top+12,154,row,3,s_skin->ink);
        else text(ctx,154,top+4,154,row,&bambu_bold_26,s_skin->ink,LV_TEXT_ALIGN_LEFT);
        tray_reading(remain,sizeof(remain),&u->slots[0],false);
        snprintf(row,sizeof(row),"%s  %s",u->slots[0].reported&&u->slots[0].type[0]?u->slots[0].type:"--",remain);
        text(ctx,154,top+38,154,row,&bambu_bold_18,s_skin->ink,LV_TEXT_ALIGN_LEFT);
        bool active=s_data.active_unit_id==u->id && s_data.active_slot_id==0;
        design_pill(ctx,176,top+64,130,active?SDG_T("使用中","In use"):SDG_T("未使用","Idle"),active?s_skin->accent:s_skin->card,active?0xFFFFFF:s_skin->ink);
        int sensors=ext?top+93:top+108;
        if(u->temperature>=0)snprintf(temp,sizeof(temp),"%.0f°C",(double)u->temperature);else strcpy(temp,"--");
        if(u->humidity>=0)snprintf(rh,sizeof(rh),"%d%%",u->humidity);else strcpy(rh,"--");
        snprintf(row,sizeof(row),SDG_T("仓温 %s  ·  湿度 %s","Temp %s  ·  RH %s"),temp,rh);
        center(ctx,48,sensors,264,row,&bambu_bold_18,s_skin->ink);
        duration(time,sizeof(time),u->dry_minutes);
        snprintf(row,sizeof(row),"%s · %s",dry_name(u->dry_state),time);
        if(lv_txt_get_width(row,strlen(row),&bambu_font_16,0,0)>188 && u->dry_minutes>=0){
            if(u->dry_minutes>=1440)snprintf(time,sizeof(time),"%dd %02d:%02d",u->dry_minutes/1440,u->dry_minutes/60%24,u->dry_minutes%60);
            else snprintf(time,sizeof(time),"%d:%02d",u->dry_minutes/60,u->dry_minutes%60);
            snprintf(row,sizeof(row),"%s · %s",dry_name(u->dry_state),time);
        }
        design_pill(ctx,78,ext?top+120:top+145,204,row,mixed(s_skin->accent,s_skin->bg,65),s_skin->ink);
    }else{
        center(ctx,55,211,250,SDG_T("未连接 AMS HT","No AMS HT connected"),&bambu_bold_22,s_skin->ink);
        center(ctx,55,249,250,SDG_T("温湿度 / 烘干 --","Temp / RH / Drying --"),&bambu_font_16,s_skin->ink);
    }
    const char *note=design_freshness_note();
    if(note)center(ctx,126,66,144,note,&bambu_font_16,s_skin->accent);
}
static void batch1_materials(lv_draw_ctx_t *ctx){
    int id=design_id(),index=bambu_pages_unit(&s_data,s_page);if(index<0)return;
    const bambu_ams_unit_t *u=&s_data.units[index];int slots=u->slot_count;
    char b[80];snprintf(b,sizeof(b),"%s %d",u->kind==BAMBU_AMS_STANDARD?"AMS":bambu_ams_kind_name(u->kind),bambu_pages_ordinal(&s_data,index));
    if(id==5){center(ctx,42,62,130,b,&bambu_bold_26,s_skin->ink);icon(ctx,ICON_PRINT,169,21,s_skin->accent);rect(ctx,123,22,20,20,0xFFDF7D,10);rect(ctx,130,17,18,20,s_skin->bg,10);rect(ctx,224,28,5,5,0xFFDF7D,1);}
    else if(id==2)matrix(ctx,75,28,210,b,4,s_skin->ink);
    else center(ctx,75,26,210,b,&bambu_bold_26,s_skin->ink);
    if(id==3)icon(ctx,ICON_PRINT,257,42,s_skin->ink);
    if(id==6){spool(ctx,244,42,25,26,0x22C5D0);spool(ctx,264,63,25,26,0xF97855);spool(ctx,235,68,25,26,0xEBCB3F);}
    bool active_unit=s_data.active_unit_id==u->id&&s_data.active_slot_id>=0&&s_data.active_slot_id<slots;
    if(active_unit)snprintf(b,sizeof(b),SDG_T("当前：%d 号槽","Active: slot %d"),s_data.active_slot_id+1);else snprintf(b,sizeof(b),"%s",SDG_T("当前：--","Active: --"));
    const char *note=design_freshness_note();if(note)snprintf(b,sizeof(b),"%s",note);
    design_pill(ctx,id==5?178:id==6?74:id==4?150:104,id==5||id==4?72:68,id==4?174:152,b,id==2?s_skin->bg:mixed(s_skin->accent,s_skin->bg,75),s_skin->ink);

    if(slots==1){
        const bambu_ams_tray_t *t=&u->slots[0];spool(ctx,139,115,82,76,tray_color(t));
        snprintf(b,sizeof(b),"%s  %s",t->reported&&t->type[0]?t->type:"--",active_unit?SDG_T("使用中","In use"):"");center(ctx,60,204,240,b,&bambu_bold_22,s_skin->ink);
        if(t->reported&&t->remain>=0)snprintf(b,sizeof(b),"%.0f%%",(double)t->remain);else strcpy(b,"--");center(ctx,80,232,200,b,&bambu_bold_26,s_skin->ink);
    }else if(id==1){
        for(int i=0;i<slots;i++){
            const bambu_ams_tray_t *t=&u->slots[i];int x=39+i*74;bool active=active_unit&&s_data.active_slot_id==i;
            if(active)frame(ctx,x-7,111,70,151,s_skin->accent,15,2);
            side_spool(ctx,x+3,121,tray_color(t));snprintf(b,sizeof(b),"%d",i+1);center(ctx,x+16,172,24,b,&bambu_bold_18,s_skin->ink);
            center(ctx,x-4,194,66,t->reported&&t->type[0]?t->type:"--",&bambu_bold_18,s_skin->ink);
            if(t->reported&&t->remain>=0)snprintf(b,sizeof(b),"%.0f%%",(double)t->remain);else strcpy(b,"--");center(ctx,x-5,216,68,b,&bambu_bold_22,s_skin->ink);
            if(active)design_pill(ctx,x-1,238,58,SDG_T("使用中","In use"),s_skin->accent,0xFFFFFF);
        }
    }else if(id==5){
        for(int i=0;i<slots;i++){
            const bambu_ams_tray_t *t=&u->slots[i];int x=35+i*74;bool active=active_unit&&s_data.active_slot_id==i;
            rect(ctx,x,115,68,148,0x3F3753,17);frame(ctx,x,115,68,148,active?s_skin->accent:0x9389A4,17,active?3:1);
            if(t->reported&&t->remain>=0){int fill=(int)(108*t->remain/100);if(fill>108)fill=108;if(fill>0)rect(ctx,x+3,260-fill,62,fill,tray_color(t),12);}
            snprintf(b,sizeof(b),"%d",i+1);center(ctx,x,120,68,b,&bambu_bold_18,s_skin->ink);
            uint32_t ink=t->reported&&t->remain>=65?0x152229:s_skin->ink;
            center(ctx,x+2,181,64,t->reported&&t->type[0]?t->type:"--",&bambu_bold_18,ink);
            if(t->reported&&t->remain>=0)snprintf(b,sizeof(b),"%.0f%%",(double)t->remain);else strcpy(b,"--");center(ctx,x+1,205,66,b,&bambu_bold_22,ink);
            if(active)design_pill(ctx,x+2,237,64,SDG_T("使用中","In use"),0xD95848,0xFFFFFF);
        }
    }else{
        if(id==4){rect(ctx,25,109,310,155,0xFFF9E8,20);frame(ctx,25,109,310,155,s_skin->ink,20,1);cat_peek(ctx,43,51,92);}
        for(int i=0;i<slots;i++){
            const bambu_ams_tray_t *t=&u->slots[i];int y=115+i*36;bool active=active_unit&&s_data.active_slot_id==i;
            if(id==6)rect(ctx,29,y,302,31,mixed(0xFFFFFF,s_skin->bg,155),14);
            if(active)frame(ctx,27,y-1,306,33,id==6?0xF56542:s_skin->accent,11,2);
            snprintf(b,sizeof(b),"%d",i+1);center(ctx,34,y+3,22,b,&bambu_bold_18,s_skin->ink);
            rect(ctx,64,y+7,19,19,tray_color(t),id==3?2:4);
            text(ctx,91,y+3,73,t->reported&&t->type[0]?t->type:"--",&bambu_bold_18,s_skin->ink,LV_TEXT_ALIGN_LEFT);
            if(t->reported&&t->remain>=0)snprintf(b,sizeof(b),"%.0f%%",(double)t->remain);else strcpy(b,"--");
            center(ctx,163,y+2,60,b,&bambu_bold_18,s_skin->ink);
            if(active)center(ctx,248,y+4,76,SDG_T("使用中","In use"),&bambu_font_16,id==6?0xF56542:s_skin->accent);
            else if(id==2||id==3){for(int j=0;j<8;j++){
                bool filled=t->reported&&t->remain>=0&&j*100<8*t->remain;
                if(id==2)rect(ctx,232+j*11,y+12,7,7,filled?s_skin->ink:s_skin->card,4);
                else if(filled)rect(ctx,232+j*11,y+12,8,9,s_skin->ink,0);else frame(ctx,232+j*11,y+12,8,9,0xAAC5B2,0,1);
            }}
        }
    }
    if(id==2)dashline(ctx,66,268,228,s_skin->ink);else stroke(ctx,63,269,297,269,s_skin->card,1);
    materials_footer(ctx,u);
}
static void batch1_details(lv_draw_ctx_t *ctx){
    int id=design_id();char a[24],b[24];
    if(id==4)cat_peek(ctx,72,44,48);
    else if(id==6){spool(ctx,77,48,24,24,0xEE7357);spool(ctx,91,58,24,24,0x7D59B5);}
    else if(id==2){stroke(ctx,90,51,98,59,s_skin->accent,2);stroke(ctx,98,59,90,67,s_skin->accent,2);}
    else {icon(ctx,ICON_PRINT,83,47,id==5?s_skin->accent:s_skin->ink);}
    design_detail_heading(ctx);
    design_detail_status(ctx,70,id==2?s_skin->bg:id==5?0xAB8BD2:s_skin->accent,id==2?s_skin->accent:id==5?0x20182B:0xFFFFFF,false);
    design_layers(ctx,id==2,id==5?s_skin->accent:s_skin->ink);
    const char *names[]={SDG_T("喷嘴","Nozzle"),SDG_T("热床","Bed"),SDG_T("腔温","Chamber"),SDG_T("速度","Speed")};
    char readings[4][64];
    value(a,sizeof(a),BAMBU_VALID_NOZZLE,s_data.nozzle_temp,"");value(b,sizeof(b),BAMBU_VALID_NOZZLE_TARGET,s_data.nozzle_target,"");snprintf(readings[0],sizeof(readings[0]),"%s/%s°C",a,b);
    value(a,sizeof(a),BAMBU_VALID_BED,s_data.bed_temp,"");value(b,sizeof(b),BAMBU_VALID_BED_TARGET,s_data.bed_target,"");snprintf(readings[1],sizeof(readings[1]),"%s/%s°C",a,b);
    value(readings[2],sizeof(readings[2]),BAMBU_VALID_CHAMBER,s_data.chamber_temp,"°C");value(readings[3],sizeof(readings[3]),BAMBU_VALID_SPEED,s_data.spd_mag,"%");
    if(id==2){
        for(int i=0;i<4;i++){int y=183+i*28;dashline(ctx,39,y-3,282,s_skin->ink);rect(ctx,46,y+9,6,6,s_skin->accent,0);text(ctx,63,y+2,70,names[i],&bambu_bold_18,s_skin->ink,LV_TEXT_ALIGN_LEFT);text(ctx,143,y+2,174,readings[i],&bambu_bold_18,s_skin->ink,LV_TEXT_ALIGN_LEFT);}
    }else{
        const uint32_t fill[]={0xFFC9C9,0xBCEAB6,0xBBD8FF,0xFFF0A1};
        for(int i=0;i<4;i++){
            int x=DESIGN_GRID_X+(i%2)*DESIGN_GRID_STEP,y=DESIGN_GRID_Y+(i/2)*DESIGN_GRID_ROW;
            if(id==1){if(i<2)stroke(ctx,x,y-4,x+DESIGN_GRID_W-1,y-4,s_skin->card,1);stroke(ctx,x,y+47,x+DESIGN_GRID_W-1,y+47,s_skin->card,1);}
            else if(id==3||id==4)frame(ctx,x,y,DESIGN_GRID_W,DESIGN_GRID_H,s_skin->ink,9,1);
            else rect(ctx,x,y,DESIGN_GRID_W,DESIGN_GRID_H,id==6?fill[i]:0x49415D,10);
            metric_icon(ctx,x+9,y+12,i,id==1||id==4?0xF56542:id==5?0xD7B8F4:s_skin->ink);
            text(ctx,x+39,y+1,89,names[i],&bambu_font_16,s_skin->ink,LV_TEXT_ALIGN_LEFT);
            text(ctx,x+39,y+20,89,readings[i],&bambu_bold_18,s_skin->ink,LV_TEXT_ALIGN_LEFT);
        }
        if(id==1)stroke(ctx,179,181,179,287,s_skin->card,1);
    }
    stroke(ctx,70,293,290,293,s_skin->card,1);
    design_fans(ctx,false,s_skin->accent,s_skin->accent);
}
