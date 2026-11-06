#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Windows double-click screenshot entry point, using the official serial protocol."""
import argparse
from datetime import datetime
import os
from pathlib import Path
import sys

from screenshot_recv import run

ROOT = Path(__file__).resolve().parents[2]
OUTPUT_DIR = ROOT / 'docs' / 'verification' / 'device' / 'screenshots'


def main(argv=None):
    parser = argparse.ArgumentParser(description='串口截图：默认 COM6，自动保存并打开图片')
    parser.add_argument('--port', default='COM6', help='串口，默认 COM6')
    parser.add_argument('--output-dir', type=Path, default=OUTPUT_DIR)
    parser.add_argument('--no-open', action='store_true', help='保存后不打开图片')
    args = parser.parse_args(argv)

    try:
        folder = args.output_dir.resolve()
        folder.mkdir(parents=True, exist_ok=True)
        filename = 'shot_' + datetime.now().strftime('%Y%m%d_%H%M%S_%f')
        output = folder / (filename + '.png')
        print('请保持设备停留在要截图的页面，并关闭串口监视器。', flush=True)
        status = run(args.port, 115200, 1, str(output), 120, True)
        if status:
            return status
        # The device normally sends JPEG; the receiver changes .png to .jpg.
        saved = output.with_suffix('.jpg') if output.with_suffix('.jpg').is_file() else output
        if not saved.is_file():
            raise OSError('接收结束但没有找到截图文件')
        print('\n截图已保存：' + str(saved), flush=True)
        if not args.no_open:
            try:
                os.startfile(str(saved))
            except OSError as error:
                print('图片已保存，自动打开失败：%s' % error, file=sys.stderr)
        return 0
    except KeyboardInterrupt:
        print('\n已取消截图。', file=sys.stderr)
        return 130
    except Exception as error:
        print('截图失败：%s' % error, file=sys.stderr)
        return 1


if __name__ == '__main__':
    sys.exit(main())
