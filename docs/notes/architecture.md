# Architecture

# 现行架构 · 0.3.9

## 分层与线程

main/main.c 是唯一装配点，负责显示/触摸初始化顺序以及服务和UI轮询接入。

官方平台：components/sdgoods_board、sdgoods_launcher、jpegenc 以及 platform 原样保留，通过官方公开API调用。components/lvgl 是本地离线依赖和既有平台补丁；通用DNS组件不包含打印业务。

数据层：main/bambu/bambu_report 解析真实JSON增量与身份，bambu_state 发布带锁快照，bambu_pages 按实际AMS/HT拓扑生成页面。解析、分页不依赖LVGL。身份应答不冒充配对证明或刷新遥测时效，未知值保持未知。

连接与配置：bambu_mqtt 负责Wi-Fi/MQTT-TLS、订阅和分片；wifi_prov/prov_form/pairing_probe 负责手机配网、输入校验及真实配对验证；app_config 管理试运行、确认及回滚。monitor_service 是唯一网络生命周期任务，通过队列接收UI请求，不调用LVGL。

应用偏好：monitor_preferences 管理皮肤、熄屏和提醒，属于应用设置；目前保留在 main/bambu 目录，不因本轮资料整理搬动源码。

UI层：main/apps/ui_monitor 在LVGL线程读取服务/数据快照，处理绘制、翻页、设置与唤醒。ui_monitor_design.inc、batch2.inc、batch3.inc 分别绘制三批；layout.inc 管理共享几何，settings.inc 管理设置界面。monitor_skin 管理稳定ID与配色，monitor_art 管理当前单图解码缓存。

资源层：main/assets、main/fonts 为固件直接输入；docs/assets 保存图片源、源字体与许可；运行编码以main/assets、main/fonts为准，旧副本归入历史ZIP。tools 下的编码、字体、预览、配对解析测试和打包脚本用于生成或验证，不进入业务UI。

## 数据流

打印机报告经 MQTT 分片重组进入解析器，合并为带锁状态快照；UI每500ms读取快照，由分页模型选择逻辑页面，再按皮肤绘制。UI不直接操控网络生命周期；配网启停通过服务请求队列完成。

页面顺序为首页、普通AMS、外置＋HT、详情。四类逻辑页面不等于固定四个物理页，多盒按拓扑展开。所有型号和实时数值由程序绘制，不写死在插画里。

## 不变量

- 官方82文件与radio逐字节比对；官方控制中心提供亮度、声音和退出。
- 不擦除共享NVS；网络配置使用 bmb_net，偏好使用 bmb_prefs。Wi-Fi使用RAM存储。
- LVGL绘制及图像缓存仅由UI线程拥有；后台网络服务不持有界面对象。
- 场景压缩图仅缓存当前一张，解码上限61,440字节，切换前失效旧缓存。
- 设置中部右滑复用同一个返回入口；配网取消由后台完成。拖动、唤醒首触及系统手势隔离，详见设置规范。
- 三批共用同一数据模型，原始设计、软件预览和真机反馈分别管理。

## 构建与交付

标准单工程 build 目录，ESP32-S3，ESP-IDF 5.5.x。本机5.5.5；命令以根README为准。dist 中为官方纯应用包，build/sdgoods-bambu-monitor-full.bin 为完整本地烧录镜像，两种用途分别校验。

本轮收尾仅调整文档、目录索引、归档工具和保存策略，不拆分当前稳定运行代码。0.3.9触控仍待用户真机确认；第二、三批HT页仍缺单独批准原型。


# MQTT 型号和耗材盒名称规则

核查日期：2026-11-05。结论：可以从 MQTT 应答识别受支持的打印机型号；不能保证每台机器都返回型号，更不能把型号等同于用户自定义设备昵称。

## 官方来源

