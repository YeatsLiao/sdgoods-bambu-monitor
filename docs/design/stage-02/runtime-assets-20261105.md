# 第二批还原素材与提示词

工具：内置 ImageGen。参考是第二批最初首页和 AMS 原图，未覆盖最初设计。输出完整保留源字节、透明 alpha 和 SHA，编码脚本 tools/assets/encode_batch2.cjs 只按六格裁出/等比缩放/编码，不重新画角色。

## 六格透明场景与独立头像

原图 exec-21d1d29f-3333-437d-bb33-282abd675760.png；工程源 main/assets/batch-02-sprites-v2.png；归档 docs/assets/images/batch-02-sprites-v2.png。各格为掌机、举船机器人、竹林、太空、机器人头像、宇航员头像。场景取高不透明度主体的边界定位，保留边界内原始 alpha。

提示词：

> Production game UI sprite atlas for the six round-screen UI reference designs, closely matching those reference objects and proportions. Transparent background, six equal cells, exactly 2 columns x 3 rows, each object isolated with generous transparent gutters. Row1 left: cream pixel handheld console framing a dark 3D printer viewport with orange Benchy boat, purple D pad, red buttons. Row1 right: same adorable cream purple helper robot from reference, full torso, holding orange Benchy boat, happy square face with mint cheeks and antenna, no base rectangle. Row2 left: wide lush bamboo workshop scene, large cream 3D printer with orange Benchy center, orange spool left, small plant table right, bamboo stalks framing scene, all resting on continuous thin shelf. Row2 right: wide navy space workshop scene, big cream blue printer orange Benchy, small white astronaut with round purple visor on right, circular purple Saturn window behind right, small crates and stars, all on thin shelf. Row3 left: isolated head-only version of same cream purple robot, happy face and antenna. Row3 right: isolated cute white astronaut head and shoulders with round purple visor. Crisp clean pixel art, exact friendly forms and theme colors from provided original concepts. No text, no labels, no numbers, no UI frame, no round dials, no solid color rectangular backgrounds. Each scene large and legible for 240px wide screen illustration. Preserve true alpha.

## 纸张纹理

原图 exec-1bd82f61-cd98-40ed-ad32-a98e9d3a0484.png；工程源 main/assets/paper-texture-v1.png；归档 docs/assets/images/paper-texture-v1.png。运行编码 96×96 RGB565，显示时降低不透明度保持文字清楚。

提示词：

> An evenly lit, flat warm ivory cream crumpled paper texture, no objects and no text. Match the subtle antique stationery paper seen behind the first cream paper-label round UI in this reference image. Extremely low contrast warm beige delicate fibers and shallow creases. Seamless square material tile, no border, no vignette, no gradients from lighting, no marks or stamps, no graphics, no letters, no shapes. It is a background texture only, for black and cobalt blue UI text to remain legible.

两项为原型还原用运行素材候选，不代表已经通过实机视觉验收。原始三张参考图与程序输出分目录管理。
