/* SPDX-License-Identifier: MIT */
#include "monitor_skin.h"
#include "sdkconfig.h"
#include "monitor_art.h"
static const monitor_skin_t s_skins[]={
    {"大字进度","Editorial",0xFFF9ED,0x181916,0xFF603E,0xE9DDCA},
#if CONFIG_BAMBU_SKIN_COUNT > 1
    {"点阵终端","Dot Matrix",0x111210,0xFFF6D6,0xFF5545,0x353630},
    {"像素工坊","Pixel Workshop",0xE6F9DF,0x104A28,0xFF6749,0xCBDFC4},
    {"猫咪监工","Cat Foreman",0xFFF2BC,0x20241C,0xF56542,0xE4D5AA},
    {"夜班打印室","Night Workshop",0x25213F,0xFFF2DC,0xF6BF82,0x504664},
    {"彩色料仓","Color Spools",0xEEE0FA,0x330C43,0x8842AB,0xD5BCE3},
#endif
#if CONFIG_BAMBU_SKIN_COUNT > 6
    {"纸感标签","Paper Label",0xFFF4DC,0x184184,0x2753CB,0xE6D8BB},
    {"掌机像素","Pocket Console",0xDDF5F2,0x123F46,0xF2785D,0xB6DCDA},
    {"机器人助手","Robot Helper",0xD9CCFA,0x39204E,0x47BB96,0xC1AFDE},
    {"竹林工坊","Bamboo Workshop",0xE2EBCB,0x20462B,0x739A38,0xC5D3A6},
    {"太空货舱","Space Workshop",0x151D42,0xFFF0D8,0x55D0EB,0x313B60},
    {"拼色几何","Color Geometry",0xFFF5E4,0x262536,0x3156DC,0xE4DCCA},
#endif
#if CONFIG_BAMBU_SKIN_COUNT > 12
    {"农场造物","Farm Workshop",0xFFF3D8,0x244C20,0x72AA31,0xDDDDAE},
    {"赛博街区","Cyber Street",0x29152F,0xFFF1EC,0xEF51AD,0x4A3050},
    {"海底实验室","Undersea Lab",0xC7F4FA,0x123B59,0x078FC3,0x9ED7E6},
    {"雪山木屋","Snow Cabin",0xDEEEFF,0x283966,0x9770D8,0xB9D2EF},
    {"沙漠基地","Desert Base",0xFFE5BD,0x60321D,0xE56C26,0xEDC99D},
    {"魔法工坊","Magic Workshop",0x3E245A,0xFFE6C3,0xCF99EB,0x634675},
#endif
};
const monitor_skin_t *monitor_skin_get(int id) {
    if(id<1||id>(int)(sizeof(s_skins)/sizeof(*s_skins))) id=1;
    return &s_skins[id-1];
}
bool monitor_skin_has_art(int id) {
    return (id>=3&&id<=5)||(id>=8&&id<=11)||(id>=13&&id<=18);
}
static void shape(lv_draw_ctx_t *ctx,int x,int y,int w,int h,uint32_t color,int radius) {
    lv_draw_rect_dsc_t d;lv_draw_rect_dsc_init(&d);d.bg_color=lv_color_hex(color);d.radius=radius;
    lv_area_t a={x,y,x+w-1,y+h-1};lv_draw_rect(ctx,&d,&a);
}
void monitor_skin_decor(lv_draw_ctx_t *ctx,int id) {
    if(id==7) {
        shape(ctx,72,92,216,2,0x2753CB,0);
        for(int i=0;i<5;i++)shape(ctx,42,115+i*20,9,12,i%2?0x2753CB:0xE95D43,0);
        shape(ctx,72,271,216,1,0xC5AF8C,0);
    } else if(id==12) {
        shape(ctx,18,20,86,86,0x3156DC,43);
        shape(ctx,267,109,26,26,0xFF7547,13);
        const uint32_t colors[]={0xF6C832,0x287BEC,0x41B68B,0x8C4CB9};
        for(int i=0;i<4;i++)shape(ctx,119+i*32,273,22,9,colors[i],3);
    }
}
/* Artwork is a separate replaceable asset; rendering code never contains telemetry. */
void monitor_skin_art(lv_draw_ctx_t *ctx,int id) {
    monitor_skin_art_at(ctx,id,108,83,144);
}
void monitor_skin_art_at(lv_draw_ctx_t *ctx,int id,int x,int y,int width) {
#if CONFIG_BAMBU_SKIN_COUNT > 1
    if(id>=3&&id<=5){monitor_skin_scene(ctx,id,x,y,width);return;}
    const lv_img_dsc_t *img=NULL;
#if CONFIG_BAMBU_SKIN_COUNT > 6
    LV_IMG_DECLARE(bambu_art_handheld);
    LV_IMG_DECLARE(bambu_art_robot);
    LV_IMG_DECLARE(bambu_art_bamboo);
    LV_IMG_DECLARE(bambu_art_space);
    if(id==8)img=&bambu_art_handheld;
    if(id==9)img=&bambu_art_robot;
    if(id==10)img=&bambu_art_bamboo;
    if(id==11)img=&bambu_art_space;
#endif
#if CONFIG_BAMBU_SKIN_COUNT > 12
    if(id>=13&&id<=18){
        extern const monitor_art_t bambu_art_farm,bambu_art_cyber,bambu_art_ocean,bambu_art_snow,bambu_art_desert,bambu_art_magic;
        const monitor_art_t *scenes[]={&bambu_art_farm,&bambu_art_cyber,&bambu_art_ocean,&bambu_art_snow,&bambu_art_desert,&bambu_art_magic};
        monitor_art_draw(ctx,scenes[id-13],x,y,width);return;
    }
#endif
    if(!img) return;
    lv_draw_img_dsc_t d;lv_draw_img_dsc_init(&d);
    d.zoom=width*256/img->header.w;d.pivot=(lv_point_t){0,0};
    lv_area_t area={x,y,x+img->header.w-1,y+img->header.h-1};
    lv_draw_img(ctx,&d,&area,img);
#else
    (void)ctx; (void)id;(void)x;(void)y;(void)width;
#endif
}
void monitor_skin_scene(lv_draw_ctx_t *ctx,int id,int x,int y,int width) {
#if CONFIG_BAMBU_SKIN_COUNT > 1
    LV_IMG_DECLARE(bambu_scene_printer);LV_IMG_DECLARE(bambu_scene_cat);LV_IMG_DECLARE(bambu_scene_night);
    const lv_img_dsc_t *src=id==3?&bambu_scene_printer:id==4?&bambu_scene_cat:id==5?&bambu_scene_night:NULL;
    if(!src)return;
    lv_draw_img_dsc_t d;lv_draw_img_dsc_init(&d);
    d.zoom=width*256/src->header.w;d.pivot=(lv_point_t){0,0};
    d.antialias=id==5;
    lv_area_t a={x,y,x+src->header.w-1,y+src->header.h-1};
    lv_draw_img(ctx,&d,&a,src);
#else
    (void)ctx;(void)id;(void)x;(void)y;(void)width;
#endif
}

void monitor_skin_header(lv_draw_ctx_t *ctx,int id,int x,int y,int width){
#if CONFIG_BAMBU_SKIN_COUNT > 12
    extern const monitor_art_t bambu_art_farm_header,bambu_art_cyber_header,bambu_art_ocean_header,bambu_art_snow_header,bambu_art_desert_header,bambu_art_magic_header;
    const monitor_art_t *headers[]={&bambu_art_farm_header,&bambu_art_cyber_header,&bambu_art_ocean_header,&bambu_art_snow_header,&bambu_art_desert_header,&bambu_art_magic_header};
    if(id>=13&&id<=18)monitor_art_draw(ctx,headers[id-13],x,y,width);
#else
    (void)ctx;(void)id;(void)x;(void)y;(void)width;
#endif
}
