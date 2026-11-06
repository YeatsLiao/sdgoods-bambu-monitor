# 第三组运行素材与提示词

工具：内置 ImageGen。最初三张设计图不覆盖，运行素材另存 main/assets，并由 archive_images.py 复制到 docs/assets/atlases。新图为还原用候选，真机验收单独进行。

## 六个大场景

输入参考：docs/design/batch-03/page-1-overview.png。
输出原图：exec-f173ffde-bf80-4221-8a0c-6c6b9ae78d99.png；工程源：main/assets/batch-03-scenes-v2.png。

Production pixel-art background atlas based closely on the SIX ORIGINAL round UI designs in the reference. This is NOT a new UI mockup. Exactly TWO columns by THREE rows of six fully opaque rectangular landscape panels, no gutters, no captions, no round masks, no progress bars, no UI numbers. Each panel is an independent wide scene that fills its entire panel including sky/background. Order: row1 left FARM red-roof wooden barn enclosing a cream 3D printer with orange Benchy boat, cute white hen on left, pumpkin and wheat right, blue sky green hedges and fence. row1 right CYBER dark rainy neon pink/indigo storefront, central black printer orange Benchy, purple cat left, plants right, wet reflective floor. row2 left UNDERWATER teal round submarine lab with central printer orange Benchy, yellow fish left, bubbles pink coral right, rich underwater background. row2 right SNOW warm wooden cabin with snow-covered roof, central teal printer orange Benchy, bundled penguin left, snowy pine right, mountains behind. row3 left DESERT solar panel shelter with central cream printer orange Benchy, tiny cream orange robot left, cactus and teal crates right, ochre desert hills. row3 right MAGIC purple alchemy room with domed printer orange Benchy, cute purple wizard cat left, potion bottles right, glowing gold stars. Match original buildings, characters, scene lighting and framing. Each complete building must fit with its roof intact and a little sky above, all important characters and the benchy viewport in the middle 80% of panel width. Consistent flat baseline at lower panel edge. Crisp low-resolution pixel art, broad coherent color areas rather than noise. No writing anywhere: no P1S nameplates, no signs or letters. Device labels will be dynamic program text.

## 六个角色横幅初稿

输入：最初 AMS、详情原型。输出：exec-6ca75823-f37f-4268-9b00-cc23fd33d6fe.png；工程源：main/assets/batch-03-headers-v1.png。

Create six pixel-art HEADER background panels matching the ORIGINAL round AMS and print-details UI references. Exactly TWO columns by THREE rows, each rectangular panel wide and shallow content, opaque background filling each panel, NO captions NO letters NO text NO UI controls NO printer NO percentage. Keep the CENTER 55% of every panel very plain and low detail so dynamic UI headings and a status pill can be drawn there. Put the character in the LEFT quarter, other theme objects in the RIGHT quarter. Row1 left FARM: cute white hen with red comb standing on grass at left, wheat at right, blue sky white clouds green hedges. Row1 right CYBER: same cute purple cat left, neon indigo city windows right, dark plum cyber backdrop with very restrained city details. Row2 left UNDERWATER: two cute yellow fish left, pink coral right, aqua water background and a few bubbles. Row2 right SNOW: bundled penguin with blue winter hat and orange scarf left, snowy pine right, light blue sky and snowy mountain edges. Row3 left DESERT: tiny cream orange robot with cyan face left, cactus right, ochre desert hill backdrop and small clouds. Row3 right MAGIC: cute purple wizard cat wearing star hat left, gold and violet potion bottles right, deep purple room background, a few tiny golden stars at edges. Faithfully retain character identities, palettes, pixel art style of provided references, do not replace them with generic icons. Keep all subjects compact vertically suitable for a 360x96px round-screen header, with quiet open sky/wall upper center for a heading. Absolutely no text or numbers baked into artwork. Six independent full-bleed panels, no gutters or borders.

## 圆屏角色内收修订

编辑横幅初稿，输出：exec-46c51f3b-96cb-480b-8428-7045ae480725.png；工程源：main/assets/batch-03-headers-v2.png。

Edit this six-panel pixel-art header atlas. Preserve exactly the same six panels, 2 columns x3 rows, same characters, palettes, scene details, backgrounds and pixel art quality. Change ONLY positioning and scale of edge characters and right objects so they fit a round display: in EVERY panel place the LEFT character(s) fully inside horizontal 13% to 28% of that panel width (center at 20%); place the RIGHT wheat/buildings/coral/pine/cactus/potions fully inside 72% to 88% (center at 80%). Character height stays about 50% of panel height, standing on baseline 90% down the panel. Keep the CENTRAL 44% (28%-72%) quiet sky or wall completely clear for dynamic heading and status. Preserve recognizable hen, purple cat, yellow fish, penguin, cream robot, wizard cat. No additional characters, no text, no labels, no UI controls. These headers are cropped by circular screen edges, so the characters must be moved inward, not left against rectangular outer edges. Opaque full-bleed backgrounds remain.

## 编码

源图完整保留；tools/encode_batch3.cjs 将每格按比例缩放，场景靠底适宽，顶部用源图顶部天空采样色补齐，避免边缘像素重复形成条纹，横幅居中适高并延展两侧边缘。不会把方形素材拉扁。每张先转 RGB565，再以加权中位切分量化到 256 色，索引使用 PackBits 游程/字面量编码。原始图不受量化影响。运行展开当前一张图至 RGB565，缓冲上限 61,440 字节，明确区分量化后的调色板与索引的无损压缩。

