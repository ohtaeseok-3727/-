# Chapter 2 Orc sprites — v1

## Deterministic cleanup v2 — 2026-09-24

User explicitly authorized script editing. Four v2 sheets now reside in Common, Elite and Barkan alongside their per-sheet JSON frame metadata. `../../../../tools/assets/prepare_chapter2_sprites.py` extracts the original 56 new combat poses with alpha >=240, removes small disconnected fringe noise, makes the foreground fully opaque, and repacks without resizing or changing retained RGB values. All use 640×640 cells and (320,512) pivots, with >=113px clear margins to every cell edge. Grounded poses align to the common baseline; one raised jump pose retains 34px lift. File round-trip, binary alpha, RGB and margin assertions passed; four contrast-background previews were visually inspected. The original v1 sheets are preserved. No runtime integration or game execution; anatomy/animation approval remains separate from this completed technical cleanup. Details: `output/chapter2-orc-combat-sprites/README.md` and `inspection-v2.json`.

## Combat draft follow-up — 2026-09-24

New combat pose candidates and exact generation prompts are saved separately in `output/chapter2-orc-combat-sprites/`. Common/Elite: idle, swing, hit, death; Barkan: swing, jumping slam, battle cry, plus idle/hit/collapse support poses. All remain REVIEW_ONLY because generated cell dimensions, partial-alpha halos and exact walk-master consistency need correction. No runtime assets were replaced. See that directory's README.md and inspection.json for measured dimensions, alpha counts and remaining limitations. Barkan's three attacks are now user-confirmed; detailed targeting/buff parameters remain unconfirmed.

Built-in image generation, 2026-09-22. Branch: `codex/chapter-02-orc-sprites`.

## Deliverables

- `Common/walk-v1.png`: common orc, leather/iron equipment, one cleaver.
- `Elite/walk-v1.png`: same species, heavier iron armor and larger cleaver.
- `Barkan/walk-v1.png`: broader commander, asymmetric shoulder armor, deep red cloth, brass seal.

All sheets: 1536×1024, four columns × two rows, 384×512 cells, eight walk poses. Generated assets only; not connected to runtime. Attack, hit, death and Barkan's other required animations remain outstanding.

## Sources and generation prompt specifications

Production authority: `_game_resource_production/05_sprite_prompts/C02_SPRITE_PROMPTS.md` and `04_characters/major_npc_specs/BOSS-002_BARKAN.md`.

Player references: SmallKnight/walk-transparent-v1.png and SmallMage/walk-transparent-v2.png. Lock compact head/torso proportions, short thick limbs, chunky pixel size, dark outlines, limited stepped shading and palette family. User explicitly requests larger orc bodies. Numeric size targets were production assumptions, not user-confirmed settings.

Common prompt: newly draw eight walking poses, muted green skin, short tusks, leather and iron armor, broad melee cleaver; enlarged body with unchanged pixel density; front three-quarter view, four by two cells, consistent anatomy and foot anchors; true transparent RGBA background, no text or glow. A targeted correction removed inconsistent secondary axes, leaving one cleaver in the anatomical right hand and the other hand empty.

Elite prompt: use corrected common walk as species/body master; preserve head, torso, limbs, face, green palette and chunky pixels; heavier dark iron chest/shoulder armor, reinforced boots and gauntlets, larger right-hand cleaver; no command cloth or seal; eight newly drawn walking poses with transparent RGBA background.

Barkan prompt: use common and elite walk references; larger and broader compact orc body, same pixel density and shading; dark iron segmented armor, asymmetric shoulder plate, deep red command cloth, brass seal, heavy right-hand cleaver; eight newly drawn walking poses, transparent RGBA, no beast anatomy or fine realistic details.

## Inspection and limitations

Visual review: three identities readable; corrected common sheet retains one weapon in every pose. Barkan is broader, but height separation from Elite is small.

First-cell measurements (alpha >=128): player knight 323px; common 398px; elite 413px; Barkan 418px. These measure image pixels, not runtime display sizes. All three are larger than the player at equal sheet scale.

Actual alpha=0 pixels exist in each first cell: Common 104085, Elite 97242, Barkan 79270. Partial-alpha pixels also exist; these are not binary-alpha pixel sprites. Exact palette equality, pixel-grid uniformity, every-frame anchor alignment and absence of translucent edge halos have not been established. Treat v1 as review assets rather than fully validated production-ready animations. No game build or execution performed.
