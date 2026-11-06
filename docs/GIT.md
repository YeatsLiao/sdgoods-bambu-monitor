# 哪些文件提交 Git

## 应提交

- main：应用源码、皮肤、运行素材C编码和生成字体；排除 config.local.h。
- components、platform：项目依赖、官方组件和预置资源，包括必要的预置bin文件。
- tools：构建、截图、生成、校验、备份工具；字体临时副本除外。
- docs/design：所有原型图、三批规范及设置说明。
- docs/assets：24张生成原图、图集、字体源及许可、运行编码镜像、索引和追溯说明。新竹叶素材也应提交，虽然当前运行未采用它。docs/assets/showcase中的项目首页精选预览也提交，以免克隆后README缺图。
- docs/verification/device：真机截图与反馈；docs/verification下的验证报告、JSON也提交。
- docs/history中的design、records、assets、verification及迁移清单：旧原型、提示词、旧资源和历史报告。
- 根目录README、.gitignore、CMakeLists.txt、sdkconfig.defaults、dependencies.lock、version.txt、截图入口和许可文件。

## 不提交，但保留本地

- build、build-*：编译缓存、临时渲染、构建日志及固件输出。
- dist：应用包、完整固件及冻结交付副本。
- backups：完整私有ZIP及校验文件，可能包含本地配置。
- sdkconfig、sdkconfig.old、main/bambu/config.local.h：本机配置和凭据。
- node_modules、managed_components、__pycache__、pyc和日志。
- tools/fonts中的TTF工作副本：其归档源在docs/assets/fonts/source提交。
- docs/verification/preview、docs/history/releases：大量程序生成的当前预览和历史渲染帧。

不提交不等于删除。所有历史渲染帧与当前预览已存入完整ZIP；Git用于维护源码和核心素材，完整ZIP另负责保存本机全部媒体和交付物。只克隆Git不会取得被忽略的旧渲染帧，完整媒体恢复应使用私有备份。tools/check_docs.py的全量媒体检查需要完整归档数据。

## 本轮状态

.gitignore已按以上范围分组并验证；核心原图、真机截图、历史原型及提示词均不会被忽略。目前只修改了规则和说明，尚未执行git add、commit或push。Git不会自动收录尚未暂存的新素材，提交时需要把以上“应提交”范围内的新增文件一并加入。

提交前查看 git status --short。不要把私有备份或本机凭据强制加入Git。完整保存及恢复方法见 [收尾记录](verification/closeout-0.3.9.md)。
