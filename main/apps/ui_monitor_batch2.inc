#if CONFIG_BAMBU_SKIN_COUNT > 6
/* SPDX-License-Identifier: MIT
 * Batch 02 layouts follow the ORIGINAL three reference sheets in docs/design.
 * Live text is separate from art. Accessories extend the same visual language.
 */
static void b2_image(lv_draw_ctx_t *ctx,const lv_img_dsc_t *src,int x,int y,int width){
    lv_draw_img_dsc_t d;lv_draw_img_dsc_init(&d);d.zoom=width*256/src->header.w;
    d.pivot=(lv_point_t){0,0};d.antialias=0;
    if(src->header.cf==LV_IMG_CF_TRUE_COLOR)d.opa=100;
    lv_area_t a={x,y,x+src->header.w-1,y+src->header.h-1};lv_draw_img(ctx,&d,&a,src);
}
static void b2_background(lv_draw_ctx_t *ctx){
    int id=design_id();
    if(id==7){
        LV_IMG_DECLARE(bambu_paper_tile);
        for(int y=0;y<360;y+=96)for(int x=0;x<360;x+=96)b2_image(ctx,&bambu_paper_tile,x,y,96);
        /* Airmail stripes live at the circle edge; the text stays inboard. */
        for(int i=0;i<8;i++)stroke(ctx,-10,88+i*24,21,68+i*24,i%2?0x2854CC:0xEB6956,10);
    }else if(id==12){
        rect(ctx,-70,-40,158,158,0x2D50E6,79);rect(ctx,279,35,78,78,0xFF7046,39);
    }
}
static void b2_boat(lv_draw_ctx_t *ctx,int x,int y,int scale){
    static const int8_t path[][2]={{0,24},{12,24},{12,9},{26,9},{26,18},{39,13},{34,31},{7,31},{0,24}};
    for(unsigned i=1;i<sizeof(path)/sizeof(*path);i++)stroke(ctx,x+path[i-1][0]*scale,y+path[i-1][1]*scale,x+path[i][0]*scale,y+path[i][1]*scale,s_skin->ink,2);
    frame(ctx,x+16*scale,y+13*scale,6*scale,8*scale,s_skin->ink,1,2);
    stroke(ctx,x+20*scale,y+9*scale,x+20*scale,y+2*scale,s_skin->ink,2);
    stroke(ctx,x+27*scale,y+1*scale,x+30*scale,y-4*scale,0xF56A46,2);
}
static void b2_stamp(lv_draw_ctx_t *ctx,int x,int y){
    frame(ctx,x,y,27,44,0xE45D45,3,2);
    center(ctx,x+2,y+1,23,"拓",&bambu_bold_18,0xE45D45);center(ctx,x+2,y+20,23,"竹",&bambu_bold_18,0xE45D45);
}
/* Fit the visible decoration into a reserved box; images keep their aspect. */
static void b2_badge_box(lv_draw_ctx_t *ctx,int x,int y,int w,int h){
    int id=design_id();const lv_img_dsc_t *img=NULL;
    if(id==7){b2_boat(ctx,x+(w-39)/2,y+(h-31)/2,1);return;}
    LV_IMG_DECLARE(bambu_art_handheld);LV_IMG_DECLARE(bambu_robot_head);LV_IMG_DECLARE(bambu_bamboo_leaf);LV_IMG_DECLARE(bambu_astronaut_head);
    if(id==8)img=&bambu_art_handheld;else if(id==9)img=&bambu_robot_head;else if(id==10)img=&bambu_bamboo_leaf;else if(id==11)img=&bambu_astronaut_head;
    if(!img)return;
    int width=LV_MIN(w,h*(int)img->header.w/img->header.h),height=width*(int)img->header.h/img->header.w;
    b2_image(ctx,img,x+(w-width)/2,y+(h-height)/2,width);
}
static void b2_status(lv_draw_ctx_t *ctx,int x,int y,int w){
    uint32_t c=(design_id()==9||design_id()==12)?0x2CBA72:0xFF7149;
    rect(ctx,x,y,w,27,c,5);center(ctx,x+3,y+2,w-6,s_data.state==BAMBU_STATE_UNKNOWN?SDG_T("等待状态","Waiting"):state_name(),&bambu_bold_18,0xFFFFFF);
}
static void b2_progress_bar(lv_draw_ctx_t *ctx,int x,int y,int w){
    int p=s_data.valid_mask&BAMBU_VALID_PROGRESS?s_data.mc_percent:-1;
    bool segments=design_id()>=7&&design_id()<=11;
    if(segments){
        for(int i=0;i<12;i++){
            int start=i*w/12,end=(i+1)*w/12-3;
            rect(ctx,x+start,y,end-start,10,s_skin->card,2);
            int fill=w*(p<0?0:p)/100-start;if(fill>end-start)fill=end-start;
            if(fill>0)rect(ctx,x+start,y,fill,10,s_skin->accent,2);
        }
    }else{rect(ctx,x,y,w,13,s_skin->card,7);if(p>0)rect(ctx,x,y,w*p/100,13,s_skin->accent,7);}
}
static void b2_temps(lv_draw_ctx_t *ctx,int y,bool stacked){
    char a[20],b[20];value(a,sizeof(a),BAMBU_VALID_NOZZLE,s_data.nozzle_temp,"°");value(b,sizeof(b),BAMBU_VALID_BED,s_data.bed_temp,"°");
    if(stacked){
        stroke(ctx,67,y-4,293,y-4,s_skin->ink,1);stroke(ctx,180,y-3,180,y+48,s_skin->ink,1);
        center(ctx,72,y,101,SDG_T("喷嘴","Nozzle"),&bambu_bold_18,s_skin->ink);center(ctx,187,y,101,SDG_T("热床","Bed"),&bambu_bold_18,s_skin->ink);
        center(ctx,72,y+21,101,a,&bambu_bold_26,design_id()==7?0x35251D:s_skin->ink);center(ctx,187,y+21,101,b,&bambu_bold_26,design_id()==7?0x35251D:s_skin->ink);
    }else{
        char row[80];snprintf(row,sizeof(row),SDG_T("喷嘴 %s  |  热床 %s","Noz %s  |  Bed %s"),a,b);
        center(ctx,56,y,248,row,&bambu_bold_18,s_skin->ink);
    }
}
static void b2_big_progress(lv_draw_ctx_t *ctx,int y,uint32_t color){
    char b[16];if(s_data.valid_mask&BAMBU_VALID_PROGRESS)snprintf(b,sizeof(b),"%d",s_data.mc_percent);else strcpy(b,"--");
    const lv_font_t *f=&bambu_num_128,*unit=&bambu_num_64;
    int baseline=y+f->line_height-f->base_line,width=0,uw=0,x=40,py=y;
    for(int attempt=0;attempt<3;attempt++){
        unit=f==&bambu_num_128?&bambu_num_64:f==&bambu_num_88?&bambu_num_44:&bambu_bold_26;
        width=lv_txt_get_width(b,strlen(b),f,0,0);uw=lv_txt_get_width("%",1,unit,0,0);py=baseline-f->line_height+f->base_line;
        int band=design_band_width(py,f->line_height);x=LV_MAX(40,(360-band)/2);
        if(width+uw+3<=LV_MIN(255,(360+band)/2-x))break;
        f=f==&bambu_num_128?&bambu_num_88:&bambu_num_64;
    }
    text(ctx,x,py,width+2,b,f,color,LV_TEXT_ALIGN_LEFT);
    text(ctx,x+3+width,baseline-unit->line_height+unit->base_line,uw+2,"%",unit,color,LV_TEXT_ALIGN_LEFT);
}
static void batch2_overview(lv_draw_ctx_t *ctx){
    int id=design_id();
    if(id==7){
        center(ctx,79,27,100,printer_model(),&bambu_bold_26,s_skin->ink);b2_status(ctx,190,32,82);
        stroke(ctx,53,62,307,62,s_skin->ink,1);
        b2_big_progress(ctx,75,0x35251D);b2_boat(ctx,272,125,1);
        b2_progress_bar(ctx,51,209,258);design_remaining(ctx,232);b2_temps(ctx,269,true);b2_stamp(ctx,285,255);
    }else if(id==12){
        center(ctx,120,20,120,printer_model(),&bambu_bold_26,s_skin->ink);b2_status(ctx,132,55,96);
        b2_big_progress(ctx,89,s_skin->ink);b2_progress_bar(ctx,44,217,272);
        design_remaining(ctx,240);b2_temps(ctx,275,true);
        const uint32_t c[]={0xF9C83B,0x2E88E6,0x33B783,0x8F4BB4};for(int i=0;i<4;i++)rect(ctx,279+i*12,187,9,17,c[i],3);
    }else{
        center(ctx,89,28,100,printer_model(),&bambu_bold_22,s_skin->ink);b2_status(ctx,190,32,82);
        int width=id==8?244:id==9?166:282;
        monitor_skin_art_at(ctx,id,(360-width)/2,id==8?53:40,width);
        if(id==8){char b[12];if(s_data.valid_mask&BAMBU_VALID_PROGRESS)snprintf(b,sizeof(b),"%d%%",s_data.mc_percent);else strcpy(b,"--%");matrix(ctx,35,186,290,b,strlen(b)>3?8:9,s_skin->ink);}
        else design_progress(ctx,181,&bambu_num_88,s_skin->ink);
        b2_progress_bar(ctx,63,257,234);design_remaining(ctx,275);b2_temps(ctx,306,false);
    }
}
static void b2_spool(lv_draw_ctx_t *ctx,int x,int y,uint32_t c){
    frame(ctx,x,y,27,24,s_skin->ink,4,2);rect(ctx,x+6,y+4,15,16,c,2);
    rect(ctx,x+1,y,25,5,c,3);rect(ctx,x+1,y+19,25,5,c,3);
    stroke(ctx,x+4,y+7,x+4,y+17,0xFFFFFF,1);stroke(ctx,x+9,y+8,x+18,y+8,mixed(c,0xFFFFFF,120),1);
}
static void batch2_materials(lv_draw_ctx_t *ctx){
    int index=bambu_pages_unit(&s_data,s_page);if(index<0)return;
    const bambu_ams_unit_t *u=&s_data.units[index];char b[80];
    snprintf(b,sizeof(b),"%s %d",bambu_ams_kind_name(u->kind),bambu_pages_ordinal(&s_data,index));
    center(ctx,77,27,164,b,&bambu_bold_26,s_skin->ink);b2_badge_box(ctx,246,47,54,48);
    if(design_id()==7)b2_stamp(ctx,222,22);
    bool active=s_data.active_unit_id==u->id&&s_data.active_slot_id>=0&&s_data.active_slot_id<u->slot_count;
    if(active)snprintf(b,sizeof(b),SDG_T("当前：%d 号槽","Active: slot %d"),s_data.active_slot_id+1);else snprintf(b,sizeof(b),SDG_T("当前：--","Active: --"));
    const char *note=design_freshness_note();if(note)snprintf(b,sizeof(b),"%s",note);
    center(ctx,76,64,172,b,&bambu_bold_22,s_skin->ink);stroke(ctx,39,104,321,104,s_skin->ink,1);
    for(int i=0;i<u->slot_count;i++){
        const bambu_ams_tray_t *t=&u->slots[i];int y=115+i*37;bool selected=active&&s_data.active_slot_id==i;
        if(selected){rect(ctx,29,y-2,302,34,mixed(s_skin->bg,0xFFFFFF,180),10);frame(ctx,29,y-2,302,34,0xF56A46,10,2);}
        b2_spool(ctx,39,y+3,tray_color(t));
        snprintf(b,sizeof(b),"%d",i+1);rect(ctx,74,y+3,21,24,tray_color(t),4);center(ctx,74,y+3,21,b,&bambu_bold_18,0xFFFFFF);
        text(ctx,101,y+3,74,t->reported&&t->type[0]?t->type:"--",&bambu_bold_18,s_skin->ink,LV_TEXT_ALIGN_LEFT);
        if(selected){rect(ctx,260,y+6,60,20,0xF56A46,4);center(ctx,262,y+6,56,SDG_T("使用中","In use"),&bambu_font_16,0xFFFFFF);}
        else stroke(ctx,181,y+6,181,y+25,s_skin->ink,1);
        if(t->reported&&t->remain>=0)snprintf(b,sizeof(b),"%.0f%%",(double)t->remain);else strcpy(b,"--");
        center(ctx,188,y+2,64,b,&bambu_bold_22,s_skin->ink);
        uint32_t color=t->reported?tray_color(t):s_skin->card;
        if(!selected){rect(ctx,265,y+11,52,9,s_skin->card,5);
        if(t->reported&&t->remain>=0){int fill=52*t->remain/100;if(fill>52)fill=52;rect(ctx,265,y+11,fill,9,color,5);}
        else for(int k=0;k<52;k+=8)stroke(ctx,265+k,y+11,269+k,y+11,s_skin->ink,1);}
    }
    stroke(ctx,52,270,308,270,s_skin->ink,1);materials_footer(ctx,u);

}
static void batch2_details(lv_draw_ctx_t *ctx){
    b2_badge_box(ctx,70,40,52,44);design_detail_heading(ctx);
    char a[24],b[24];
    design_detail_status(ctx,70,design_id()==9||design_id()==12?0x2CBA72:0xFF7149,0xFFFFFF,false);
    design_layers(ctx,design_id()==8,s_skin->ink);
    const char *labels[]={SDG_T("喷嘴","Nozzle"),SDG_T("热床","Bed"),SDG_T("腔温","Chamber"),SDG_T("速度","Speed")};char readings[4][64];
    value(a,sizeof(a),BAMBU_VALID_NOZZLE,s_data.nozzle_temp,"");value(b,sizeof(b),BAMBU_VALID_NOZZLE_TARGET,s_data.nozzle_target,"");snprintf(readings[0],sizeof(readings[0]),"%s/%s°C",a,b);
    value(a,sizeof(a),BAMBU_VALID_BED,s_data.bed_temp,"");value(b,sizeof(b),BAMBU_VALID_BED_TARGET,s_data.bed_target,"");snprintf(readings[1],sizeof(readings[1]),"%s/%s°C",a,b);
    value(readings[2],sizeof(readings[2]),BAMBU_VALID_CHAMBER,s_data.chamber_temp,"°C");value(readings[3],sizeof(readings[3]),BAMBU_VALID_SPEED,s_data.spd_mag,"%");
    for(int i=0;i<4;i++){
        int x=DESIGN_GRID_X+i%2*DESIGN_GRID_STEP,y=DESIGN_GRID_Y+i/2*DESIGN_GRID_ROW;
        rect(ctx,x,y,DESIGN_GRID_W,DESIGN_GRID_H,design_id()==11?s_skin->bg:design_id()==7?0xFFF0D7:0xFFF9EA,design_id()==8?2:9);
        frame(ctx,x,y,DESIGN_GRID_W,DESIGN_GRID_H,s_skin->ink,design_id()==8?2:9,1);
        center(ctx,x+4,y+1,DESIGN_GRID_W-8,labels[i],&bambu_bold_18,s_skin->ink);center(ctx,x+4,y+20,DESIGN_GRID_W-8,readings[i],&bambu_bold_22,s_skin->ink);
    }
    stroke(ctx,70,293,290,293,s_skin->ink,1);design_fans(ctx,false,s_skin->accent,s_skin->accent);
}
static void batch2_accessories(lv_draw_ctx_t *ctx){
    batch1_accessories(ctx);
    b2_badge_box(ctx,70,40,52,44);
}

#endif
