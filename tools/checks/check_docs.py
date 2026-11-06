#!/usr/bin/env python3
"""Validate compact docs; --archives additionally verifies every ZIP payload."""
from pathlib import Path
import argparse,hashlib,json,re,sys
from urllib.parse import unquote
ROOT=Path(__file__).resolve().parents[2];DOCS=ROOT/'docs'
sys.path.insert(0,str(ROOT/'tools/assets'))
from archive_store import entries,verify_archives,digest

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--archives',action='store_true');args=parser.parse_args()
    errors=[];refs=[]
    if {p.name for p in DOCS.iterdir() if p.is_dir()}!={'design','screenshots','assets','notes'}:errors.append('Unexpected top-level docs folders')
    for p in DOCS.rglob('*'):
        if p.is_dir() and len(p.relative_to(DOCS).parts)>2:errors.append('Docs directory too deep: '+str(p))
    for i in (1,2,3):
        folder=DOCS/f'design/stage-{i:02d}';data=json.loads((folder/'manifest.json').read_text(encoding='utf-8'))
        if i==1 and [p['role'] for p in data['pages']]!=['overview','ams','external-ht','details']:errors.append('Page order mismatch')
        for item in data['pages']:
            p=folder/item['file'];source=folder/item['sourceOriginal']
            if not p.is_file() or digest(p)!=item['sha256']:errors.append('Reference mismatch: '+str(p))
            if not source.is_file():errors.append('Missing reference source: '+str(source))
            refs.append(p)
    m=json.loads((DOCS/'assets/image-manifest.json').read_text(encoding='utf-8'));hashes=set()
    records=m['images']+m['resources'];indexed={r['path'] for r in records}
    for item in records:
        p=ROOT/item['path']
        if not p.is_file() or digest(p)!=item['sha256']:errors.append('Indexed file mismatch: '+item['path'])
        hashes.add(item['sha256'])
    for item in m['generatedOriginals']:
        if item['path'] not in indexed:errors.append('Original not indexed: '+item['path'])
    media_ext={'.png','.jpg','.jpeg','.ppm','.gif','.webp','.ttf','.otf','.bin'}
    for p in DOCS.rglob('*'):
        if p.is_file() and p.suffix.lower() in media_ext and p.relative_to(ROOT).as_posix() not in indexed:errors.append('Unindexed local media: '+str(p))
    archived=entries();hashes.update(e['sha256'] for e in archived)
    migration=json.loads((DOCS/'notes/migration.json').read_text(encoding='utf-8'))
    for path,item in migration['baselineFiles'].items():
        if item['sha256'] not in hashes:errors.append('Original bytes not retained: '+path)
    for item in migration['legacyMedia']:
        if item['sha256'] not in hashes:errors.append('Legacy resource lost: '+item['path'])
    # Canonical runtime files are tracked directly, not shadow copies under docs.
    for folder in ('main/assets','main/fonts'):
        for p in (ROOT/folder).glob('*.c'):
            if p.relative_to(ROOT).as_posix() not in indexed:errors.append('Runtime resource not indexed: '+str(p))
    links=re.compile(r'!?\[[^\]]*\]\(([^\n]*?)\)')
    for p in [ROOT/'README.md',*DOCS.rglob('*.md'),*(ROOT/'tools').rglob('*.md')]:
        body=re.sub(r'```[\s\S]*?```','',p.read_text(encoding='utf-8'))
        for match in links.finditer(body):
            target=match[1].strip('<>')
            if not target or target.startswith(('#','http:','https:','mailto:','codex:','plugin:')) or ':/' in target:continue
            if not (p.parent/unquote(target.split('#',1)[0])).exists():errors.append('Broken link: '+str(p.relative_to(ROOT))+' -> '+target)
    if errors:print('\n'.join(errors));return 1
    if args.archives:
        packages,count=verify_archives();print(f'PASS: {packages} private archives; {count} entries decompressed and SHA-256 verified.')
    else:print('Archive coverage checked against registry metadata; use --archives to verify private ZIP payloads.')
    print(f'PASS: 4 docs folders, max 2 subfolder levels; {len(refs)} references; {len(m["generatedOriginals"])} originals; {len(m["images"])} local images; all {len(migration["baselineFiles"])} former docs files and {len(migration["legacyMedia"])} legacy resources retained; links valid.')
    return 0
if __name__=='__main__':sys.exit(main())
