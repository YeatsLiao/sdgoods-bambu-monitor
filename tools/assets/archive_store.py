"""Shared private archive registry for the compact documentation layout."""
from pathlib import Path
from datetime import datetime
from zoneinfo import ZoneInfo
import hashlib,json,zipfile
ROOT=Path(__file__).resolve().parents[2]
REGISTRY=ROOT/'docs/notes/archive-index.json'
def digest(p):
    with Path(p).open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def registry():return json.loads(REGISTRY.read_text(encoding='utf-8'))
def entries():
    return [dict(e,archive=a['file']) for a in registry()['archives'] for e in a['entries']]
def verify_archives():
    data=registry();count=0
    for a in data['archives']:
        path=ROOT/a['file']
        if digest(path)!=a['sha256']:raise RuntimeError('Archive checksum mismatch: '+a['file'])
        with zipfile.ZipFile(path) as z:
            if len(z.namelist())!=len(a['entries'])+1:raise RuntimeError('Archive entry count mismatch: '+a['file'])
            inner=json.loads(z.read('_manifest.json'))
            if inner['files']!=a['entries']:raise RuntimeError('Archive manifest mismatch: '+a['file'])
            for e in a['entries']:
                with z.open(e['path']) as f:actual=hashlib.file_digest(f,'sha256').hexdigest()
                if actual!=e['sha256'] or z.getinfo(e['path']).file_size!=e['bytes']:raise RuntimeError('Archive entry mismatch: '+e['path'])
                count+=1
    return len(data['archives']),count

def archive_files(files,tag,kind='snapshot'):
    files=[Path(p).resolve() for p in files if Path(p).is_file()]
    if not files:return None
    if not tag.replace('-','').replace('_','').replace('.','').isalnum():raise ValueError('Invalid archive tag')
    stamp=datetime.now(ZoneInfo('Asia/Shanghai')).strftime('%Y%m%d-%H%M%S-%f')
    path=ROOT/'backups/history-outputs'/f'{tag}-{stamp}.zip';path.parent.mkdir(parents=True,exist_ok=True)
    manifest=[]
    with zipfile.ZipFile(path,'x',compression=zipfile.ZIP_DEFLATED,compresslevel=1) as z:
        for p in sorted(files):
            if not p.is_relative_to(ROOT) or p.is_symlink():raise ValueError('Unsafe archive source')
            key=p.relative_to(ROOT).as_posix();before=p.stat();h=hashlib.sha256()
            with p.open('rb') as src,z.open(key,'w',force_zip64=True) as dst:
                for b in iter(lambda:src.read(1048576),b''):h.update(b);dst.write(b)
            if p.stat().st_mtime_ns!=before.st_mtime_ns:raise RuntimeError('Archive source changed: '+key)
            manifest.append({'path':key,'bytes':before.st_size,'sha256':h.hexdigest()})
        z.writestr('_manifest.json',json.dumps({'kind':kind,'files':manifest},ensure_ascii=False,indent=2))
    with zipfile.ZipFile(path) as z:
        for e in manifest:
            with z.open(e['path']) as f:
                if hashlib.file_digest(f,'sha256').hexdigest()!=e['sha256']:raise RuntimeError('Archive verification failed')
    data=registry();data['archives'].append({'file':path.relative_to(ROOT).as_posix(),'kind':kind,'bytes':path.stat().st_size,'sha256':digest(path),'verifiedEveryEntry':True,'entries':manifest})
    REGISTRY.write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    path.with_suffix('.sha256').write_text(digest(path)+'  '+path.name+'\n',encoding='utf-8')
    index=ROOT/'docs/notes/archive-index.md'
    with index.open('a',encoding='utf-8') as f:f.write('\n- '+path.relative_to(ROOT).as_posix()+f'：{len(manifest)}个文件，SHA-256 `'+digest(path)+'`。\n')
    return path
