#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Record and verify the unchanged official layer against the local radio snapshot."""
from pathlib import Path
import hashlib,json,argparse
ROOT=Path(__file__).resolve().parents[2]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--reference',default='D:/2.Project/SDGOODS/sdgoods-radio')
    args=parser.parse_args();ref=Path(args.reference)
    files=[];errors=[]
    for folder in ('components/sdgoods_board','components/sdgoods_launcher','components/jpegenc','platform'):
        for p in sorted((ROOT/folder).rglob('*')):
            if not p.is_file():continue
            relative=p.relative_to(ROOT);up=ref/relative
            digest=sha(p)
            if not up.is_file() or sha(up)!=digest:errors.append(str(relative))
            files.append({'path':relative.as_posix(),'sha256':digest})
    output=ROOT/'docs/verification/reports/platform-snapshot.json';output.parent.mkdir(parents=True,exist_ok=True)
    output.write_text(json.dumps({'reference':str(ref),'matches':not errors,'files':files,'differences':errors},indent=2),encoding='utf-8')
    if errors:raise SystemExit('Platform mismatch: '+', '.join(errors))
    print(f'PASS: {len(files)} official files match the radio project byte for byte.')
if __name__=='__main__':main()
