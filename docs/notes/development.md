# Development and Git

## Project

一个ESP-IDF工程，18款皮肤共用数据、网络和设置。官方层边界见 [Architecture](architecture.md)，标准构建及COM6烧录命令见 [项目README](../../README.md)。

## Git scope

提交main、components、platform、tools及全部当前docs：阶段原型、源图、字体许可、设备反馈、精选软件图、归档索引。仅main/bambu/config.local.h、本机sdkconfig、依赖缓存、构建输出build/dist和私有backups不提交。原始生成图和旧稿不会被整体忽略。

历史批量输出已按版本保存在backups/history-outputs；Git只保存archive-index.json，不包含私有ZIP。只克隆Git可以检查本地文档与素材；全量历史检查还需要这些ZIP。完整备份可能含凭据，按私有文件保管。同盘副本不等于异盘容灾。

## Tools

- 截图：根目录串口截图.cmd，保存至docs/screenshots/device。
- 原生预览：python tools/preview/render_preview.py，批量帧输出至build/preview/frames。
- README图：python tools/assets/export_showcase.py，输出至docs/screenshots/software。
- 素材索引：python tools/assets/archive_images.py，然后python tools/assets/catalog_assets.py。
- 本地检查：python tools/checks/check_docs.py、python tools/assets/catalog_assets.py --check。
- 历史包校验：python tools/checks/check_docs.py --archives。
- 完整私有快照：python tools/maintenance/backup_project.py --label v0.3.9-compact-docs。

旧编码只在历史包中保留；当前工作编码直接使用main/assets和main/fonts，不再由docs维护第二套。已有提示词随阶段保存；旧报告全文及旧索引在source-records.zip。

0.3.9设置触控待真机确认，第二/三阶段HT页仍为主题延展候选。后续修改必须保留真实未知值、线程边界和官方组件，更新资料不会自动升级验收状态。
