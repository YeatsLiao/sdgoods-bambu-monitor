/* SPDX-License-Identifier: MIT; package native LVGL framebuffer outputs as PNG. */
const fs=require('fs'),path=require('path');
const {PNG}=require(process.argv[2]);
const root=path.resolve(__dirname,'../../build/preview/frames');
for(const name of fs.readdirSync(root).filter(n=>n.endsWith('.ppm'))) {
  const b=fs.readFileSync(path.join(root,name));
  const offset=b.indexOf('255\n')+4;
  const png=new PNG({width:360,height:360});
  for(let y=0;y<360;y++)for(let x=0;x<360;x++) {
    let a=(y*360+x)*4,s=offset+(y*360+x)*3;
    const inside=(x-179.5)**2+(y-179.5)**2<180**2;
    for(let c=0;c<3;c++) png.data[a+c]=inside?b[s+c]:230;
    png.data[a+3]=255;
  }
  fs.writeFileSync(path.join(root,name.replace('.ppm','.png')),PNG.sync.write(png));
}
for(const lang of ['zh','en']) for(const batch of [1,2,3]) {
 const out=new PNG({width:2160,height:1440});out.data.fill(230);
 for(let column=0;column<6;column++)for(let page=1;page<=4;page++) {
  const skin=(batch-1)*6+column+1;
  const src=PNG.sync.read(fs.readFileSync(path.join(root,`skin-${String(skin).padStart(2,'0')}-page-${page}-${lang}.png`)));
  PNG.bitblt(src,out,0,0,360,360,column*360,(page-1)*360);
 }
 fs.writeFileSync(path.join(root,`batch-${String(batch).padStart(2,'0')}-${lang}.png`),PNG.sync.write(out));
}
for(let page=1;page<=4;page++){
 const out=new PNG({width:1080,height:720});out.data.fill(230);
 for(let i=0;i<6;i++){
  const src=PNG.sync.read(fs.readFileSync(path.join(root,`skin-${String(i+1).padStart(2,'0')}-page-${page}-zh.png`)));
  PNG.bitblt(src,out,0,0,360,360,(i%3)*360,Math.floor(i/3)*360);
 }
 fs.writeFileSync(path.join(root,`batch-01-${['overview','ams','external-ht','details'][page-1]}-zh.png`),PNG.sync.write(out));
}
const settings=new PNG({width:720,height:720});settings.data.fill(230);
for(const [i,name] of ['settings-1','settings-3','settings-2','skin-picker-01-zh'].entries()){
 const src=PNG.sync.read(fs.readFileSync(path.join(root,name+'.png')));
 PNG.bitblt(src,settings,0,0,360,360,(i%2)*360,Math.floor(i/2)*360);
}
fs.writeFileSync(path.join(root,'settings-overview.png'),PNG.sync.write(settings));

const picker=new PNG({width:1080,height:720});picker.data.fill(230);
for(let i=0;i<6;i++){
 const src=PNG.sync.read(fs.readFileSync(path.join(root,`skin-picker-${String(i+1).padStart(2,'0')}-zh.png`)));
 PNG.bitblt(src,picker,0,0,360,360,(i%3)*360,Math.floor(i/3)*360);
}
fs.writeFileSync(path.join(root,'skin-picker-overview.png'),PNG.sync.write(picker));
