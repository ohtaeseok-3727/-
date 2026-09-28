# SMALL_KNIGHT_STYLE_SPEC

## Master Reference
- Reference folder: SimpleGame/Assets/Characters/SmallKnight/
- Read-only: YES
- Primary files: walk-transparent-v1.png, attacks-transparent-v2.png, effects-transparent-v2.png
- README confirms transparent RGBA final files and animation layout.

## Measured Sheet Structure
- Walk: 1536×1024, 4 columns × 2 rows = 8 frames.
- Walk cell: 384×512.
- Walk visible character bounds per cell: approximately 222–243 px wide × 321–331 px high.
- Attack: 1536×1024, 6 columns × 3 rows; rows are shield push / horizontal slash / downward slash.
- Effects: 1536×1024, 6 columns × 3 rows; matching impact/slash effect rows.
- Background contains alpha-0 regions; use actual transparent RGBA output.

## Visual Style
- Compact mini/chibi proportions; head and torso visually dominant, limbs short and thick.
- Large chunky pixel clusters; no high-frequency detail.
- Dark brown/near-black outline grouping.
- Limited stepped light/shadow masses; no smooth painted gradients.
- Minimal face detail.
- Equipment uses broad readable shapes rather than fine engraving.
- Lowest boot pixel is the ground/depth anchor.

## Project Sprite Override
- GAME_DESIGN.md requires the relevant mini walk sprite as the fixed body/pixel/palette reference.
- Knight fixed reference: SimpleGame/Assets/Characters/SmallKnight/walk-transparent-v1.png.
- Attack sprites must be newly drawn; do not copy/reuse walk pixels.
- Transparent alpha background mandatory.

## Do Not
- Do not modify or overwrite SmallKnight source files.
- Do not increase realism, face detail, shading complexity, antialiasing, or resolution detail.
- Do not use anime/concept-art/3D proportions.
- Do not change body structure across animation frames.
