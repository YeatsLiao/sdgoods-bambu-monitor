# 架构与官方层边界

## 依赖方向

`main/main.c` 装配应用 → `main/apps` 界面与皮肤 → `main/bambu` 偏好、连接与数据模型。
应用通过官方公开 API 调用 `components/sdgoods_board` 和 `components/sdgoods_launcher`；官方组件不包含任何拓竹业务头文件。

| 目录 | 责任 | 修改约定 |
|---|---|---|
| components/sdgoods_board | 官方显示、触摸、电源、系统语言、控制中心外壳、截图 | 原样保留 |
| components/sdgoods_launcher | 官方控制中心、设备模式检测、退出启动器、应用槽接口 | 原样保留 |
| platform | 官方分区表与本地参考引导层 | 原样保留，不打进应用包 |
| components/jpegenc | 官方截图依赖 | 原样保留 |
| components/lvgl | 本地离线 LVGL 8.3 依赖及已有上游平台补丁 | 第三方依赖，不放应用逻辑 |
| components/dns_server | 局域网配网 DNS 组件 | 通用依赖 |
| main/main.c | 初始化顺序、导航与轮询回调注册 | 唯一装配点 |
| main/bambu/bambu_state、bambu_report | 线程安全状态快照、真实增量 JSON 解析与 AMS 模型 | 与 UI 无关 |
| main/bambu/bambu_mqtt | 局域网 Wi-Fi / MQTT-TLS 连接及分片重组 | 后台服务调用 |
| main/bambu/wifi_prov、prov_form、pairing_probe | 手机本地配网、表单校验、真实配对验证、试运行与回滚 | 后台服务调用 |
| main/bambu/monitor_service | 唯一网络生命周期任务、UI 请求队列、状态快照 | 禁止调用 LVGL |
| main/bambu/monitor_preferences | 本应用皮肤、熄屏、亮屏提醒偏好 | 独立命名空间 bmb_prefs |
| main/apps/ui_monitor | 四组页面、设置、翻页、熄屏唤醒 | 仅 LVGL 主线程 |
| main/apps/monitor_skin | 稳定皮肤 ID、配色、插画选择 | 共享页面与业务数据 |
| main/assets、main/fonts | 本应用插画与字体生成物 | 不改官方字体 |
| tools/preview | 本机真实 LVGL 渲染和交互检查 | 示例数据只存在于测试程序 |

## 与官方服务共存

- 不调用启动器宿主的 boot_check，不扫描或修改其他应用的 manifest，不修改分区。
- 控制中心始终由官方组件提供；`sdgoods_app_shell_bind` 接入顶部下滑，官方 Exit 返回启动器。
- 基于当前电台配置：先初始化显示/触摸，再初始化其他服务；LVGL 使用自定义分配器，通用分配可使用 PSRAM。保持官方显示缓冲、QSPI 分片及驱动参数不变。
- 网络由拓竹服务独占初始化/拆卸；main 不再并行调用 `sdgoods_wifi_init`。Wi-Fi 驱动设置为 RAM 存储，避免覆盖共享系统的 Wi-Fi 凭据记录。
- 网络配置独立存入 `bmb_net`；应用偏好存 `bmb_prefs`。应用业务层不擦除整个 NVS。
- 亮度由官方控制中心管理。应用自动熄屏只临时设置背光 0，恢复时读取官方用户亮度，不保存临时熄屏值。
- 横向翻页只接收监控页中部起手的水平滑动；顶部和底部系统手势区域留给官方外壳。设置点按复用官方位移守卫。

## 构建与交付

项目统一使用标准 ESP-IDF 流程：`idf.py build` → `idf.py merge-bin`，构建目录为 `build/`，根目录 `sdkconfig` 保存本地配置，`sdkconfig.defaults` 默认包含全部 18 款皮肤。芯片固定为 ESP32-S3；皮肤在设置中切换，无需分批编译。历史分批构建和容量报告仅作为开发记录保留。

本地烧录使用 `idf.py -p COM6 flash`；也可将 `build/merged-binary.bin` 复制为 `build/sdgoods-bambu-monitor-full.bin` 后，用 esptool 从 `0x0` 写入。完整镜像按 IDF 构建清单合并，包含 NVS 区域的 FF 填充，会重置该范围内设置。原版 `platform/prebuilt` 仍原样保留。

`python tools/pack_app.py --name BambuMonitor_app.bin` 从标准 `build/` 导出纯应用包，默认遵守官方 2.9 MiB 上限，用于官方平台管理应用槽。完整镜像与纯应用包的用途见根目录 README。

## 当前实现与目标的差距

v0.3.3 候选已接入型号查询与动态文字、统一四组分页、外置/HT 合并监控、透明猫咪探头和按实测字宽适配。生产代码渲染图保存在 verification/preview；此前 15:10 的真机照片仍是反馈依据，不能把软件检查替代真机视觉验收。第一批按 [当前四页规范](../design/batch-01/SPEC.md) 对照；第二批0.3.4已重做最初原图还原候选，第三批0.3.5已恢复大场景、角色横幅、AMS和详情候选。第二/三批分别在 ui_monitor_batch2.inc、ui_monitor_batch3.inc 绘制，继续共用数据/分页与字宽规则。

当前原型、素材、验证、历史目录分别负责目标、原始/运行资源、证据、旧记录。历史构建缓存在 build/history，本机渲染缓存为 build/preview；项目仍是标准单工程，官方层不改。

## 第三组素材缓存

monitor_art.c/h负责当前单张调色板游程图展开，最大61,440字节，LVGL线程独占；源数据在flash，切换前使旧图缓存失效。编码脚本、原始PNG、量化策略与验证结果见第三组报告。旧通用占位绘制已移除，数据/分页仍共享。
