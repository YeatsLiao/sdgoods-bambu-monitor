# 0.3.9 收尾记录 · 2026-11-06

本轮仅整理资料、索引、保存策略和归档工具；生产源码、字体、原始图像、官方组件及固件版本未改。未删除素材，未搬动原始图像路径，故没有媒体路径迁移。

## 保存范围

整理前完整私有ZIP：backups/20261106-134542-v0.3.9-before-closeout.zip。8,784个文件逐项解压SHA-256验证通过；ZIP SHA-256为 ec765131497c8d4c97ba2d56cb7d9926205cbbbb1fc44e4896ab473af4540428。

包含全部源码、离线依赖、docs（含被Git忽略的历史和预览）、本地配置、dist、标准构建的固件/映射/烧录清单及日志；排除.git、可重建缓存、node_modules、managed_components、__pycache__和备份自身。该快照不是Git历史或完整ESP-IDF安装备份；环境版本和依赖要求继续由README记录。

备份内可能包含本地凭据，按私有文件保存；不会自动上传。当前副本在同一个D盘，不能当作异盘容灾副本。整理后另建完整快照与SHA文件，见项目backups目录。

## 当前入口

- [架构](../../architecture/README.md)：模块责任、线程、数据流程、官方边界和维护顺序。
- [设计](../../design/README.md)：18款稳定编号、三批规范、设置与原型状态。
- [素材追溯](../../assets/CATALOG.md)：每款皮肤到原始图、运行资源及转换脚本。
- [当前验收](../CURRENT-STATUS.md)：软件检查、用户整体反馈与0.3.9手势待验分开记录。
- [冻结固件](../../history/releases/firmware/releases/v0.3.9/release.json)：纯应用包、完整镜像和各文件校验值。

catalog.json记录24张原图的原名/尺寸/SHA/历史说明关联、18款皮肤、21份运行编码、源图和编码器、字体来源，以及相同哈希的重复副本。未记录的精确提示词不作补写；当前未用的旧资源继续保存。原始生成缓存24张已逐字节核对，旧迁移清单1,721条媒体资源全部保留。

## 版本管理

核心原图、图集、历史设计/提示词、旧编码与历史报告允许Git跟踪；docs/history/releases和当前批量软件帧仍忽略，但均纳入私有完整快照。backups、dist与本地敏感配置继续忽略。当前收尾未代替用户提交或推送Git，工作区变更由前后完整快照保存。

## 复现检查

```powershell
python tools/assets/archive_images.py --source docs/assets/generated-originals
python tools/assets/catalog_assets.py
python tools/checks/check_docs.py
python tools/assets/catalog_assets.py --check
python tools/checks/check_platform.py
python tools/maintenance/backup_project.py --label v0.3.9-closeout
```

单独验证ZIP：python tools/maintenance/backup_project.py --verify backups/实际文件名.zip。标准编译/合并/COM6烧录仍见根README，只有一个build目录。

## 验收边界

0.3.9圆形设置按钮与右滑返回的软件交互、像素居中已通过；用户尚未反馈这版触控真机结果。第二、三批HT页继续属于主题延展候选，未因收尾归档自动标记为已批准。

归档工具使用Python 3.11及以上；素材尺寸读取使用已安装的Pillow。复核报告 [closeout-integrity-0.3.9.json](closeout-integrity-0.3.9.json) 对照整理前ZIP确认8,433个受保护文件无变化、无缺失。没有对素材做删重或重新编码，目录移动数量为0。

## 最终检查结果

标准ESP-IDF build、merge-bin及完整镜像组件比对通过；复核重建的纯应用包容量检查通过。冻结交付仍使用dist/releases/v0.3.9原交付文件；本次复核重建产物位于build，因构建时间等元数据不同，校验值与原交付可能不同，源码和媒体对照保持不变。此次收尾无需重新烧录。

24张生成原图、18款皮肤追溯、21份编码、全部媒体覆盖、10张当前原型引用、1,721条旧迁移资源、7,075份图片索引、Markdown链接和官方82文件比对均通过。素材与运行代码无删除或更改，生产行为没有变动。

整理后的完整ZIP使用v0.3.9-closeout标签，实际文件名、大小、SHA-256和逐项验证结果由backups下同名.json/.sha256记录。所有核心资料均纳入该快照，批量可生成帧虽不进入Git，也不会遗漏备份。
