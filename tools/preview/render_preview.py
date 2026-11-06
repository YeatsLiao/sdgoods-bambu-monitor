#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Compile and render the real LVGL view on Windows without flashing a device."""
from pathlib import Path
import subprocess,os
ROOT=Path(__file__).resolve().parents[2]
def main():
    out=ROOT/'build/preview';out.mkdir(parents=True,exist_ok=True)
    shots=ROOT/'docs/verification/preview';shots.mkdir(parents=True,exist_ok=True)
    cc='D:/1.Soft/msys64/mingw64/bin/gcc.exe'
    env=dict(os.environ);env['PATH']=str(Path(cc).parent)+os.pathsep+env.get('PATH','')
    sources=list((ROOT/'components/lvgl/src').rglob('*.c'))
    sources += [ROOT/'tools/preview/render.c',ROOT/'main/apps/monitor_skin.c',ROOT/'main/apps/monitor_art.c',ROOT/'main/bambu/bambu_state.c',ROOT/'main/bambu/bambu_pages.c']
    sources += list((ROOT/'main/fonts').glob('*.c'))+list((ROOT/'main/assets').glob('*.c'))
    args=['-std=c11','-O1','-DLV_CONF_SKIP','-DLV_COLOR_DEPTH=16','-DLV_COLOR_16_SWAP=1',
        '-DLV_FONT_MONTSERRAT_20=1','-DLV_FONT_MONTSERRAT_28=1','-DLV_FONT_MONTSERRAT_48=1',
        '-DLV_MEM_SIZE=4194304','-I'+str(ROOT/'tools/preview/stubs'),'-I'+str(ROOT/'components/lvgl'),
        '-I'+str(ROOT/'components'),'-I'+str(ROOT/'main/apps'),'-I'+str(ROOT/'main/bambu')]+[str(p) for p in sources]+['-lm','-o',str(out/'render.exe')]
    rsp=out/'compile.rsp';rsp.write_text('\n'.join('"'+a.replace('\\','/')+'"' for a in args),encoding='utf-8')
    subprocess.run([cc,'@'+str(rsp)],check=True,cwd=ROOT,env=env)
    subprocess.run([str(out/'render.exe'),str(shots)],check=True,cwd=ROOT,env=env)
if __name__=='__main__':main()
