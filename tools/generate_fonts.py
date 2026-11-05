#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Generate application-only fonts without changing any vendor files."""
from pathlib import Path
import argparse
import re
import subprocess
ROOT=Path(__file__).resolve().parents[1]
def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--converter',required=True,help='lv_font_conv.js path')
    args=parser.parse_args()
    sources=[ROOT/'main/apps/ui_monitor.c',ROOT/'main/apps/monitor_skin.c']+list((ROOT/'main/apps').glob('*.inc'))
    # Only actual C string literals belong in the UI subset. Comments, baseline
    # template text and identifier names must not enlarge every Chinese font.
    token=re.compile(r'"(?:\\.|[^"\\])*"|//[^\n]*|/\*[\s\S]*?\*/')
    literals=[m.group(0) for p in sources for m in token.finditer(p.read_text(encoding='utf-8')) if m.group(0).startswith('"')]
    chars=set(''.join(literals))
    print('UI Chinese subset:',len({c for c in chars if '\u4e00'<=c<='\u9fff'}))
    symbols=''.join(sorted(c for c in chars if ord(c)>127))+''.join(chr(c) for c in range(32,127))
    for size in (16,20):
        target=ROOT/f'main/fonts/bambu_font_{size}.c'
        subprocess.run(['node',args.converter,'--font',str(ROOT/'tools/fonts/NotoSansSC-Regular.ttf'),
            '--size',str(size),'--bpp','4','--format','lvgl','--lv-include','lvgl.h','--symbols',symbols,
            '--no-compress','-o',str(target)],check=True)
        target.write_text('/* Noto Sans SC glyphs: SIL Open Font License 1.1. See tools/fonts/README.md. */\n'+target.read_text(encoding='utf-8'),encoding='utf-8')
    target=ROOT/'main/fonts/bambu_digits_72.c'
    subprocess.run(['node',args.converter,'--font',str(ROOT/'tools/fonts/NotoSansSC-Regular.ttf'),
        '--size','72','--bpp','4','--format','lvgl','--lv-include','lvgl.h','--symbols','0123456789%--',
        '--no-compress','-o',str(target)],check=True)
    target.write_text('/* Noto Sans SC glyphs: SIL Open Font License 1.1. See tools/fonts/README.md. */\n'+target.read_text(encoding='utf-8'),encoding='utf-8')
    bold=ROOT/'tools/fonts/NotoSansSC-Bold.otf'
    for name,size,glyphs in [(f'bambu_bold_{s}',s,symbols) for s in (18,22,26)]+[(f'bambu_num_{s}',s,'0123456789%/-°C.') for s in (44,64,88,128)]:
        target=ROOT/f'main/fonts/{name}.c'
        source=ROOT/'tools/fonts/NotoSansSC-Black-Numbers.ttf' if name.startswith('bambu_num_') else bold
        subprocess.run(['node',args.converter,'--font',str(source),'--size',str(size),
            '--bpp','4','--format','lvgl','--lv-include','lvgl.h','--symbols',glyphs,
            '--no-compress','-o',str(target)],check=True)
        target.write_text('/* Noto Sans SC Bold / Black numeric subset: SIL OFL 1.1, see main/fonts/OFL.txt. */\n'+target.read_text(encoding='utf-8'),encoding='utf-8')
    print('Application fonts generated; official platform fonts unchanged.')
if __name__=='__main__':main()
