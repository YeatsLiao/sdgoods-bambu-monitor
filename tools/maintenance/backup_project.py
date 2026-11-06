#!/usr/bin/env python3
"""Private, lossless project snapshot, including Git-ignored documentation/media.

Excludes rebuildable caches and .git, but includes local config and flash outputs.
Never upload this archive automatically: local configuration can contain secrets.
"""
from pathlib import Path
import argparse, hashlib, json, zipfile
from datetime import datetime
from zoneinfo import ZoneInfo

ROOT = Path(__file__).resolve().parents[2]
SKIP = {'.git', 'backups', 'node_modules', '__pycache__', 'managed_components'}

def digest(path):
    with path.open('rb') as f:
        return hashlib.file_digest(f, 'sha256').hexdigest()

def verify(path):
    with zipfile.ZipFile(path) as z:
        manifest = json.loads(z.read('_backup_manifest.json'))
        assert len(z.namelist()) == len(manifest['files']) + 1
        for item in manifest['files']:
            with z.open(item['path']) as f:
                actual = hashlib.file_digest(f, 'sha256').hexdigest()
            if actual != item['sha256'] or z.getinfo(item['path']).file_size != item['bytes']:
                raise RuntimeError('Backup mismatch: ' + item['path'])
    print(f'PASS: verified every archive entry; files={len(manifest["files"])}', flush=True)

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--label', default='snapshot')
    p.add_argument('--verify', type=Path)
    args = p.parse_args()
    if args.verify:
        verify(args.verify); return
    if not args.label.replace('-', '').replace('_', '').replace('.', '').isalnum():
        raise SystemExit('Invalid label')
    out = ROOT/'backups'; out.mkdir(exist_ok=True)
    stamp = datetime.now(ZoneInfo('Asia/Shanghai')).strftime('%Y%m%d-%H%M%S')
    target = out/f'{stamp}-{args.label}.zip'
    files = []
    for f in sorted(ROOT.rglob('*')):
        if not f.is_file(): continue
        rel = f.relative_to(ROOT)
        if any(part in SKIP for part in rel.parts): continue
        if rel.parts[0].startswith('build'):
            if rel.parts[0] != 'build': continue
            keep = len(rel.parts) == 2 and f.suffix in {'.bin','.elf','.map','.json'}
            keep |= rel.parts[:2] == ('build','log')
            keep |= rel.as_posix() in {'build/bootloader/bootloader.bin','build/partition_table/partition-table.bin'}
            if not keep: continue
        files.append(f)
    manifest = {'created': stamp, 'timezone':'Asia/Shanghai',
        'version': (ROOT/'version.txt').read_text().strip(),
        'scope':'All source, dependencies, docs (including ignored history/previews), local config, dist and selected flash outputs/logs.',
        'excluded':sorted(SKIP)+['rebuildable build caches'],
        'private': True, 'files': []}
    with zipfile.ZipFile(target, 'x', compression=zipfile.ZIP_DEFLATED, compresslevel=1, allowZip64=True) as z:
        for i,f in enumerate(files):
            before = f.stat(); key = f.relative_to(ROOT).as_posix(); sha = hashlib.sha256()
            with f.open('rb') as src, z.open(key, 'w', force_zip64=True) as dst:
                for chunk in iter(lambda:src.read(1024*1024), b''):
                    sha.update(chunk); dst.write(chunk)
            after = f.stat()
            if before.st_size != after.st_size or before.st_mtime_ns != after.st_mtime_ns:
                raise RuntimeError('File changed during snapshot: ' + key)
            manifest['files'].append({'path':key,'bytes':after.st_size,'sha256':sha.hexdigest()})
            if (i+1)%1000==0: print(f'Archived {i+1}/{len(files)} files',flush=True)
        z.writestr('_backup_manifest.json', json.dumps(manifest,ensure_ascii=False,indent=2))
    verify(target)
    checksum = digest(target)
    target.with_suffix('.sha256').write_text(checksum+'  '+target.name+'\n', encoding='utf-8')
    target.with_suffix('.json').write_text(json.dumps({k:v for k,v in manifest.items() if k!='files'}|{
        'archive':target.name,'files':len(files),'bytes':target.stat().st_size,'sha256':checksum,
        'verifiedEveryEntry':True}, ensure_ascii=False,indent=2),encoding='utf-8')
    print('BACKUP: '+str(target),flush=True)
    print('SHA256: '+checksum,flush=True)

if __name__ == '__main__': main()
