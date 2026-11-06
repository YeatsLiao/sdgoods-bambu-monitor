/* SPDX-License-Identifier: MIT. Palette quantization and lossless PackBits.
 * Original PNGs remain archived; no UI text is baked into encoded artwork. */
const fs=require('fs'),path=require('path');
const root=path.resolve(__dirname,'../..');
function pack(indices){
 const out=[];let i=0;
 while(i<indices.length){
  let run=1;while(run<128&&i+run<indices.length&&indices[i+run]===indices[i])run++;
  if(run>=3){out.push(128|run-1,indices[i]);i+=run;continue;}
  const start=i;i+=run;
  while(i<indices.length&&i-start<128){let r=1;while(r<3&&i+r<indices.length&&indices[i+r]===indices[i])r++;if(r>=3)break;i++;}
  const n=i-start;out.push(n-1,...indices.slice(start,i));
 }
 return out;
}
function quantize(pixels){
 const hist=new Map();for(const c of pixels)hist.set(c,(hist.get(c)||0)+1);
 const colors=[...hist].map(([v,n])=>({v,n,r:(v>>11)*255/31,g:((v>>5)&63)*255/63,b:(v&31)*255/31}));
 function box(list){const min=[255,255,255],max=[0,0,0];let n=0;for(const c of list){[c.r,c.g,c.b].forEach((v,k)=>{min[k]=Math.min(min[k],v);max[k]=Math.max(max[k],v);});n+=c.n;}const ranges=max.map((v,k)=>v-min[k]);return {list,n,ranges,score:Math.max(...ranges)*Math.sqrt(n)};}
 const boxes=[box(colors)];
 while(boxes.length<256){
  let ix=-1;for(let i=0;i<boxes.length;i++)if(boxes[i].list.length>1&&(ix<0||boxes[i].score>boxes[ix].score))ix=i;
  if(ix<0)break;const a=boxes.splice(ix,1)[0],key=['r','g','b'][a.ranges.indexOf(Math.max(...a.ranges))];a.list.sort((x,y)=>x[key]-y[key]);
  let n=0,cut=1;for(;cut<a.list.length;cut++){n+=a.list[cut-1].n;if(n>=a.n/2)break;}
  cut=Math.min(cut,a.list.length-1);boxes.push(box(a.list.slice(0,cut)),box(a.list.slice(cut)));
 }
 const palette=[],lookup=new Map();
 for(let i=0;i<boxes.length;i++){
  const a=boxes[i];let r=0,g=0,b=0;for(const c of a.list){r+=c.r*c.n;g+=c.g*c.n;b+=c.b*c.n;lookup.set(c.v,i);}
  const v=(Math.round(r/a.n*31/255)<<11)|(Math.round(g/a.n*63/255)<<5)|Math.round(b/a.n*31/255);palette.push(v>>8,v&255);
 }
 while(palette.length<512)palette.push(0,0);
 return {palette,indices:pixels.map(v=>lookup.get(v))};
}
function panel(src,cell,w,h,mode){
 const pw=src.width/2,ph=src.height/3,left=Math.floor(cell%2*pw),top=Math.floor(Math.floor(cell/2)*ph);
 /* Preserve proportions. Extend edge pixels to pad quiet sky/side walls:
  * scenes fit width at the bottom; character banners fit height centered. */
 const scale=mode==='scene'?w/pw:h/ph,ox=(w-pw*scale)/2,oy=mode==='scene'?h-ph*scale:0,pixels=[];
 let r=0,g=0,b=0,n=0;
 for(let yy=1;yy<4;yy++)for(let xx=Math.floor(pw*.2);xx<pw*.8;xx++){const i=((top+yy)*src.width+left+xx)*4;r+=src.data[i];g+=src.data[i+1];b+=src.data[i+2];n++;}
 const sky=((Math.round(r/n)>>3)<<11)|((Math.round(g/n)>>2)<<5)|(Math.round(b/n)>>3);
 for(let y=0;y<h;y++)for(let x=0;x<w;x++){
  if(mode==='scene'&&y<oy){pixels.push(sky);continue;}
  const sx=Math.max(left,Math.min(Math.floor(left+pw)-1,Math.floor(left+(x-ox+.5)/scale))),sy=Math.max(top,Math.min(Math.floor(top+ph)-1,Math.floor(top+(y-oy+.5)/scale))),i=(sy*src.width+sx)*4;
  pixels.push(((src.data[i]>>3)<<11)|((src.data[i+1]>>2)<<5)|(src.data[i+2]>>3));
 }
 return pixels;
}
function emit(name,variants){
 let code='/* ImageGen originals in docs/assets; palette256+PackBits. SPDX-License-Identifier: MIT */\n#include "monitor_art.h"\n';
 const arrays=(n,values)=>{code+=`static const uint8_t ${n}[]={\n`;for(let i=0;i<values.length;i+=32)code+=values.slice(i,i+32).map(v=>'0x'+v.toString(16).padStart(2,'0')).join(',')+',\n';code+='};\n';};
 let total=0;
 for(const [suffix,pixels,w,h] of variants){const {palette,indices}=quantize(pixels),runs=pack(indices);arrays('palette'+suffix,palette);arrays('runs'+suffix,runs);code+=`const monitor_art_t bambu_art_${name}${suffix}={${w},${h},palette${suffix},runs${suffix},sizeof(runs${suffix})};\n`;total+=palette.length+runs.length;}
 fs.writeFileSync(path.join(root,`main/assets/art_${name}.c`),code);return total;
}
function main(){
 const {PNG}=require(process.argv[2]),scenes=PNG.sync.read(fs.readFileSync(path.join(root,'main/assets/batch-03-scenes-v2.png'))),headers=PNG.sync.read(fs.readFileSync(path.join(root,'main/assets/batch-03-headers-v2.png')));
 const names=['farm','cyber','ocean','snow','desert','magic'];let total=0;
 for(let i=0;i<6;i++){const bytes=emit(names[i],[['',panel(scenes,i,240,128,'scene'),240,128],['_header',panel(headers,i,240,64,'header'),240,64]]);total+=bytes;console.log(names[i],bytes);}
 console.log('Encoded third-batch total bytes',total);
}
module.exports={pack,quantize};if(require.main===module)main();
