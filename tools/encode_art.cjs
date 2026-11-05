/* SPDX-License-Identifier: MIT
 * Convert the untouched generated atlas to small RGB565 LVGL image descriptors.
 * No retouching: each equal tile is sampled to the display resource dimensions.
 */
const fs = require('fs');
const path = require('path');
const pngModule = process.argv[2];
const {PNG} = require(pngModule);
const root = path.resolve(__dirname, '..');
const batch=Number(process.argv[3]||1);
if(![1,2,3].includes(batch))throw new Error('Batch must be 1, 2 or 3');
const columns=batch===2?2:3,rows=batch===1?1:2;
const names=batch===3?['farm','cyber','ocean','snow','desert','magic']:
    batch===2?['handheld','robot','bamboo','space']:['printer','cat','night'];
const png = PNG.sync.read(fs.readFileSync(path.join(root,`main/assets/batch-${String(batch).padStart(2,'0')}-art-source.png`)));
const widthOut=144,heightOut=96;
for(let tile=0; tile<names.length; tile++) {
  const bytes=[];
  const width=png.width/columns,height=png.height/rows;
  for(let y=0;y<heightOut;y++) for(let x=0;x<widthOut;x++) {
    const sx=Math.min(png.width-1,Math.floor((tile%columns+(x+.5)/widthOut)*width));
    const sy=Math.min(png.height-1,Math.floor((Math.floor(tile/columns)+(y+.5)/heightOut)*height));
    const i=(sy*png.width+sx)*4;
    const rgb=((png.data[i]>>3)<<11)|((png.data[i+1]>>2)<<5)|(png.data[i+2]>>3);
    // LV_COLOR_16_SWAP=y on official hardware: serialized high byte first.
    bytes.push(rgb>>8,rgb&255);
  }
  const name=names[tile];
  let output='/* SPDX-License-Identifier: MIT; AI-generated source art, RGB565 encoding. */\n#include "lvgl.h"\n';
  output+=`static const uint8_t pixels[] = {\n`;
  for(let i=0;i<bytes.length;i+=32) output+=bytes.slice(i,i+32).map(b=>'0x'+b.toString(16).padStart(2,'0')).join(',')+',\n';
  output+=`};\nconst lv_img_dsc_t bambu_art_${name} = {.header={.cf=LV_IMG_CF_TRUE_COLOR,.w=${widthOut},.h=${heightOut}},.data_size=sizeof(pixels),.data=pixels};\n`;
  fs.writeFileSync(path.join(root,`main/assets/art_${name}.c`),output);
}
