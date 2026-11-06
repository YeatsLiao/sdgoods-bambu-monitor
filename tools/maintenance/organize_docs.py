#!/usr/bin/env python3
"""One-time, lossless docs migration. Keeps every image/resource and a path ledger."""
from pathlib import Path
import hashlib, json, re, shutil, os

ROOT = Path(__file__).resolve().parents[2]
DOCS = ROOT / 'docs'
HISTORY = DOCS / 'history'
MEDIA = {'.png', '.jpg', '.jpeg', '.webp', '.ppm', '.gif', '.c', '.ttf', '.otf', '.bin'}

def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()

def checked(p):
    p = p.resolve()
    if p == ROOT or not p.is_relative_to(ROOT) or p.is_symlink():
        raise RuntimeError('Unsafe migration path: ' + str(p))
    return p

def main():
    ledger = HISTORY / 'migration-20261105.json'
    if ledger.exists():
        raise SystemExit('Migration already recorded; use tools/checks/check_docs.py to verify.')
    before = [{'path':p.relative_to(ROOT).as_posix(),'sha256':sha(p),'bytes':p.stat().st_size}
              for p in sorted(DOCS.rglob('*')) if p.is_file() and p.suffix.lower() in MEDIA]
    old_md = {p.resolve():p.read_text(encoding='utf-8') for p in DOCS.rglob('*.md')}
    old_md[(ROOT/'README.md').resolve()] = (ROOT/'README.md').read_text(encoding='utf-8')
    mapping = {}
    moves = []
    def move(src, dst):
        src, dst = checked(ROOT/src), checked(ROOT/dst)
        if not src.exists(): return
        if dst.exists(): raise RuntimeError('Destination exists: ' + str(dst))
        if src.is_dir():
            for child in src.rglob('*'):
                if child.is_file(): mapping[child.resolve()] = dst / child.relative_to(src)
        else: mapping[src] = dst
        dst.parent.mkdir(parents=True,exist_ok=True)
        src.rename(dst)
        moves.append({'from':str(src.relative_to(ROOT)).replace('\\','/'),
                      'to':str(dst.relative_to(ROOT)).replace('\\','/')})
    def preserve_text(src, dst):
        target=ROOT/dst;target.parent.mkdir(parents=True,exist_ok=True)
        shutil.copy2(ROOT/src,target)
    preserve_text('docs/design/README.md','docs/history/records/design-decisions-before-20261105.md')
    preserve_text('docs/assets/GENERATION.md','docs/history/records/asset-generation-before-20261105.md')
    move('docs/ARCHITECTURE.md','docs/architecture/overview.md')
    move('docs/UPSTREAM.md','docs/architecture/upstream.md')
    move('docs/DEVELOPMENT-PLAN.md','docs/history/records/development-plan-before-20261105.md')
    move('docs/verification/releases','docs/history/releases')
    for p in list((DOCS/'verification').iterdir()):
        if p.is_file() and p.name!='platform-snapshot.json':
            move(p.relative_to(ROOT),Path('docs/history/verification')/p.name)
    move('docs/design/batch-01/page-3-external-ht-v1.png','docs/design/batch-01/page-3-external-ht.png')
    move('docs/design/batch-01/page-3-external-ht-v1.prompt.md','docs/history/design/batch-01/page-3-external-ht-v1.prompt.md')
    # The user already renamed this file to page-4-details; support either state.
    move('docs/design/batch-01/page-3-details.png','docs/design/batch-01/page-4-details.png')
    for n in (2,3):
        move(f'docs/design/batch-{n:02d}/page-3-details.png',f'docs/design/batch-{n:02d}/page-4-details.png')
    move('docs/design/settings/settings-flow.png','docs/history/design/settings/settings-flow-v1.png')
    move('docs/design/settings/settings-overview.png','docs/history/design/settings/settings-implementation-v0.3.2.png')
    # Hash-suffixed material snapshots are history, not current source mirrors.
    for folder in ('runtime-encoded','fonts'):
        for p in list((DOCS/'assets'/folder).rglob('*')):
            if p.is_file() and re.search(r'-[0-9a-f]{12}$',p.stem):
                move(p.relative_to(ROOT),Path('docs/history/assets')/p.relative_to(DOCS/'assets'))
    # Old experiment build directories/logs no longer occupy the project root.
    move('build-preview','build/preview')
    for p in list(ROOT.iterdir()):
        if p.is_dir() and p.name.startswith('build-'):
            move(p.name,Path('build/history')/p.name)
        elif p.is_file() and (p.suffix=='.log' or (p.name.startswith('build-') and p.suffix=='.defaults')):
            move(p.name,Path('build/history')/('logs' if p.suffix=='.log' else 'configs')/p.name)
    # Rebase every existing Markdown link relative to both its old and new file.
    link=re.compile(r'(!?\[[^\]]*\]\()([^\n]*?)(\))')
    for old,body in old_md.items():
        dest=mapping.get(old,old)
        if not dest.exists(): continue
        def rewrite(m):
            raw=m.group(2);url=raw.strip('<>')
            if not url or url.startswith(('#','http:','https:','mailto:','plugin:','codex:','app:')) or ':/' in url:
                return m.group(0)
            path,sep,anchor=url.partition('#')
            target=(old.parent/path).resolve()
            new_target=mapping.get(target,target)
            # Directory links need to follow directory moves as well.
            for entry in sorted(moves,key=lambda x:len(x['from']),reverse=True):
                src=(ROOT/entry['from']).resolve()
                if target==src or target.is_relative_to(src):
                    new_target=(ROOT/entry['to'])/target.relative_to(src);break
            rel=os.path.relpath(new_target,dest.parent).replace('\\','/')
            if sep:rel+='#'+anchor
            return m.group(1)+rel+m.group(3)
        dest.write_text(link.sub(rewrite,body),encoding='utf-8')
    HISTORY.mkdir(parents=True,exist_ok=True)
    ledger.write_text(json.dumps({'mediaBefore':before,'moves':moves},ensure_ascii=False,indent=2),encoding='utf-8')
    # Update source/tool references; application behavior stays unchanged.
    p=ROOT/'tools/preview/render_preview.py'
    p.write_text(p.read_text(encoding='utf-8').replace("ROOT/'build-preview'","ROOT/'build/preview'"),encoding='utf-8')
    for p in (ROOT/'main/bambu').iterdir():
        if p.suffix in ('.c','.h'):
            p.write_text(p.read_text(encoding='utf-8').replace('docs/UPSTREAM.md','docs/architecture/upstream.md'),encoding='utf-8')
    (ROOT/'build/history/README.md').write_text('# 历史构建缓存\n\n仅保留此前分批测量的缓存和日志，不作为当前构建入口。缓存内的旧绝对路径可能失效；当前工程始终使用根目录标准 `idf.py build`，不使用 Variant 或 build-batch 参数。\n',encoding='utf-8')
    print('Migrated',len(moves),'paths; inventoried',len(before),'original images/resources. No deletions.')

if __name__=='__main__':main()
