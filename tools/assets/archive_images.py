#!/usr/bin/env python3
"""Preserve image/font sources and index local plus privately archived media."""
from pathlib import Path
import argparse,json,shutil
from archive_store import ROOT,digest,entries,archive_files
IMAGE_TYPES={'.png','.jpg','.jpeg','.webp','.gif','.ppm'}
TARGET=ROOT/'docs/assets';IMAGES=TARGET/'images'
def copy(src,dst,original=False):
    dst.parent.mkdir(parents=True,exist_ok=True)
    if src.resolve()==dst.resolve():return dst
    if dst.exists() and digest(src)!=digest(dst):
        if original:dst=dst.with_name(dst.stem+'-'+digest(src)[:12]+dst.suffix)
        else:archive_files([dst],'replaced-source','replaced-source')
    if not dst.exists() or digest(src)!=digest(dst):shutil.copy2(src,dst)
    if digest(src)!=digest(dst):raise RuntimeError('Source copy mismatch: '+str(dst))
    return dst

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--source',type=Path,default=IMAGES)
    parser.add_argument('--snapshot',help='Archive build/preview/frames with this tag')
    args=parser.parse_args();source=args.source.resolve()
    if not source.is_dir():raise SystemExit('Source directory not found: '+str(source))
    manifest=TARGET/'image-manifest.json';old=json.loads(manifest.read_text(encoding='utf-8')) if manifest.exists() else {}
    originals={i['path']:i for i in old.get('generatedOriginals',[])}
    for src in sorted(source.iterdir()):
        if not src.is_file() or src.suffix.lower() not in IMAGE_TYPES:continue
        dst=copy(src,IMAGES/src.name,original=True);key=dst.relative_to(ROOT).as_posix()
        if key in originals or src.name.startswith('exec-') or source!=IMAGES.resolve():
            originals[key]={'title':originals.get(key,{}).get('title','生成原图'),'originalName':src.name,'path':key,'bytes':dst.stat().st_size,'sha256':digest(dst)}
    for src in (ROOT/'main/assets').glob('*.png'):copy(src,IMAGES/src.name)
    for src in (ROOT/'tools/fonts').iterdir():
        if src.is_file() and src.suffix.lower() in {'.ttf','.otf'}:copy(src,TARGET/'fonts'/src.name)
    copy(ROOT/'main/fonts/OFL.txt',TARGET/'fonts/OFL.txt')
    if args.snapshot:
        frames=ROOT/'build/preview/frames'
        if not frames.is_dir():raise SystemExit('Run tools/preview/render_preview.py first')
        archive_files(list(frames.iterdir()),args.snapshot,'software-rendering')
    images=[];resources=[]
    for p in sorted((ROOT/'docs').rglob('*')):
        if not p.is_file():continue
        item={'path':p.relative_to(ROOT).as_posix(),'bytes':p.stat().st_size,'sha256':digest(p)}
        if p.suffix.lower() in IMAGE_TYPES:images.append(item)
        elif p.is_relative_to(TARGET/'fonts'):resources.append(item)
    for folder in ('main/assets','main/fonts'):
        for p in sorted((ROOT/folder).iterdir()):
            if p.is_file() and p.suffix in {'.c','.txt'}:resources.append({'path':p.relative_to(ROOT).as_posix(),'bytes':p.stat().st_size,'sha256':digest(p)})
    archived=[{'archive':e['archive'],'path':e['path'],'bytes':e['bytes'],'sha256':e['sha256']} for e in entries() if Path(e['path']).suffix.lower() in IMAGE_TYPES|{'.c','.ttf','.otf','.bin'}]
    result={'schema':2,'generatedSource':str(IMAGES),'originalGenerationCache':old.get('originalGenerationCache'),
        'generatedOriginals':list(originals.values()),'generatedCount':len(originals),'images':images,'resources':resources,
        'archivedMedia':archived,'archiveIndex':'docs/notes/archive-index.json','docsImageCount':len(images),'allGeneratedBytesVerified':True}
    manifest.write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print(f'PASS: {len(originals)} originals; {len(images)} local pictures; {len(archived)} archived media records; canonical runtime resources indexed.')
if __name__=='__main__':main()
