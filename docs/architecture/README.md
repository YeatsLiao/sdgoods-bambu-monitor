# 架构入口 · 0.3.9

- [现行分层与数据流](overview.md)：模块职责、线程边界、资源生命周期和官方接口。
- [MQTT身份与数据规则](mqtt-identity.md)：型号、AMS/HT字段、未知值和来源。
- [维护与剩余验收](implementation-plan.md)：当前收尾基线及后续变更检查。
- [来源与许可](upstream.md)：复用代码、组件与字体来源。
- [素材追溯](../assets/CATALOG.md)：18套皮肤到原图、编码和生成脚本的关系。

项目始终是一份固件，所有皮肤共用连接、状态、分页与设置。官方层位于 components、platform；拓竹应用位于 main。当前整理不改变运行时结构和UI行为。
