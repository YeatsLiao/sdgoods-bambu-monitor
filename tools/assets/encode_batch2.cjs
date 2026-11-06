/* SPDX-License-Identifier: MIT; preserve PNG sources, crop/scale/encode only. */
const fs=require('fs'),path=require('path'),{PNG}=require(process.argv[2]);
const root=path.resolve(__dirname,'../..');
const atlas=PNG.sync.read(fs.readFileSync(path.join(root,'main/assets/batch-02-sprites-v2.png')));
const specs=[['art_handheld','bambu_art_handheld',176,94],['art_robot','bambu_art_robot',140,124],
 ['art_bamboo','bambu_art_bamboo',190,106],['art_space','bambu_art_space',190,106],
 ['robot_head','bambu_robot_head',54,46],['astronaut_head','bambu_astronaut_head',48,48]];
function save(name,symbol,w,h,bytes,alpha){
 let c='/* ImageGen source archived in docs/assets; RGB565'+(alpha?'+alpha':'')+'. SPDX-License-Identifier: MIT */\n#include "lvgl.h"\nstatic const uint8_t pixels[]={\n';
 for(let i=0;i<bytes.length;i+=36)c+=bytes.slice(i,i+36).map(v=>'0x'+v.toString(16).padStart(2,'0')).join(',')+',\n';
 c+=`};\nconst lv_img_dsc_t ${symbol}={.header={.cf=${alpha?'LV_IMG_CF_TRUE_COLOR_ALPHA':'LV_IMG_CF_TRUE_COLOR'},.w=${w},.h=${h}},.data_size=sizeof(pixels),.data=pixels};\n`;
 fs.writeFileSync(path.join(root,`main/assets/${name}.c`),c);console.log(name,bytes.length);
}
for(let cell=0;cell<6;cell++){
 const [name,symbol,w,h]=specs[cell],left=Math.floor(cell%2*atlas.width/2),right=Math.floor((cell%2+1)*atlas.width/2),top=Math.floor(Math.floor(cell/2)*atlas.height/3),bottom=Math.floor((Math.floor(cell/2)+1)*atlas.height/3);
 let x0=right,y0=bottom,x1=left,y1=top;
 for(let y=top;y<bottom;y++)for(let x=left;x<right;x++)if(atlas.data[(y*atlas.width+x)*4+3]>=160){x0=Math.min(x0,x);x1=Math.max(x1,x);y0=Math.min(y0,y);y1=Math.max(y1,y);}
 if(x1<=x0||y1<=y0)throw Error('Missing alpha sprite '+name);
 const scale=Math.min((w-4)/(x1-x0+1),(h-4)/(y1-y0+1)),dw=(x1-x0+1)*scale,dh=(y1-y0+1)*scale,bytes=[];
 for(let y=0;y<h;y++)for(let x=0;x<w;x++){
  const sx=Math.floor(x0+(x-(w-dw)/2)/scale),sy=Math.floor(y0+(y-(h-dh)/2)/scale);
  if(sx<x0||sx>x1||sy<y0||sy>y1){bytes.push(0,0,0);continue;}
  const i=(sy*atlas.width+sx)*4,c=((atlas.data[i]>>3)<<11)|((atlas.data[i+1]>>2)<<5)|(atlas.data[i+2]>>3);
  bytes.push(c>>8,c&255,atlas.data[i+3]);
 }
 save(name,symbol,w,h,bytes,true);
}
const paper=PNG.sync.read(fs.readFileSync(path.join(root,'main/assets/paper-texture-v1.png'))),bytes=[];
for(let y=0;y<96;y++)for(let x=0;x<96;x++){
 const i=(Math.floor(y*paper.height/96)*paper.width+Math.floor(x*paper.width/96))*4,c=((paper.data[i]>>3)<<11)|((paper.data[i+1]>>2)<<5)|(paper.data[i+2]>>3);bytes.push(c>>8,c&255);
}
save('paper_tile','bambu_paper_tile',96,96,bytes,false);
