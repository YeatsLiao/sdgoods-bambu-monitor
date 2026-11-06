# 素材保存与更新规则

当前原型只由 docs/design 各批 README 和 manifest.json 指定，不能从 generated-originals 或软件预览目录自行选择替代图。

原始生成图保存在 generated-originals，保留文件名、原分辨率与 SHA-256；图集源在 atlases，固件编码当前副本在 runtime-encoded，字体源/许可/当前生成物在 fonts。当前副本应与 main/assets、main/fonts 对应；旧哈希变体归 history/assets，不与当前副本混排。

tools/archive_images.py 会校验和归档原图、同步当前资源、先保留旧版再更新当前副本，并将冻结预览放在 docs/history/releases/<版本标签>。image-manifest.json 是机器索引，不是视觉验收记录。

2026-11-05 重试后，新版 AMS 和第四页详情图片已成功返回，统一归入 design/batch-01，原版保存在 history/design/batch-01。猫咪探头另由内置 ImageGen 根据已确认 AMS 图生成透明源素材，保存为 atlases/cat-peek-v1.png，编码为 runtime-encoded/cat_peek.c（96×64 RGB565+alpha，18,432 字节）。转换只做裁出透明边界、等比缩放和编码，不重画角色。

第二批新增透明场景/头像图集和纸纹后，第三批再新增六大场景与横幅初稿/修订图后，全部 24 张原始生成图保留在 generated-originals；提示词记录见 [本轮素材记录](../history/design/batch-01/runtime-assets-20261105.md)。软件渲染单独放 verification，不能代替当前原型。

当前逐项追溯见 [素材导航](CATALOG.md) 与 [机器清单](catalog.json)。所有原始生成图、旧稿、旧编码和重复副本保留，运行使用状态单独记录。核心素材与历史原型/提示词可被Git跟踪；大批量可生成预览及冻结帧由完整私有备份覆盖。完整备份包含本地配置，保存在backups，需按私有文件保管；本地同盘副本不等于异盘备份。
