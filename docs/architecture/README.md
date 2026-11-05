# 架构与实施入口

- [代码分层与官方边界](overview.md)：哪些目录可以改，哪些官方文件保持原样。
- [MQTT 型号、名称和数据规则](mqtt-identity.md)：已核查的来源、可读取字段及缺失回退。
- [当前实施顺序](implementation-plan.md)：以第一批四页原型为准修复，不以历史“完成”记录代替验收。
- [来源与许可](upstream.md)：官方组件、复用代码、字体与生成素材来源。

代码仍是一个应用工程：main/apps 是 UI，main/bambu 是数据/网络，main/assets 与 main/fonts 是固件资源；components 和 platform 保持分离。资料分类不意味着把六款拆成六个固件。