拓竹官方 [BambuStudio DeviceManager.cpp](https://github.com/bambulab/BambuStudio/blob/master/src/slic3r/GUI/DeviceManager.cpp) 的 get_version 分支读取 info.module 中的 name/product_name/hw_ver；print 分支读取 printer_type。官方机型配置确认 [H2D 的 O1D](https://github.com/bambulab/BambuStudio/blob/master/resources/profiles/BBL/machine/Bambu%20Lab%20H2D.json)、[X1 Carbon 的 BL-P001](https://github.com/bambulab/BambuStudio/blob/master/resources/profiles/BBL/machine/Bambu%20Lab%20X1%20Carbon.json)、[P1S 的 C12](https://github.com/bambulab/BambuStudio/blob/master/resources/profiles/BBL/machine/Bambu%20Lab%20P1S.json)。这说明可做已识别代码到 X1C/H2D/P1S 的显示映射；不是所有固件必有这些字段的保证。

## 接入约定

订阅 device/<serial>/report 成功后，向 device/<serial>/request 发送查询：

```json
{"info":{"sequence_id":"1","command":"get_version"}}
```

优先解析 print.printer_type；备用只接受 info/get_version 中主机 ota 模块的可信 product_name 或可识别 hw_ver。AMS 模块同样可能有产品信息，不能覆盖打印机型号。未知/失败应答不覆盖已识别值，重新连接另一台机器时清空旧身份。身份消息不能冒充实时打印报告或配对成功。

建议数据字段 printer_model 独立于打印状态；缺失显示 Bambu/--。自定义昵称如未获得可靠字段就不显示，不猜测。需要用实机查询结果验证本机返回哪一类字段；当前截图没有包含型号应答，不能声称已经识别出用户机器是 H2D 或 X1C。

AMS 1 是盒子类型与序号。普通盒标题可显示 AMS / AMS Lite / AMS 2 Pro + 逻辑序号；HT 显示 AMS HT + 逻辑序号，不露出内部 128/129。打印机型号统一在首页/详情对应的动态文字位显示，两者并存。

## 当前代码状态

v0.3.8 统一规范化大小写、空格、连字符和下划线后作白名单精确匹配。支持15个名称：X1/X1C/X1E、P1P/P1S/P2S、A1/A1 mini/A2L、H2D/H2D Pro/H2S/H2C/H3C、X2D。名称清单依据官方 [BBL 索引](https://github.com/bambulab/BambuStudio/blob/master/resources/profiles/BBL.json)。H3C目前只接受官方名称，不猜尚未核实的代码。

已核实的官方 machine_model 代码：BL-P001→X1C、BL-P002→X1、C13→X1E、C11→P1P、C12→P1S、N1→A1 mini、N2S→A1；新增 [N7→P2S](https://github.com/bambulab/BambuStudio/blob/master/resources/profiles/BBL/machine/Bambu%20Lab%20P2S.json)、[N6→X2D](https://github.com/bambulab/BambuStudio/blob/master/resources/profiles/BBL/machine/Bambu%20Lab%20X2D.json)、[N9→A2L](https://github.com/bambulab/BambuStudio/blob/master/resources/profiles/BBL/machine/Bambu%20Lab%20A2L.json)、[O1S→H2S](https://github.com/bambulab/BambuStudio/blob/master/resources/profiles/BBL/machine/Bambu%20Lab%20H2S.json)、[O1C2→H2C](https://github.com/bambulab/BambuStudio/blob/master/resources/profiles/BBL/machine/Bambu%20Lab%20H2C.json)、[O1E→H2D Pro](https://github.com/bambulab/BambuStudio/blob/master/resources/profiles/BBL/machine/Bambu%20Lab%20H2D%20Pro.json)，继续 O1D→H2D。每个代码都通过实际 C 解析测试；这不代表所有机型都已做真机连接测试。

v0.3.7 补齐 X1C OTA 的 `Bambu Lab X1-Carbon` 名称。此前仅接受 `Bambu Lab X1 Carbon`，有连字符的实际应答会被白名单拒绝，回退 BAMBU。名称实例来自设备用户直接上传的 [X1C get_version 诊断记录](https://github.com/greghesp/ha-bambulab/issues/1465)，不是本用户 COM6 的捕获；不根据 AP04/AP05 等主板版本猜型号。

get_version 查询由后台服务在订阅成功后发起（轮询周期2秒），缺少型号时每6秒重试，最多3次；取得型号立即停止重试，重连后重新查询。串口记录 `Printer model query N/3` 和 `Printer model identified: X1C`。仅身份应答不再误报成无有效遥测的忽略警告；仍不作为配对证明或刷新遥测时间。首页和详情页通过现有500ms快照刷新读取型号，无需改皮肤。

v0.3.3 已移植 printer_model、订阅后 get_version 查询及身份独立提交。解析测试覆盖型号代码/名称、忽略 AMS 模块、未知/失败不覆盖、重新连接清空、身份不刷新遥测时间且不作为配对证明。用户设备的实际返回字段仍需 COM6 真机核对。

HT 烘干状态与分钟数按官方 [DevFilaSystem.cpp](https://github.com/bambulab/BambuStudio/blob/master/src/slic3r/GUI/DeviceCore/DevFilaSystem.cpp) 的 info 位 4–7、dry_time 解析；枚举来自 [DevFilaSystem.h](https://github.com/bambulab/BambuStudio/blob/master/src/slic3r/GUI/DeviceCore/DevFilaSystem.h)：0 待机、1 检查、2 烘干、3 冷却、4 停止、5/6 异常，未支持值显示 --。dry_time 单位为分钟。只显示监控信息，不发烘干控制指令。


# 来源与再分发说明

- 官方板级支持、启动器、平台文件和 jpegenc：取自本机 `D:/2.Project/SDGOODS/sdgoods-radio`。保留原作者声明、Required Notice、LICENSE、LICENSING.md、NOTICE、TRADEMARK.md，遵守各文件原许可。
- 官方层内容校验清单：`docs/notes/platform-snapshot.json`，由 `tools/checks/check_platform.py` 对本机参考工程逐文件 SHA-256 比较生成。
- LVGL：8.3 系列本地离线依赖，来源为电台项目已安装的 `managed_components/lvgl__lvgl`，MIT 许可及原版权文件保留。已有平台补丁由 `main/patches` 幂等校验与应用。
- 拓竹协议、状态、配网、试运行与测试：取自本机 `D:/2.Project/ai-passport-bambu-monitor`，该项目 README 标明 MIT；本工程新增 SPDX 注释不删除原来源说明。适配改动集中在 `main/bambu`，不修改原工程。
- 字体：Noto Sans SC，SIL OFL 1.1。应用字体单独生成在 `main/fonts`，不覆盖官方字体。完整 OFL 见 `components/sdgoods_board/fonts/OFL.txt`。字体源与转换说明见 `tools/fonts/README.md`。
- 像素插画：内置 ImageGen 在本聊天生成，原图保留为 `main/assets/batch-01-art-source.png`，转换出的 RGB565 数据只包含插画，不含监控数值。概念图在 `docs/design`。
- 第二批插画原图为 `main/assets/batch-02-art-source.png`，以已确认的第二批首页为视觉参考，通过内置 ImageGen 生成掌机、机器人、竹林、太空四个独立图块；同样只含插画。
- 第三批插画原图为 `main/assets/batch-03-art-source.png`，以已确认第三批首页为参考，生成农场、赛博、海底、雪山、沙漠、魔法六个图块。所有三批图集另有 docs/assets/images 原字节归档，所有生成原图见 docs/assets/images。
- 原始概念图、字体源、测试截图、构建缓存均不直接打进应用 bin。运行时只有 CMake 列明的字体、插画编码与应用代码进入固件。

代码转换工具默认参考本机现成的 lv_font_conv；它是开发工具依赖，不是固件依赖。
