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

v0.3.3 已移植 printer_model、订阅后 get_version 查询及身份独立提交。解析测试覆盖型号代码/名称、忽略 AMS 模块、未知/失败不覆盖、重新连接清空、身份不刷新遥测时间且不作为配对证明。用户设备的实际返回字段仍需 COM6 真机核对。

HT 烘干状态与分钟数按官方 [DevFilaSystem.cpp](https://github.com/bambulab/BambuStudio/blob/master/src/slic3r/GUI/DeviceCore/DevFilaSystem.cpp) 的 info 位 4–7、dry_time 解析；枚举来自 [DevFilaSystem.h](https://github.com/bambulab/BambuStudio/blob/master/src/slic3r/GUI/DeviceCore/DevFilaSystem.h)：0 待机、1 检查、2 烘干、3 冷却、4 停止、5/6 异常，未支持值显示 --。dry_time 单位为分钟。只显示监控信息，不发烘干控制指令。
