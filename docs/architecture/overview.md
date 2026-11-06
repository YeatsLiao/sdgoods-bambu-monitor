# 现行架构 · 0.3.9

## 分层与线程

main/main.c 是唯一装配点，负责显示/触摸初始化顺序以及服务和UI轮询接入。

官方平台：components/sdgoods_board、sdgoods_launcher、jpegenc 以及 platform 原样保留，通过官方公开API调用。components/lvgl 是本地离线依赖和既有平台补丁；通用DNS组件不包含打印业务。

数据层：main/bambu/bambu_report 解析真实JSON增量与身份，bambu_state 发布带锁快照，bambu_pages 按实际AMS/HT拓扑生成页面。解析、分页不依赖LVGL。身份应答不冒充配对证明或刷新遥测时效，未知值保持未知。

连接与配置：bambu_mqtt 负责Wi-Fi/MQTT-TLS、订阅和分片；wifi_prov/prov_form/pairing_probe 负责手机配网、输入校验及真实配对验证；app_config 管理试运行、确认及回滚。monitor_service 是唯一网络生命周期任务，通过队列接收UI请求，不调用LVGL。

应用偏好：monitor_preferences 管理皮肤、熄屏和提醒，属于应用设置；目前保留在 main/bambu 目录，不因本轮资料整理搬动源码。

UI层：main/apps/ui_monitor 在LVGL线程读取服务/数据快照，处理绘制、翻页、设置与唤醒。ui_monitor_design.inc、batch2.inc、batch3.inc 分别绘制三批；layout.inc 管理共享几何，settings.inc 管理设置界面。monitor_skin 管理稳定ID与配色，monitor_art 管理当前单图解码缓存。

资源层：main/assets、main/fonts 为固件直接输入；docs/assets 保存原图、图集、许可及可校验镜像。tools 下的编码、字体、预览、配对解析测试和打包脚本用于生成或验证，不进入业务UI。

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
