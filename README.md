# SDGoods 拓竹监视器

基于 aiPassport 拓竹监视器功能，为 SDGoods 圆屏设备设计的官方启动器兼容应用。

一个 ESP-IDF 工程，包含 18 款候选皮肤。当前 v0.3.5 包含三批18款原图还原候选，等待真机验收；页面顺序为：首页 → AMS → 外置 + HT → 打印详情。

- **[第一批当前原型与规范](docs/design/batch-01/README.md)**：后续还原的唯一当前入口，包含四张参考图、猫咪与型号要求及旧图差异。
- [第三组本轮还原与容量记录](docs/verification/third-batch-0.3.5.md)
- [第二组本轮还原记录](docs/verification/second-batch-0.3.4.md)
- [本轮修复与真机待验项](docs/verification/CURRENT-STATUS.md)
- [架构与 MQTT 型号读取方案](docs/architecture/README.md)
- [当前实施顺序](docs/architecture/implementation-plan.md)
- [资料分类总入口](docs/README.md) · [素材](docs/assets/README.md) · [历史记录](docs/history/README.md)

主界面中部左右滑动翻页，详情页齿轮打开设置，顶部下滑使用官方控制中心。设置页短按电源键逐级返回；监控根页沿用官方电源键/返回启动器行为。首次未配置自动引导手机连接 Bambu-XXXX，在 192.168.4.1 填写 Wi-Fi 与打印机信息。

## 选择皮肤

- 在打印概览主屏中部静止长按约 **0.7 秒**，进入一屏一皮肤的实时预览；也可从「设置 → 外观与语言 → 选择皮肤」进入。
- 左右滑动切换 01–18 款，手指移动过程中即可看到当前数据在对应皮肤中的样子，浏览不保存。
- 右下角 **勾选** 保存并返回；左下角 **叉号** 或短按电源键取消，保留原皮肤和原页面。
- 操作条停留 3 秒后隐藏，展示完整皮肤；再点一下仅唤出操作条，下一次点勾选才保存。顶部下滑继续使用官方控制中心，熄屏后的首次触摸只唤醒。
- 语言在「外观与语言 → 界面语言」中单独切换。

[单皮肤预览](docs/verification/preview/skin-picker-overview.png) · [设置页](docs/verification/preview/settings-overview.png)

## 编译、构建与烧录

需要 **ESP-IDF 5.5.x**（本机使用 5.5.5），目标芯片 **ESP32-S3**，串口 **COM6**。在已激活 ESP-IDF 环境的终端中，进入本项目目录：

```powershell
cd D:\2.Project\SDGOODS\sdgoods-bambu-monitor
```

与 aiPassport 项目相同，使用标准 `build` 目录：

```powershell
idf.py build
idf.py merge-bin
copy build\merged-binary.bin build\sdgoods-bambu-monitor-full.bin
idf.py -p COM6 flash

# 或直接烧录合并后的完整固件（烧录后手机配网即可）：
# python -m esptool --chip esp32s3 -p COM6 --baud 460800 write_flash 0x0 build\sdgoods-bambu-monitor-full.bin
```

以上构建包含全部 18 款皮肤，无需选择批次或指定构建目录。`idf.py -p COM6 flash` 直接烧录当前构建；最后一行是烧录完整合并固件的另一种方式，二者选一种即可。

查看串口日志：

```powershell
idf.py -p COM6 monitor
```

按 `Ctrl+]` 退出串口监视器。首次启动用手机连接 `Bambu-XXXX`，打开 `http://192.168.4.1` 配置 Wi-Fi 与打印机。

完整合并固件从 `0x0` 写入，会替换当前 factory 应用，并重置覆盖范围内的 NVS 设置与启动选择；这是本地开发烧录方式。

## 双击串口截图

设备通过 USB 连接后，停留在要截图的页面，双击项目根目录的 **`串口截图.cmd`**。默认使用 **COM6**，自动接收一张截图，按时间命名保存到 `docs/verification/device/screenshots/`，随后打开图片。设备发送 JPEG 时保存为 `.jpg`，原始像素回退时保存为 `.png`。

截图前先退出 `idf.py monitor` 或其他占用 COM6 的串口工具（`Ctrl+]` 退出 IDF 监视器）。失败时窗口会保留提示；成功后可直接将保存的图片发到聊天中。

## 导出官方应用包

完成 `idf.py build` 后执行：

```powershell
python tools/pack_app.py --name BambuMonitor_app.bin
```

产物为 `dist/BambuMonitor_app.bin`，用于官方平台安装及切换应用。它是纯应用镜像，不能作为完整固件写到 `0x0`。设计资料和历史容量报告中的“第一批、第二批、第三批”仅用于记录开发过程。
