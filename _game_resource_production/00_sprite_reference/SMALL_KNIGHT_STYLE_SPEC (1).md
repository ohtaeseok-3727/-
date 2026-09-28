# SMALL_KNIGHT_STYLE_SPEC

## Master Reference
- Reference folder: SimpleGame/Assets/Characters/SmallKnight/
- Read-only: YES
- Primary files: walk-transparent-v1.png, attacks-transparent-v2.png, effects-transparent-v2.png
- README confirms transparent RGBA final files and animation layout.

## Measured Sheet Structure
- Walk sheet: 1536×1024, 4 columns × 2 rows = 8 frames.
- Walk cell: 384×512.
- Attack sheet: 1536×1024, 6 columns × 3 rows = 18 cells.
- Attack cell: 256×341.33 average grid height; production slicing must follow source row bounds rather than assume perfectly even integer rows.
- Effects sheet: 1536×1024, 6 columns × 3 rows.
- Walk visible character bounds per cell are approximately 222–243 px wide × 321–331 px high.
- Alpha background is transparent; generated files include soft edge alpha.

## Visual Style
- Camera: side/front-biased upright character presentation compatible with the project 2.5D world.
- Body: compact chibi/mini proportions; head and torso are visually dominant, limbs short and thick.
- Pixel density: large chunky pixel clusters; avoid high-frequency detail.
- Outline: dark brown/near-black edge grouping, not pure vector outlines.
- Shading: limited stepped light/shadow masses; avoid smooth painted gradients.
- Face: minimal readable eyes/face, no anime portrait detail.
- Equipment: broad readable shapes; detail subordinated to silhouette.
- Foot anchor: lowest visible boot pixels define the ground/depth anchor.
- Weapon swing: broad silhouette arcs with large readable spacing from torso.

## Animation Reference
- Walk: 8 frames total, arranged 4×2.
- Attack: 3 attack rows × 6 cells: shield push / horizontal slash / downward slash according to README.
- Effects: matching 3 rows × 6 cells for impact/slash effects.
- New characters should reuse the same animation philosophy and frame density where the gameplay role requires equivalent motions.

## Project Sprite Override
- GAME_DESIGN.md requires all character sprite work to use the relevant mini walk sprite as the fixed body/pixel/palette reference.
- Knight fixed mini reference: SimpleGame/Assets/Characters/SmallKnight/walk-transparent-v1.png.
- Attack sprites must be newly drawn; do not copy/reuse original walk pixels.
- Transparent alpha background is mandatory.

## Do Not
- Do not modify or overwrite SmallKnight source files.
- Do not increase realism, resolution detail, face detail, shading steps, or antialiasing.
- Do not use concept-art/anime/3D proportions.
- Do not change limb count or body structure across animation frames.
