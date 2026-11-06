/* SPDX-License-Identifier: MIT
 * Shared geometry for all skins. Artwork and palettes remain theme-specific.
 * Keep the slash in its own box; never rely on Black-font side bearings.
 */
#define DESIGN_GRID_X 44
#define DESIGN_GRID_STEP 140
#define DESIGN_GRID_W 132
#define DESIGN_GRID_Y 188
#define DESIGN_GRID_ROW 51
#define DESIGN_GRID_H 48
typedef struct {
    const lv_font_t *font;
    int x,y,left,slash,right,gap,cell;
} layer_layout_t;
static layer_layout_t layer_layout(const char *a,const char *b,bool pixel){
    layer_layout_t p={0};
    if(pixel){
        for(int c=6;c>=2;c--){
            int left=(int)strlen(a)*6*c-c,right=(int)strlen(b)*6*c-c,slash=5*c,gap=2*c;
            if(left+right+slash+2*gap>292)continue;
            p=(layer_layout_t){.x=(360-left-right-slash-2*gap)/2,.y=100+(64-7*c)/2,.left=left,.slash=slash,.right=right,.gap=gap,.cell=c};break;
        }
    }else{
        const lv_font_t *fonts[]={&bambu_num_64,&bambu_num_44,&bambu_bold_26,&bambu_bold_22,&bambu_bold_18};
        for(unsigned i=0;i<sizeof(fonts)/sizeof(*fonts);i++){
            const lv_font_t *f=fonts[i];int left=lv_txt_get_width(a,strlen(a),f,0,0),right=lv_txt_get_width(b,strlen(b),f,0,0),gap=f->line_height>48?12:9,slash=f->line_height>48?14:10;
            if(left+right+slash+2*gap>292)continue;
            p=(layer_layout_t){.font=f,.x=(360-left-right-slash-2*gap)/2,.y=100+(64-f->line_height)/2,.left=left,.slash=slash,.right=right,.gap=gap};break;
        }
    }
    return p;
}
static void design_layers(lv_draw_ctx_t *ctx,bool pixel,uint32_t color){
    char a[24],b[24];
    if(s_data.valid_mask&BAMBU_VALID_LAYER)snprintf(a,sizeof(a),"%d",s_data.layer_num);else strcpy(a,"--");
    if(s_data.valid_mask&BAMBU_VALID_TOTAL_LAYER)snprintf(b,sizeof(b),"%d",s_data.total_layer);else strcpy(b,"--");
    layer_layout_t p=layer_layout(a,b,pixel);int sx=p.x+p.left+p.gap,rx=sx+p.slash+p.gap;
    if(pixel){matrix(ctx,p.x,p.y,p.left,a,p.cell,color);matrix(ctx,sx,p.y,p.slash,"/",p.cell,color);matrix(ctx,rx,p.y,p.right,b,p.cell,color);}
    else if(p.font){
        text(ctx,p.x,p.y,p.left+1,a,p.font,color,LV_TEXT_ALIGN_LEFT);
        int inset=p.font->line_height>48?12:7;
        stroke(ctx,sx+p.slash-3,p.y+inset,sx+3,p.y+p.font->line_height-inset,color,p.font->line_height>48?3:2);
        text(ctx,rx,p.y,p.right+1,b,p.font,color,LV_TEXT_ALIGN_LEFT);
    }
    center(ctx,75,160,210,SDG_T("当前层 / 总层","Current / Total layers"),&bambu_bold_18,s_skin->ink);
}
static void design_detail_heading(lv_draw_ctx_t *ctx){
    center(ctx,126,32,122,SDG_T("打印详情","Print details"),sdg_i18n_is_zh()?&bambu_bold_26:&bambu_bold_22,s_skin->ink);
    design_gear(ctx,DETAIL_SETTINGS_CX,DETAIL_SETTINGS_CY,s_skin->accent);
}
static const char *design_freshness_note(void){
    if(!s_service.connected)return SDG_T("离线","Offline");
    if(!bambu_state_fresh(&s_data,(uint32_t)(esp_timer_get_time()/1000)))return SDG_T("数据过期","Stale data");
    return NULL;
}
static const char *design_compact_state(void){
    if(!s_service.connected)return SDG_T("离线","Offline");
    if(!bambu_state_fresh(&s_data,(uint32_t)(esp_timer_get_time()/1000)))return SDG_T("等待","Stale");
    if(s_data.state==BAMBU_STATE_UNKNOWN)return SDG_T("等待","Waiting");
    if(s_data.state==BAMBU_STATE_FAILED)return SDG_T("异常","Error");
    if(s_data.state==BAMBU_STATE_PREPARE&&!sdg_i18n_is_zh())return "Prep";
    return state_name();
}
static void design_detail_status(lv_draw_ctx_t *ctx,int y,uint32_t bg,uint32_t ink,bool outline){
    char b[96];snprintf(b,sizeof(b),"%s · %s",printer_model(),state_name());
    if(lv_txt_get_width(b,strlen(b),&bambu_font_16,0,0)>140)snprintf(b,sizeof(b),"%s · %s",printer_model(),design_compact_state());
    design_pill(ctx,124,y,148,b,bg,ink);if(outline)frame(ctx,124,y,148,26,ink,13,1);
}
static void design_fan_icon(lv_draw_ctx_t *ctx,int x,int y,uint32_t color){
    lv_draw_arc_dsc_t d;lv_draw_arc_dsc_init(&d);d.color=lv_color_hex(color);d.width=3;d.rounded=1;
    lv_point_t p={x+7,y+7};for(int i=0;i<3;i++)lv_draw_arc(ctx,&d,&p,6,15+i*120,86+i*120);
    rect(ctx,x+6,y+6,3,3,color,2);
}
static void design_fans(lv_draw_ctx_t *ctx,bool icons,uint32_t first,uint32_t second){
    char a[24],b[24],left[48],right[48],row[112];
    value(a,sizeof(a),BAMBU_VALID_COOLING,s_data.cooling_fan,"%");value(b,sizeof(b),BAMBU_VALID_AUX_FAN,s_data.big_fan1,"%");
    snprintf(left,sizeof(left),SDG_T("模型 %s","Model %s"),a);snprintf(right,sizeof(right),SDG_T("辅助 %s","Aux %s"),b);
    if(!icons){snprintf(row,sizeof(row),"%s  |  %s",left,right);center(ctx,70,296,220,row,&bambu_font_16,s_skin->ink);return;}
    int lw=lv_txt_get_width(left,strlen(left),&bambu_font_16,0,0),rw=lv_txt_get_width(right,strlen(right),&bambu_font_16,0,0),x=(360-lw-rw-50)/2;
    design_fan_icon(ctx,x,300,first);text(ctx,x+18,296,lw+1,left,&bambu_font_16,s_skin->ink,LV_TEXT_ALIGN_LEFT);
    x+=lw+32;design_fan_icon(ctx,x,300,second);text(ctx,x+18,296,rw+1,right,&bambu_font_16,s_skin->ink,LV_TEXT_ALIGN_LEFT);
}
