#!/usr/bin/env python3
"""Export Git-visible README sheets from existing native LVGL previews."""
from pathlib import Path
import hashlib,json,shutil
from PIL import Image

ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'docs/verification/preview'
OUT=ROOT/'docs/assets/showcase'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def preserve(path):
    if path.exists():
        old=ROOT/'docs/history/releases/showcase-revisions'/f'{path.stem}-{sha(path)[:12]}{path.suffix}'
        old.parent.mkdir(parents=True,exist_ok=True)
        if not old.exists():shutil.copy2(path,old)
def main():
    OUT.mkdir(parents=True,exist_ok=True);records=[]
    def sheet(name,sources):
        image=Image.new('RGB',(1080,720),(230,230,230))
        for i,file in enumerate(sources):
            with Image.open(SOURCE/file) as tile:
                assert tile.size==(360,360)
                image.paste(tile.convert('RGB'),((i%3)*360,(i//3)*360))
        dest=OUT/name
        if dest.exists():
            with Image.open(dest) as current:same=current.convert('RGB').tobytes()==image.tobytes()
        else:same=False
        if not same:preserve(dest);image.save(dest)
        with Image.open(dest) as saved:
            assert saved.convert('RGB').tobytes()==image.tobytes()
        records.append({'file':name,'sha256':sha(dest),'size':[1080,720],'sources':sources})
    names=['first-six-skins.png','second-six-skins.png','third-six-skins.png']
    for group,name in enumerate(names):
        sheet(name,[f'skin-{i:02d}-page-1-zh.png' for i in range(group*6+1,group*6+7)])
    sheet('settings-six-pages.png',['settings-1.png','settings-2.png','settings-3.png',
        'settings-4.png','settings-provisioning.png','skin-picker-01-zh.png'])
    for batch in (1,2,3):
        source=SOURCE/f'batch-{batch:02d}-zh.png';dest=OUT/f'skins-{(batch-1)*6+1:02d}-{batch*6:02d}-all-pages.png'
        if dest.exists() and sha(dest)!=sha(source):preserve(dest)
        if not dest.exists() or sha(dest)!=sha(source):shutil.copy2(source,dest)
        assert sha(dest)==sha(source)
        with Image.open(dest) as im:size=list(im.size)
        records.append({'file':dest.name,'sha256':sha(dest),'size':size,'sources':[source.name]})
    (OUT/'manifest.json').write_text(json.dumps({'version':(ROOT/'version.txt').read_text().strip(),
        'sourceType':'native-LVGL-software-rendering-with-sample-data','sourceDirectory':'docs/verification/preview',
        'homepageSkinIds':list(range(1,19)),'files':records},ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print('PASS: all 18 home pages, 6 settings pages, 3 full page sheets; source pixels preserved.')
if __name__=='__main__':main()
