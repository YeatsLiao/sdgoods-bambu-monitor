# 素材生成与保存约定

全部本项目概念图与插画由当前聊天的内置 ImageGen 生成。生成目录的原始文件统一保存在 generated-originals；使用有意义文件名的设计图在 docs/design，插画源图在 atlases。原图保留生成时的完整分辨率与字节，不做覆盖或破坏性裁切。

## 13 张生成原图的构成

- 首页概念图 3 张：第一批 01–06、第二批 07–12、第三批 13–18。
- AMS 页面概念图 3 张：对应三个批次。
- 打印详情页面概念图 3 张：对应三个批次。
- 全部皮肤共用的设置四屏图 1 张。
- 运行素材图集 3 张：第一批三图块、第二批四图块、第三批六图块。

编号、用途与每张原图路径见 README.md，完整文件大小和 SHA-256 见 image-manifest.json。当前归档范围是本聊天生成目录中的全部原图，不包含其他聊天的无关生成内容。

字体源文件与上游说明另存 fonts/source，三个应用子集字体及完整 OFL 另存 fonts/generated；与 RGB565 编码文件一起列入清单的 resources 节，保留字节数和 SHA-256。

## 图集到固件资源

第一批 3×1：像素工坊、猫咪监工、夜班打印室。
第二批 2×2：掌机像素、机器人助手、竹林工坊、太空货舱。
第三批 3×2：农场造物、赛博街区、海底实验室、雪山木屋、沙漠基地、魔法工坊。

生成目标为纯插画，不含进度、温度或状态文字；第三批以已认可首页概念图作为视觉参考。风格为清楚的像素小场景，分别保留谷仓/小鸡、赛博小猫、潜水实验室、雪山企鹅、沙漠机器人、巫师猫的主题特征。

`tools/assets/encode_art.cjs <pngjs模块路径> <1|2|3>` 按等分图块采样，输出 144×96 RGB565 C 资源；高字节优先，对应官方 LV_COLOR_16_SWAP 配置。编码副本另存 runtime-encoded，不作为生成原图替代品。现阶段有缩放和插画比例差异，原图便于后续重新处理，不需要重新找图。

## 预览与历史

verification/preview 为当前 LVGL 运行预览，每次渲染可能更新。verification/releases/batch-02-v0.2.0 保存本次第三批开发前的可用预览快照；后续批次用独立标签另存，不覆盖这个快照。PNG 用于查看，PPM 是软件渲染器输出的原始帧。

运行 `tools/assets/archive_images.py --source <本聊天生成目录> --snapshot <版本标签>` 会复制与校验原图，保存图集/编码副本，冻结预览并刷新总索引。同名内容不同的文件另存哈希后缀，旧文件保留。已归档的初始源文件不从用户全局生成目录删除。

## 用户指定的烧录参考

2026-11-05 用户提供的 aiPassport 构建/merge-bin/整包烧录截图已原样保存为 references/ai-passport-full-flash-reference.png，属于用户参考图，不计入 13 张 AI 生成原图。当前完整固件命令以根 README 为准。

## 第一批还原素材 v2（2026-11-05）

exec-2a4dafe9-5f36-4888-8931-37f9e0973e41.png 是参考已批准首页概念生成的三格透明插画，依次为像素打印机、猫咪与打印机、夜班打印室。保留生成原图；main/assets/batch-01-scenes-v2.png 为源字节副本。tools/assets/encode_scenes.cjs 仅按三格读取透明边界、等比例采样并编码为 RGB565+alpha，输出 scene_printer/cat/night.c。场景与文本数据完全分离，原来的图集和编码副本仍保留。

数字字体增加 Noto Sans SC 的 900 字重，来源、转换及 OFL 许可见字体素材目录。第一批还原的当前运行预览在 verification/preview；旧版预览仍存于 releases/batch-03-v0.3.0，真机原始截图仍在 verification/device/screenshots。
