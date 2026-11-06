# 工具与文档分类整理 · 2026-11-06

## 结果

工具脚本归入tools/firmware、device、assets、preview、checks、maintenance；根目录tools仅保留README和功能目录，fonts和fixtures位置保持不变。

docs根目录仅保留README。Git说明移入docs/architecture/git.md；版本报告和对应JSON归入docs/verification/reports，当前状态、真机反馈和批量预览分开保留。原型和原始图片路径不变。

42个工具/说明/报告文件迁移，旧dist的16个交付文件归入docs/history/releases/firmware。详细路径见 [迁移表](../../history/records/tool-doc-migration-20261106.json)。

build中的90条图片/字体/固件/日志记录已逐项按哈希确认存在归档，见 [清理清单](../../history/records/build-cleanup-media-20261106.json)，随后删除可重建的build缓存。dist根目录清空后移除；backups继续保留完整私有快照。

## 校验

脚本项目根路径、Python/Node/PowerShell语法、截图命令帮助、实际C解析器、压缩编码、官方包类型/容量、文档链接及官方82文件比对通过。双击截图仍走根目录串口截图.cmd。迁移只调整工具路径及文档，生产应用源码不变。

归档和目录迁移后重新生成image-manifest及catalog，保留24张原图和全部历史媒体。

## 后续使用

再次烧录前需要重新编译，标准命令仍为idf.py build、idf.py merge-bin、复制merged-binary.bin、idf.py -p COM6 flash。build和dist会按需重新生成。

工具命令以 [工具入口](../../../tools/README.md) 为准。官方应用包导出改为python tools/firmware/pack_app.py --name BambuMonitor_app.bin。

整理前检查点：backups/20261106-161439-v0.3.9-before-tool-reorganization.zip，8,816项逐项哈希验证通过。整理后再保存完整快照；backups可能含本地凭据，不提交Git。
