#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Verify idf.py merge-bin output against its actual flash component map."""
from pathlib import Path
import argparse,hashlib,json
ROOT=Path(__file__).resolve().parents[2]
def main():
    p=argparse.ArgumentParser();p.add_argument('--build-dir',required=True);p.add_argument('--image',required=True)
    args=p.parse_args();build=ROOT/args.build_dir;image=ROOT/args.image
    config=json.loads((build/'flasher_args.json').read_text(encoding='utf-8'))
    merged=image.read_bytes();parts=[]
    for offset,name in config['flash_files'].items():
        address=int(offset,0);src=build/name;data=src.read_bytes()
        if merged[address:address+len(data)]!=data:raise SystemExit('Component mismatch: '+name)
        parts.append({'offset':offset,'file':name,'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest()})
    result={'kind':'full-flash-image','chip':'esp32s3','flashOffset':'0x0','bytes':len(merged),
        'sha256':hashlib.sha256(merged).hexdigest(),'components':parts,
        'nvsIncludedAsFF':all(b==255 for b in merged[0x9000:0xf000]),
        'endOffset':hex(len(merged)),'writesOtaSlots':len(merged)>0x320000}
    if result['writesOtaSlots']:raise SystemExit('Unexpected full image extends into ota_0')
    image.with_suffix('.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
    print('PASS: merged image components match; bytes=',len(merged),'sha256=',result['sha256'])
if __name__=='__main__':main()
