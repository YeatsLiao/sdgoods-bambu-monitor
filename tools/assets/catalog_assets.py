#!/usr/bin/env python3
"""Generate reproducible asset lineage and verify complete media coverage."""
from pathlib import Path
import argparse, hashlib, json, re
from collections import defaultdict
from PIL import Image

ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'docs/assets/catalog.json'
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def rel(p): return p.relative_to(ROOT).as_posix()
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--check',action='store_true');args=parser.parse_args()
    m=json.loads((ROOT/'docs/assets/image-manifest.json').read_text(encoding='utf-8'))
    records=m['images']+m['resources']; hashes=defaultdict(list)
    for item in records:
        p=ROOT/item['path']
        if not p.is_file() or sha(p)!=item['sha256']: raise SystemExit('Changed indexed media: '+item['path'])
        hashes[item['sha256']].append(item['path'])
    media={rel(p) for p in (ROOT/'docs').rglob('*') if p.is_file() and p.suffix.lower() in {'.png','.jpg','.jpeg','.ppm','.webp','.gif','.bin'}}
    if media-{i['path'] for i in records}: raise SystemExit('Unindexed media: '+str(sorted(media-{i['path'] for i in records})))
    for item in m.get('archivedMedia',[]):hashes[item['sha256']].append('zip:'+item['archive']+'#'+item['path'])
    markdown=list((ROOT/'docs').rglob('*.md'))
    bodies={rel(p):p.read_text(encoding='utf-8') for p in markdown}
    originals=[]
    for item in m['generatedOriginals']:
        p=ROOT/item['path']
        with Image.open(p) as im: dimensions=list(im.size)
        originals.append(item|{'dimensions':dimensions,'relatedCopies':hashes[item['sha256']],
            'mentionRecords':[name for name,body in bodies.items() if name!='docs/assets/README.md' and p.name in body],
            'promptPolicy':'Only explicit historical prompt records are authoritative; mentions do not prove an exact prompt.'})
    rules={}
    for name in ('printer','cat','night'):
        rules['scene_'+name+'.c']=(['batch-01-scenes-v2.png'],'encode_scenes.cjs')
        rules['art_'+name+'.c']=(['batch-01-art-source.png'],'encode_art.cjs')
    for name in ('handheld','robot','bamboo','space'):
        rules['art_'+name+'.c']=(['batch-02-sprites-v2.png'],'encode_batch2.cjs')
    for name in ('robot_head','astronaut_head'):
        rules[name+'.c']=(['batch-02-sprites-v2.png'],'encode_batch2.cjs')
    for name in ('farm','cyber','ocean','snow','desert','magic'):
        rules['art_'+name+'.c']=(['batch-03-scenes-v2.png','batch-03-headers-v2.png'],'encode_batch3.cjs')
    rules.update({'cat_peek.c':(['cat-peek-v1.png'],'encode_cat.cjs'),
        'bamboo_leaf.c':(['bamboo-leaf-v1.png'],'encode_bamboo.cjs'),
        'paper_tile.c':(['paper-texture-v1.png'],'encode_batch2.cjs')})
    ui='\n'.join(p.read_text(encoding='utf-8') for p in (ROOT/'main/apps').glob('*') if p.suffix in {'.c','.inc'})
    cmake=(ROOT/'main/CMakeLists.txt').read_text(encoding='utf-8'); runtime=[]
    for p in sorted((ROOT/'main/assets').glob('*.c')):
        sources,script=rules[p.name]; symbols=re.findall(r'const (?:lv_img_dsc_t|monitor_art_t)\s+(\w+)\s*=',p.read_text(encoding='utf-8'))
        source_items=[]
        for name in sources:
            atlas=ROOT/'main/assets'/name
            source_items.append({'source':'main/assets/'+name,'archiveCopies':hashes[sha(atlas)],
                'originals':[i['path'] for i in originals if i['sha256']==sha(atlas)]})
        active=('assets/'+p.name in cmake and any(s in ui for s in symbols))
        runtime.append({'runtime':rel(p),'canonical':rel(p),'sha256':sha(p),
            'symbols':symbols,'sources':source_items,'encoder':'tools/assets/'+script,
            'status':'referenced-by-current-ui' if active else 'retained-legacy-not-referenced-by-current-ui'})
    names=re.findall(r'\{"([^"\n]+)","([^"\n]+)",0x',(ROOT/'main/apps/monitor_skin.c').read_text(encoding='utf-8'))
    art={1:[],2:[],3:['scene_printer.c'],4:['scene_cat.c','cat_peek.c'],5:['scene_night.c'],6:[],
        7:['paper_tile.c'],8:['art_handheld.c'],9:['art_robot.c','robot_head.c'],10:['art_bamboo.c'],11:['art_space.c','astronaut_head.c'],12:[]}
    for i,name in enumerate(('farm','cyber','ocean','snow','desert','magic'),13): art[i]=['art_'+name+'.c']
    skins=[]
    for i,(zh,en) in enumerate(names,1):
        batch=(i-1)//6+1; folder=ROOT/f'docs/design/stage-{batch:02d}'
        refs=json.loads((folder/'manifest.json').read_text(encoding='utf-8'))
        skins.append({'id':i,'zh':zh,'en':en,'spec':rel(folder/'SPEC.md'),
            'references':[{**p,'file':rel(folder/p['file']),'sourceOriginal':rel((folder/p['sourceOriginal']).resolve())} for p in refs['pages']],
            'runtimeAssets':['main/assets/'+a for a in art[i]],
            'layout':'main/apps/'+('ui_monitor_design.inc' if batch==1 else f'ui_monitor_batch{batch}.inc'),
            'commonGeometry':'main/apps/ui_monitor_layout.inc',
            'decorationNote':'Bamboo header ornament is native geometry in b2_bamboo; old generated bamboo retained.' if i==10 else '',
            'accessoryReferenceStatus':refs.get('accessoriesStatus','see-current-four-page-spec')})
    result={'schema':1,'version':(ROOT/'version.txt').read_text().strip(),'generatedOriginals':originals,
        'runtimeAssets':runtime,'skins':skins,'fonts':{'source':'docs/assets/fonts','generated':'main/fonts',
        'license':'main/fonts/OFL.txt','generator':'tools/assets/generate_fonts.py'},
        'duplicates':[{'sha256':key,'copies':sorted(value),'policy':'retain-all'} for key,value in sorted(hashes.items()) if len(value)>1],
        'counts':{'originals':len(originals),'runtimeAssets':len(runtime),'skins':len(skins),'indexedMediaAndResources':len(records)}}
    if args.check:
        existing=json.loads(OUT.read_text(encoding='utf-8'))
        if existing!=result: raise SystemExit('Asset catalog stale: run python tools/assets/catalog_assets.py')
    else: OUT.write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print('PASS: media coverage, original hashes, 18 skin lineages, runtime/source/encoder mapping; '+('catalog current' if args.check else 'catalog written'))

if __name__=='__main__': main()
