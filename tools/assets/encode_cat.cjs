/* Encode the generated transparent cat sprite; source bytes stay archived. */
const fs=require('fs'),path=require('path');
const {PNG}=require(process.argv[2]);
const root=path.resolve(__dirname,'../..');
const src=PNG.sync.read(fs.readFileSync(path.join(root,'main/assets/cat-peek-v1.png')));
let x0=src.width,y0=src.height,x1=0,y1=0,transparent=0;
for(let y=0;y<src.height;y++)for(let x=0;x<src.width;x++){
 const a=src.data[(y*src.width+x)*4+3];if(!a)transparent++;
 if(a>32){x0=Math.min(x0,x);x1=Math.max(x1,x);y0=Math.min(y0,y);y1=Math.max(y1,y);}
}
if(!transparent)throw Error('Generated cat must have a real alpha channel');
const w=96,h=64,scale=Math.min((w-2)/(x1-x0+1),(h-2)/(y1-y0+1));
const dw=(x1-x0+1)*scale,dh=(y1-y0+1)*scale,bytes=[];
for(let y=0;y<h;y++)for(let x=0;x<w;x++){
 const sx=Math.floor(x0+(x-(w-dw)/2)/scale),sy=Math.floor(y0+(y-(h-dh)/2)/scale);
 if(sx<x0||sx>x1||sy<y0||sy>y1){bytes.push(0,0,0);continue;}
 const i=(sy*src.width+sx)*4,c=((src.data[i]>>3)<<11)|((src.data[i+1]>>2)<<5)|(src.data[i+2]>>3);
 bytes.push(c>>8,c&255,src.data[i+3]);
}
let code='/* ImageGen transparent character; RGB565+alpha. Source in docs/assets. SPDX-License-Identifier: MIT */\n#include "lvgl.h"\nstatic const uint8_t pixels[]={\n';
for(let i=0;i<bytes.length;i+=36)code+=bytes.slice(i,i+36).map(v=>'0x'+v.toString(16).padStart(2,'0')).join(',')+',\n';
code+=`};\nconst lv_img_dsc_t bambu_cat_peek={.header={.cf=LV_IMG_CF_TRUE_COLOR_ALPHA,.w=${w},.h=${h}},.data_size=sizeof(pixels),.data=pixels};\n`;
fs.writeFileSync(path.join(root,'main/assets/cat_peek.c'),code);
console.log('Cat sprite: '+bytes.length+' bytes, alpha verified.');
