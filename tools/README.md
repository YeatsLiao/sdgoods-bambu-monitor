# Tools

从项目根目录执行。固件使用标准idf.py build / merge-bin / -p COM6 flash，一个工程包含18款皮肤。

- firmware：构建、烧录、包类型和容量检查、应用导出。
- device：串口截图与只读硬件探测。根目录串口截图.cmd保存到docs/screenshots/device；探测原始数据保存到backups/device-probes。
- assets：插画/字体编码、源素材归档、追溯索引、精选预览导出、私有归档注册。
- preview：原生LVGL渲染、宿主桩和拼图；原始批量帧输出到build/preview/frames。
- checks：环境、文档、官方平台、C解析器和编码回归。
- maintenance：完整私有快照和已停用的一次性历史迁移工具。
- fonts、fixtures：字体工作源与测试输入。

## Common commands

```powershell
python tools/device/capture_screenshot.py
python tools/preview/render_preview.py
python tools/assets/export_showcase.py
python tools/assets/archive_images.py
python tools/assets/catalog_assets.py
python tools/checks/check_docs.py
python tools/checks/check_docs.py --archives
python tools/assets/catalog_assets.py --check
python tools/checks/check_platform.py
node tools/checks/test_art_codec.cjs
python tools/firmware/pack_app.py --name BambuMonitor_app.bin
```

export_showcase支持原生PPM帧，不要求先生成上千张PNG；它只导出README需要的7张精选图。被替换精选图会先打包保存。

全量历史包在backups/history-outputs，目录与恢复规则见docs/notes/archive-index.md。完整备份工具会把这些历史ZIP作为输入收录，但不会再把其他完整备份套进去。备份可能包含本地配置，按私有文件保管。
