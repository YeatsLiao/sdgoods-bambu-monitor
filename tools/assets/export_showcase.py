#!/usr/bin/env python3
"""Export selected README sheets from cached native LVGL frames."""
from pathlib import Path
import argparse,hashlib,json
from PIL import Image,ImageDraw
from archive_store import ROOT,archive_files
OUT=ROOT/'docs/screenshots/software'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--source',type=Path,default=ROOT/'build/preview/frames');args=parser.parse_args();source=args.source
    if not source.is_dir():raise SystemExit('Run tools/preview/render_preview.py first; cached frames belong in build/preview/frames.')
    OUT.mkdir(parents=True,exist_ok=True);records=[]
    def tile(name):
        p=source/name
        if not p.exists():p=p.with_suffix('.ppm')
        with Image.open(p) as raw:
            assert raw.size==(360,360);im=raw.convert('RGB')
        mask=Image.new('L',(360,360));ImageDraw.Draw(mask).ellipse((0,0,359,359),fill=255)
        canvas=Image.new('RGB',(360,360),(230,230,230));canvas.paste(im,(0,0),mask);return canvas
    def sheet(name,files,cols,rows):
        image=Image.new('RGB',(cols*360,rows*360),(230,230,230))
        for i,f in enumerate(files):image.paste(tile(f),((i%cols)*360,(i//cols)*360))
        dest=OUT/name
        if dest.exists():
            with Image.open(dest) as current:same=current.convert('RGB').tobytes()==image.tobytes()
        else:same=False
        if not same:
            if dest.exists():archive_files([dest],'showcase-revision','selected-software-preview')
            image.save(dest)
        records.append({'file':name,'sha256':sha(dest),'size':list(image.size),'sources':files})
    for group,name in enumerate(('first-six-skins.png','second-six-skins.png','third-six-skins.png')):
        sheet(name,[f'skin-{i:02d}-page-1-zh.png' for i in range(group*6+1,group*6+7)],3,2)
    sheet('settings-six-pages.png',['settings-1.png','settings-2.png','settings-3.png','settings-4.png','settings-provisioning.png','skin-picker-01-zh.png'],3,2)
    for group in range(3):
        sheet(f'skins-{group*6+1:02d}-{group*6+6:02d}-all-pages.png',[f'skin-{group*6+col+1:02d}-page-{page}-zh.png' for page in range(1,5) for col in range(6)],6,4)
    (OUT/'manifest.json').write_text(json.dumps({'version':(ROOT/'version.txt').read_text().strip(),'sourceType':'native-LVGL-software-rendering-with-sample-data','sourceDirectory':'build/preview/frames','homepageSkinIds':list(range(1,19)),'files':records},ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print('PASS: 18 styles and settings exported to docs/screenshots/software.')
if __name__=='__main__':main()
