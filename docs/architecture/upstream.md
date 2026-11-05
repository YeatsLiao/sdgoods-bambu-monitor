# 来源与再分发说明

- 官方板级支持、启动器、平台文件和 jpegenc：取自本机 `D:/2.Project/SDGOODS/sdgoods-radio`。保留原作者声明、Required Notice、LICENSE、LICENSING.md、NOTICE、TRADEMARK.md，遵守各文件原许可。
- 官方层内容校验清单：`docs/verification/platform-snapshot.json`，由 `tools/check_platform.py` 对本机参考工程逐文件 SHA-256 比较生成。
- LVGL：8.3 系列本地离线依赖，来源为电台项目已安装的 `managed_components/lvgl__lvgl`，MIT 许可及原版权文件保留。已有平台补丁由 `main/patches` 幂等校验与应用。
- 拓竹协议、状态、配网、试运行与测试：取自本机 `D:/2.Project/ai-passport-bambu-monitor`，该项目 README 标明 MIT；本工程新增 SPDX 注释不删除原来源说明。适配改动集中在 `main/bambu`，不修改原工程。
- 字体：Noto Sans SC，SIL OFL 1.1。应用字体单独生成在 `main/fonts`，不覆盖官方字体。完整 OFL 见 `components/sdgoods_board/fonts/OFL.txt`。字体源与转换说明见 `tools/fonts/README.md`。
- 像素插画：内置 ImageGen 在本聊天生成，原图保留为 `main/assets/batch-01-art-source.png`，转换出的 RGB565 数据只包含插画，不含监控数值。概念图在 `docs/design`。
- 第二批插画原图为 `main/assets/batch-02-art-source.png`，以已确认的第二批首页为视觉参考，通过内置 ImageGen 生成掌机、机器人、竹林、太空四个独立图块；同样只含插画。
- 第三批插画原图为 `main/assets/batch-03-art-source.png`，以已确认第三批首页为参考，生成农场、赛博、海底、雪山、沙漠、魔法六个图块。所有三批图集另有 docs/assets/atlases 原字节归档，所有生成原图见 docs/assets/generated-originals。
- 原始概念图、字体源、测试截图、构建缓存均不直接打进应用 bin。运行时只有 CMake 列明的字体、插画编码与应用代码进入固件。

代码转换工具默认参考本机现成的 lv_font_conv；它是开发工具依赖，不是固件依赖。
