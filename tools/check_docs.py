#!/usr/bin/env python3
"""Validate current design references, links, current mirrors and lossless migration."""
from pathlib import Path
import hashlib,json,re,sys
from urllib.parse import unquote

ROOT=Path(__file__).resolve().parents[1]
DOCS=ROOT/'docs'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()

def main():
    errors=[]
    registry=json.loads((DOCS/'design/batch-01/manifest.json').read_text(encoding='utf-8'))
    if [p['role'] for p in registry['pages']]!=['overview','ams','external-ht','details']:
        errors.append('Current page order differs from the confirmed four-page contract')
    references=[(DOCS/'design/batch-01',item) for item in registry['pages']]
    for batch in (2,3):
        folder=DOCS/f'design/batch-{batch:02d}'
        other=json.loads((folder/'manifest.json').read_text(encoding='utf-8'))
        references.extend((folder,item) for item in other['pages'])
    for folder,item in references:
        p=folder/item['file']
        if not p.is_file() or sha(p)!=item['sha256']:errors.append('Reference mismatch: '+str(p))
        source=folder/item['sourceOriginal']
        if not source.is_file():errors.append('Missing original: '+str(source))
    for folder in ('runtime-encoded','fonts/generated'):
        source=ROOT/('main/assets' if folder=='runtime-encoded' else 'main/fonts')
        for p in source.iterdir():
            if not p.is_file() or (folder=='runtime-encoded' and p.suffix!='.c'):continue
            q=DOCS/'assets'/folder/p.name
            if not q.is_file() or sha(p)!=sha(q):errors.append('Current resource mirror mismatch: '+str(q))
    manifest=json.loads((DOCS/'assets/image-manifest.json').read_text(encoding='utf-8'))
    hashes=set()
    for item in manifest['images']+manifest['resources']:
        p=ROOT/item['path']
        if not p.is_file() or sha(p)!=item['sha256']:errors.append('Indexed bytes mismatch: '+item['path'])
        else:hashes.add(item['sha256'])
    ledger=json.loads((DOCS/'history/migration-20261105.json').read_text(encoding='utf-8'))
    for item in ledger['mediaBefore']:
        if item['sha256'] not in hashes:errors.append('Lost pre-migration bytes: '+item['path'])
    links=re.compile(r'!?\[[^\]]*\]\(([^\n]*?)\)')
    for p in [ROOT/'README.md',*DOCS.rglob('*.md')]:
        body=p.read_text(encoding='utf-8')
        body=re.sub(r'```[\s\S]*?```','',body)
        for m in links.finditer(body):
            target=m.group(1).strip('<>')
            if not target or target.startswith(('#','https:','http:','mailto:','codex:','plugin:')) or ':/' in target:continue
            target=unquote(target.split('#',1)[0])
            if not (p.parent/target).exists():errors.append('Broken link: '+str(p.relative_to(ROOT))+' -> '+target)
    if errors:
        print('\n'.join(errors));return 1
    print(f'PASS: {len(references)} original/current references; {len(ledger["mediaBefore"])} pre-migration image/resource records retained; {len(manifest["images"])} indexed images; all Markdown links and current resource mirrors valid.')
    return 0
if __name__=='__main__':sys.exit(main())
