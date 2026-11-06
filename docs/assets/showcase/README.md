# README界面预览

本目录图片随Git保存，供项目首页直接显示；均来自0.3.9实际LVGL绘制代码的软件输出，使用示例数据，不是实机截图或设计原型。

- first-six-skins.png、second-six-skins.png、third-six-skins.png：01–18款首页，每组3列2行。
- settings-six-pages.png：应用设置、外观与语言、熄屏设置、网络与打印机、配网、选肤，共3列2行。
- skins-01-06-all-pages.png、skins-07-12-all-pages.png、skins-13-18-all-pages.png：三组完整四类页面。

源目录为docs/verification/preview。tools/export_showcase.py按原像素拼接，保留源文件；替换已有图片时先按哈希保存旧副本到docs/history/releases/showcase-revisions。manifest.json记录每张图片的来源、尺寸与SHA-256。

先用真实LVGL渲染工具更新源帧，再运行 python tools/export_showcase.py。随后同步素材索引与追溯表。原始生成图与原型不受影响。
