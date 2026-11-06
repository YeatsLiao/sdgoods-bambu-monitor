#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Read chip/partition/app headers without writing flash; recommend a safe app slot."""
from pathlib import Path
import argparse,datetime,json,os,struct,sys
import esptool
ROOT=Path(__file__).resolve().parents[2]
def app_name(header):
    if len(header)>=112 and header[0]==0xe9 and struct.unpack_from('<I',header,32)[0]==0xabcd5432:
        return header[80:112].split(b'\0')[0].decode('ascii','replace')
    return None
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--port',required=True)
    parser.add_argument('--idf',default=os.environ.get('IDF_PATH','D:/1.Soft/Espressif/frameworks/esp-idf-v5.5.5'))
    args=parser.parse_args()
    sys.path.insert(0,str(Path(args.idf)/'components/partition_table'))
    from gen_esp32part import PartitionTable
    folder=ROOT/'docs/verification/device'/(args.port+'-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S'))
    folder.mkdir(parents=True,exist_ok=True)
    chip=esptool.detect_chip(port=args.port,baud=115200)
    loader=chip
    try:
        if chip.CHIP_NAME!='ESP32-S3':raise RuntimeError('Expected ESP32-S3; refusing other chip')
        loader=chip.run_stub();loader.change_baud(460800)
        raw=loader.read_flash(0x8000,0x1000);(folder/'partition-table.bin').write_bytes(raw)
        table=PartitionTable.from_binary(raw)
        factory=next((p for p in table if p.type==0 and p.subtype==0),None)
        if factory is None:raise RuntimeError('No factory partition found')
        header=loader.read_flash(factory.offset,512);(folder/'factory-header.bin').write_bytes(header)
        factory_id=app_name(header)
        slots=[]
        for p in table:
            if p.type!=0 or not 0x10<=p.subtype<=0x1f:continue
            head=loader.read_flash(p.offset,512);(folder/(p.name+'-header.bin')).write_bytes(head)
            slots.append({'slot':p.subtype-0x10,'name':p.name,'offset':p.offset,'offsetHex':hex(p.offset),
                'size':p.size,'appId':app_name(head),'emptyHeader':all(b==255 for b in head),'encrypted':p.encrypted})
        candidates=[p for p in slots if p['appId']=='BambuMonitor' and not p['encrypted']]
        if not candidates:candidates=[p for p in slots if p['emptyHeader'] and not p['encrypted']]
        result={'port':args.port,'chip':chip.CHIP_NAME,'factoryAppId':factory_id,
            'officialLauncher':factory_id=='SDGOODS_LAUNCHER','slots':slots,
            'recommended':candidates[0] if factory_id=='SDGOODS_LAUNCHER' and candidates else None,
            'flashWritten':False}
        output=folder/'probe.json';output.write_text(json.dumps(result,indent=2),encoding='utf-8')
        print(json.dumps(result,indent=2));print('Probe report:',output)
    finally:
        try:loader.hard_reset()
        finally:loader._port.close()
if __name__=='__main__':main()
