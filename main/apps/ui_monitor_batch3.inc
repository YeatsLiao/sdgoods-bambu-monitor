#if CONFIG_BAMBU_SKIN_COUNT > 12
/* SPDX-License-Identifier: MIT
 * Original batch-03 scene and character-header compositions, not generic cards.
 * Third logical page is explicitly a theme extension (no approved original).
 */
static uint32_t b3_accent(int n){
    int id=design_id();
    if(id==14)return n%2?0x47E5EC:0xEF51AD;
    if(id==17)return n==2?0x07858E:0xBE542B;
    if(id==18)return n<2?0xF3D88E:s_skin->accent;
    return s_skin->accent;
}
static void b3_header(lv_draw_ctx_t *ctx){
    monitor_skin_header(ctx,design_id(),0,0,360);
    stroke(ctx,30,96,330,96,s_skin->card,1);
}
static void b3_bar(lv_draw_ctx_t *ctx,int x,int y,int w,int h,int percent,uint32_t c){
    rect(ctx,x,y,w,h,s_skin->card,h/2);
    if(percent<=0)return;
    if(percent>100)percent=100;
    lv_draw_rect_dsc_t d;lv_draw_rect_dsc_init(&d);d.radius=h/2;d.bg_color=lv_color_hex(c);
    if(design_id()==14){d.bg_grad.dir=LV_GRAD_DIR_HOR;d.bg_grad.stops_count=2;
        d.bg_grad.stops[0].color=lv_color_hex(c);d.bg_grad.stops[0].frac=0;
        d.bg_grad.stops[1].color=lv_color_hex(0x4DE8EB);d.bg_grad.stops[1].frac=255;}
    lv_area_t a={x,y,x+w*percent/100-1,y+h-1};lv_draw_rect(ctx,&d,&a);
}
static void batch3_overview(lv_draw_ctx_t *ctx){
    monitor_skin_art_at(ctx,design_id(),0,0,360);
    design_pill(ctx,126,12,108,printer_model(),s_skin->bg,s_skin->ink);
    design_progress(ctx,195,&bambu_num_64,s_skin->ink);
    int progress=s_data.valid_mask&BAMBU_VALID_PROGRESS?s_data.mc_percent:-1;
    b3_bar(ctx,65,256,230,13,progress,s_skin->accent);
    design_remaining(ctx,277);
    char a[24],b[24];value(a,sizeof(a),BAMBU_VALID_NOZZLE,s_data.nozzle_temp,"°");value(b,sizeof(b),BAMBU_VALID_BED,s_data.bed_temp,"°");
    center(ctx,92,307,36,SDG_T("喷嘴","Noz"),&bambu_font_16,s_skin->ink);
    center(ctx,129,304,54,a,&bambu_bold_22,design_id()==14?0x46E8F0:s_skin->ink);
    center(ctx,189,307,34,SDG_T("热床","Bed"),&bambu_font_16,s_skin->ink);
    center(ctx,225,304,47,b,&bambu_bold_22,design_id()==14?0xFF56B6:s_skin->ink);
    if(!s_service.connected||s_data.state!=BAMBU_STATE_RUNNING){
        design_pill(ctx,103,158,154,state_name(),s_skin->bg,s_skin->ink);
    }
}
static void b3_spool(lv_draw_ctx_t *ctx,int x,int y,const bambu_ams_tray_t *t){
    bool known=t->reported&&t->remain>=0;uint32_t c=tray_color(t);
    if(!known){
        for(int i=0;i<26;i+=7){stroke(ctx,x+i,y+2,x+i+3,y+2,s_skin->ink,1);stroke(ctx,x+i,y+23,x+i+3,y+23,s_skin->ink,1);}
        stroke(ctx,x+2,y+6,x+2,y+19,s_skin->ink,1);stroke(ctx,x+24,y+6,x+24,y+19,s_skin->ink,1);return;
    }
    rect(ctx,x+4,y+3,20,20,c,4);frame(ctx,x+3,y+2,22,22,s_skin->ink,6,1);
    rect(ctx,x,y,28,8,c,5);frame(ctx,x,y,28,8,s_skin->ink,5,1);
    rect(ctx,x,y+19,28,7,c,5);frame(ctx,x,y+19,28,7,s_skin->ink,5,1);
    stroke(ctx,x+7,y+10,x+7,y+17,mixed(c,0xFFFFFF,100),1);
}
static void batch3_materials(lv_draw_ctx_t *ctx){
    int index=bambu_pages_unit(&s_data,s_page);if(index<0)return;
    const bambu_ams_unit_t *u=&s_data.units[index];char b[80];b3_header(ctx);
    snprintf(b,sizeof(b),"%s %d",bambu_ams_kind_name(u->kind),bambu_pages_ordinal(&s_data,index));
    center(ctx,115,25,130,b,&bambu_bold_26,s_skin->ink);
    bool active=s_data.active_unit_id==u->id&&s_data.active_slot_id>=0&&s_data.active_slot_id<u->slot_count;
    if(active)snprintf(b,sizeof(b),SDG_T("当前：%d 号槽","Active: slot %d"),s_data.active_slot_id+1);else snprintf(b,sizeof(b),SDG_T("当前：--","Active: --"));
    const char *note=design_freshness_note();if(note)snprintf(b,sizeof(b),"%s",note);
    center(ctx,80,62,200,b,&bambu_bold_18,s_skin->ink);
    for(int i=0;i<u->slot_count;i++){
        const bambu_ams_tray_t *t=&u->slots[i];int y=114+i*38;bool selected=active&&s_data.active_slot_id==i;
        if(selected)frame(ctx,26,y-2,308,34,s_skin->accent,16,2);
        b3_spool(ctx,38,y+3,t);snprintf(b,sizeof(b),"%d",i+1);
        if(design_id()==14)frame(ctx,73,y+3,22,24,s_skin->ink,4,1);else rect(ctx,73,y+3,22,24,s_skin->ink,4);
        center(ctx,73,y+3,22,b,&bambu_bold_18,design_id()==14?s_skin->ink:s_skin->bg);
        text(ctx,102,y+4,76,t->reported&&t->type[0]?t->type:"--",&bambu_bold_18,s_skin->ink,LV_TEXT_ALIGN_LEFT);
        if(selected)design_pill(ctx,184,y+3,61,SDG_T("使用中","In use"),s_skin->accent,s_skin->bg);
        int bx=179,bw=82;
        if(!selected){
        if(t->reported&&t->remain>=0)b3_bar(ctx,bx,y+11,bw,10,(int)t->remain,tray_color(t));
        else {rect(ctx,bx,y+11,bw,10,s_skin->card,5);unknown_bar(ctx,bx,y+11,bw,10,mixed(s_skin->ink,s_skin->bg,145));} }
        if(t->reported&&t->remain>=0)snprintf(b,sizeof(b),"%.0f%%",(double)t->remain);else strcpy(b,"--");
        center(ctx,267,y+2,56,b,&bambu_bold_22,s_skin->ink);
    }
    stroke(ctx,52,275,308,275,s_skin->accent,2);materials_footer(ctx,u);

}
static void b3_metric_icon(lv_draw_ctx_t *ctx,int x,int y,int n){
    metric_icon(ctx,x,y,n,b3_accent(n));
}
static void batch3_details(lv_draw_ctx_t *ctx){
    b3_header(ctx);design_detail_heading(ctx);
    char a[24],b[24];design_detail_status(ctx,66,s_skin->bg,s_skin->ink,true);
    design_layers(ctx,false,s_skin->ink);
    const char *labels[]={SDG_T("喷嘴","Nozzle"),SDG_T("热床","Bed"),SDG_T("腔温","Chamber"),SDG_T("速度","Speed")};char readings[4][64];
    value(a,sizeof(a),BAMBU_VALID_NOZZLE,s_data.nozzle_temp,"");value(b,sizeof(b),BAMBU_VALID_NOZZLE_TARGET,s_data.nozzle_target,"");snprintf(readings[0],sizeof(readings[0]),"%s/%s°C",a,b);
    value(a,sizeof(a),BAMBU_VALID_BED,s_data.bed_temp,"");value(b,sizeof(b),BAMBU_VALID_BED_TARGET,s_data.bed_target,"");snprintf(readings[1],sizeof(readings[1]),"%s/%s°C",a,b);
    value(readings[2],sizeof(readings[2]),BAMBU_VALID_CHAMBER,s_data.chamber_temp,"°C");value(readings[3],sizeof(readings[3]),BAMBU_VALID_SPEED,s_data.spd_mag,"%");
    for(int i=0;i<4;i++){
        int x=DESIGN_GRID_X+(i%2)*DESIGN_GRID_STEP,y=DESIGN_GRID_Y+(i/2)*DESIGN_GRID_ROW;
        rect(ctx,x,y,DESIGN_GRID_W,DESIGN_GRID_H,s_skin->card,9);
        if(design_id()==14||design_id()==18)frame(ctx,x,y,DESIGN_GRID_W,DESIGN_GRID_H,mixed(s_skin->ink,s_skin->bg,85),9,1);
        b3_metric_icon(ctx,x+9,y+12,i);
        text(ctx,x+39,y+1,89,labels[i],&bambu_font_16,s_skin->ink,LV_TEXT_ALIGN_LEFT);
        text(ctx,x+39,y+20,89,readings[i],&bambu_bold_18,s_skin->ink,LV_TEXT_ALIGN_LEFT);
    }
    stroke(ctx,70,293,290,293,s_skin->accent,1);design_fans(ctx,true,b3_accent(0),b3_accent(1));
}
static void batch3_accessories(lv_draw_ctx_t *ctx){
    b3_header(ctx);batch1_accessories(ctx);
}
#endif
