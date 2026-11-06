/* Encode isolated ImageGen decorations as RGB565+alpha; preserve source PNG. */
const fs=require('fs'),path=require('path');
const {PNG}=require(process.argv[2]);
const root=path.resolve(__dirname,'../..');
const src=PNG.sync.read(fs.readFileSync(path.join(root,'main/assets/batch-01-scenes-v2.png')));
const specs=[['printer',168,142],['cat',264,158],['night',288,172]];
for(let cell=0;cell<3;cell++) {
 const [name,w,h]=specs[cell],left=Math.floor(src.width*cell/3),right=Math.floor(src.width*(cell+1)/3);
 let x0=right,y0=src.height,x1=left,y1=0;
 for(let y=0;y<src.height;y++)for(let x=left;x<right;x++)if(src.data[(y*src.width+x)*4+3]>32){x0=Math.min(x0,x);x1=Math.max(x1,x);y0=Math.min(y0,y);y1=Math.max(y1,y);}
 if(x1<=x0||y1<=y0)throw Error('Missing alpha sprite '+name);
 const scale=Math.min((w-4)/(x1-x0+1),(h-4)/(y1-y0+1));
 const dw=(x1-x0+1)*scale,dh=(y1-y0+1)*scale,bytes=[];
 for(let y=0;y<h;y++)for(let x=0;x<w;x++){
  const sx=Math.floor(x0+(x-(w-dw)/2)/scale),sy=Math.floor(y0+(y-(h-dh)/2)/scale);
  if(sx<x0||sx>x1||sy<y0||sy>y1){bytes.push(0,0,0);continue;}
  const i=(sy*src.width+sx)*4,c=((src.data[i]>>3)<<11)|((src.data[i+1]>>2)<<5)|(src.data[i+2]>>3);
  bytes.push(c>>8,c&255,src.data[i+3]);
 }
 let text='/* AI-generated isolated decoration; RGB565+alpha; source archived in docs. SPDX-License-Identifier: MIT */\n#include "lvgl.h"\nstatic const uint8_t pixels[]={\n';
 for(let i=0;i<bytes.length;i+=36)text+=bytes.slice(i,i+36).map(v=>'0x'+v.toString(16).padStart(2,'0')).join(',')+',\n';
 text+=`};\nconst lv_img_dsc_t bambu_scene_${name}={.header={.cf=LV_IMG_CF_TRUE_COLOR_ALPHA,.w=${w},.h=${h}},.data_size=sizeof(pixels),.data=pixels};\n`;
 fs.writeFileSync(path.join(root,`main/assets/scene_${name}.c`),text);
 console.log(name,w,h,bytes.length);
}
