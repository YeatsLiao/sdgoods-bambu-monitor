# 工具入口

从项目根目录执行以下命令。固件仍使用标准idf.py build / merge-bin / -p COM6 flash流程。

- firmware：构建、完整镜像检查、应用打包与本地烧录。打包：python tools/firmware/pack_app.py --name BambuMonitor_app.bin。
- device：串口截图和只读设备探测。双击根目录串口截图.cmd，或运行python tools/device/capture_screenshot.py。
- assets：插画/字体编码、README预览导出、原图归档和追溯索引。
- preview：真实LVGL渲染器、宿主桩和PNG拼版。运行python tools/preview/render_preview.py。
- checks：文档、平台、环境、实际C解析器和图片编码测试。
- maintenance：完整私有备份及一次性历史迁移工具。organize_docs.py已执行过，保留追溯，不应重复运行。
- fonts：字体工作源与转换说明；归档源另存docs/assets/fonts。
- fixtures：解析测试输入。

常用检查：

```powershell
python tools/checks/check_docs.py
python tools/assets/catalog_assets.py --check
python tools/checks/check_platform.py
node tools/checks/test_art_codec.cjs
```

更新图片后：先运行对应编码/渲染工具，再执行python tools/assets/export_showcase.py、python tools/assets/archive_images.py --source docs/assets/generated-originals、python tools/assets/catalog_assets.py。

构建输出build和dist可重新生成；旧交付物已移入docs/history/releases/firmware。完整私有快照保存在backups，覆盖Git忽略的媒体与本地配置。
