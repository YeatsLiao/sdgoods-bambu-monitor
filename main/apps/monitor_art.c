/* SPDX-License-Identifier: MIT
 * One decoded image cache, owned exclusively by the LVGL thread. No network
 * task owns artwork buffers. Encoded assets stay in flash.
 */
#include "monitor_art.h"
static const monitor_art_t *s_cached;
static lv_img_dsc_t s_image;
static uint8_t *s_pixels;
static size_t s_capacity;

bool monitor_art_expand(const monitor_art_t *art,uint8_t *pixels,size_t capacity){
    if(!art||!pixels||!art->palette||!art->runs||!art->width||!art->height)return false;
    size_t count=(size_t)art->width*art->height;
    if(count>SIZE_MAX/2||capacity<count*2)return false;
    size_t in=0,out=0;
    while(in<art->size && out<count){
        uint8_t command=art->runs[in++];size_t n=(command&127)+1;
        if(n>count-out)return false;
        if(command&128){
            if(in>=art->size)return false;
            unsigned index=art->runs[in++]*2;
            while(n--){pixels[out*2]=art->palette[index];pixels[out*2+1]=art->palette[index+1];out++;}
        }else{
            if(n>art->size-in)return false;
            while(n--){unsigned index=art->runs[in++]*2;pixels[out*2]=art->palette[index];pixels[out*2+1]=art->palette[index+1];out++;}
        }
    }
    return out==count && in==art->size;
}
bool monitor_art_draw(lv_draw_ctx_t *ctx,const monitor_art_t *art,int x,int y,int width){
    if(!art || width<=0)return false;
    if(s_cached!=art){
        size_t required=(size_t)art->width*art->height*2;
        if(required>64000)return false;
        lv_img_cache_invalidate_src(&s_image);s_cached=NULL;
        if(s_capacity<required){
            lv_mem_free(s_pixels);s_pixels=lv_mem_alloc(required);s_capacity=s_pixels?required:0;
        }
        if(!s_pixels||!monitor_art_expand(art,s_pixels,s_capacity))return false;
        s_image=(lv_img_dsc_t){.header={.cf=LV_IMG_CF_TRUE_COLOR,.w=art->width,.h=art->height},.data_size=required,.data=s_pixels};
        s_cached=art;
    }
    lv_draw_img_dsc_t d;lv_draw_img_dsc_init(&d);d.zoom=width*256/art->width;d.pivot=(lv_point_t){0,0};d.antialias=0;
    lv_area_t a={x,y,x+art->width-1,y+art->height-1};lv_draw_img(ctx,&d,&a,&s_image);return true;
}
