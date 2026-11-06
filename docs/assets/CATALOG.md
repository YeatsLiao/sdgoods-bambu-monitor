# 素材导航与追溯

当前保存24张生成原图、11张图集/独立素材源、21份运行资源C编码，以及字体源和许可。完整资源清点由 [catalog.json](catalog.json) 和 [image-manifest.json](image-manifest.json) 提供。

## 查找顺序

1. 从 [设计入口](../design/README.md) 找到皮肤编号和所属批次。
2. 在 catalog.json 的 skins 中查看原型、规范、布局代码及运行资源。
3. 在 runtimeAssets 中查看对应的源图、原始生成图、转换脚本及当前使用状态。
4. 在 generatedOriginals 中查看原始文件名、原尺寸、SHA-256、相同字节副本和历史说明记录。

程序直接引用 main/assets 和 main/fonts；runtime-encoded、fonts/generated 是经哈希核对的归档镜像。原始生成图保留原名与字节，图集副本通过哈希关联原始来源。

## 分类

[showcase](showcase/README.md) 是随Git保存的项目首页精选软件预览，与被忽略的批量渲染目录分开；图片明确标注示例数据。

- [generated-originals](generated-originals/)：24张生成原图，包括旧稿和未采用稿。
- [atlases](atlases/)：用于裁切、缩放与编码的源PNG。
- [runtime-encoded](runtime-encoded/)：当前源码目录中的编码镜像；其中未被当前UI引用的旧资源明确标记为保留历史资源。
- [字体](fonts/README.md)：源字体、生成物、转换说明和许可。
- [真机反馈](../verification/device/)：用户实机截图及原始反馈。
- [历史素材](../history/assets/) 与 [历史设计说明](../history/design/)：被替换版本和已有生成提示词。

竹林小装饰当前由 ui_monitor_batch2.inc 中的 b2_bamboo 几何绘制；原生成竹叶图和旧编码继续保存。第二、三批外置/HT页面仍标记为主题延展候选，原型状态不因归档自动升级。

mentionRecords 只表示某文档提及该文件，不保证其中有完整提示词。未记录的原始提示词不追写成推测内容。SHA相同的文件列为重复副本，全部保留。

## 更新和检查

先运行 tools/archive_images.py 同步原图和资源镜像，再运行 python tools/catalog_assets.py 更新追溯表。检查命令：python tools/catalog_assets.py --check。完整保存策略见 [GENERATION.md](GENERATION.md)。
