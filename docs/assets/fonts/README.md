# 字体入口

- [来源与复现说明](source/README.md)：Noto Sans SC 源、数字字重与生成流程。
- [源文件](source/)：保留已有TTF/OTF与说明。
- [生成物](generated/)：与 main/fonts 一致的C子集及 [OFL许可](generated/OFL.txt)。

生成工具为 tools/assets/generate_fonts.py，转换依赖 lv_font_conv；具体命令见来源说明。只在文字或字号变化时重新生成。本轮收尾未改字体，也未把系统字体路径当作唯一归档来源。
