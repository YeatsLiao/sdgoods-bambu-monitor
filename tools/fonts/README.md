# 本应用字体素材

字体只编入 main/fonts，不修改官方字体。完整许可见 main/fonts/OFL.txt。

NotoSansSC-Regular.ttf 是此前已归档的常规字体源；NotoSansSC-Bold.otf 和 NotoSansSC-Variable.ttf 取自本机已安装的 Noto Sans SC。可变字体内部许可声明为 SIL Open Font License 1.1，其 wght 轴范围 100–900。源文件及生成物均由 tools/archive_images.py 保存到 docs/assets/fonts，便于复现。

NotoSansSC-Black-Numbers.ttf 使用 FontTools 对可变源先按字符 0123456789%/-°C. 做子集，再用 instantiateVariableFont 固定 wght=900，生成静态数字字体；运行时无需可变字体引擎。固件只链接 lv_font_conv 生成的 C 字形子集，不包含完整 OTF/TTF。

重新生成应用字体：

```powershell
python tools/generate_fonts.py --converter D:/2.Project/SDGOODS/sdgoods-radio/node_modules/lv_font_conv/lv_font_conv.js
```

脚本只扫描监控界面 .c 与 .inc 的实际字符串（不包含注释和未使用模板），生成常规 16/20、粗体 18/22/26 和数字 44/64/88/128。修改界面文字后应重新生成。
