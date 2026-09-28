# CHAPTER 01 PRODUCTION SPEC

## Map Consolidation
### MAP-001 — Border Village & Farms
Purpose: opening hub, ration interaction, alarm response, farm investigation.
Chapters: C01.
Base/Variant: BASE with alarm/damaged-state variant.
Critical Path: village entry → ration station → attacked farm → investigation points → north exit.
Landmarks: ration depot, closed mill, farm edge.
Unique Needs: ration shelf/counter, notice board, evidence prop set.
Reuse: shared crates/sacks, generic civilian, common environment props after QA.

### MAP-002 — North Forest Trail & Bridge
Purpose: tracking puzzle and convoy interception.
Chapters: C01.
Base/Variant: BASE with rain/ambush state.
Critical Path: trail fork → creek → ridge → wooden bridge encounter → mine route.
Landmarks: creek crossing, narrow timber bridge.
Unique Needs: track decals, clue props, supply wagon, restraints, shipment invoice.
Reuse: forest kit, common goblin, rocks/dead trees after QA.

### MAP-003 — Abandoned Mine Base
Purpose: infiltration, logistics discovery, named-boss encounter and aftermath.
Chapters: C01.
Base/Variant: UNIQUE BASE + MAP-003-B damaged command-arena state.
Critical Path: gate/drain → relay storehouse/training → lift/depth route → command arena → command room/exit.
Landmarks: mine gate, rail/lift infrastructure, reinforcement gate.
Unique Needs: command seal, mine logistics props, bells, reinforcement gate mechanism, ledger.
Reuse: one mine architecture kit across S05-S09.

## NPC Classification
| ID | Character | Class | Unique Sprite | Notes |
|---|---|---|---|---|
| NPC-HERO-01 | Hero | MAJOR | YES | Female, recurring central ally |
| BOSS-001 | Gruma | MAJOR/BOSS | YES | Named commander/boss |
| NPC-GEN-01 | Generic Civilian | GENERIC | ONE SHARED | all civilian/background roles |
| UNIT-GUARD-01 | Village Guard | GENERIC UNIT | NO by default | reuse until combat readability requires |
| ENEMY-GOB-01 | Common Goblin | ENEMY | EXISTING | reuse current Common asset |
| ENEMY-GOB-04 | Goblin Healer | ENEMY | YES/SPEC | must be immediately readable as priority target |

## Character Visual Direction
### Hero
- Female; combat-first heroic design.
- SmallKnight-compatible body scale, compact proportions, pixel density, outline and shading.
- Distinguish from player with one strong mantle/cape silhouette + one weapon/armor accent.
- Required C01: idle, walk, block/intercept, short finisher, hit-ready.
- Do not sexualize armor or increase anatomical realism.

### Gruma
- Same species/family as current common goblin; visibly larger/heavier, not a different monster species.
- Strong feature: inherited command seal.
- Secondary feature: one armor zone designed to break visibly.
- Required: idle, walk, melee, command-bell gesture, hit, armor-break state, injured-leg state, death.

### Generic NPC 01
- One reusable medieval civilian.
- Neutral work clothes, low-saturation palette, no memorable unique gear.
- Required: idle + walk.
- Reuse for ration clerk, farmers, villagers and rescued prisoners.

## Existing Assets to Reuse
- SimpleGame/Assets/Monsters/Goblin/Common/walk-v2.png
- SimpleGame/Assets/Monsters/Goblin/Common/attack-v2.png
- Props: rock/dead-tree/ruined-wall/campfire only after halo QA; not yet VERIFIED for deployment.

## Environment/Object Strategy
- One bell sprite, intact/destroyed states; instance it for all three command bells.
- One crate family with civilian/military/logistics label variants.
- One mine kit shared by gate, relay storehouse, training and command areas.
- Unique functional props kept: supply wagon, command seal, reinforcement gate mechanism, command ledger.

## Sprite Generation Prompts
### HERO-01
Create a game character sprite using SimpleGame/Assets/Characters/SmallKnight/walk-transparent-v1.png as the exact body/pixel/style master reference. Female heroic warrior. Match body proportions, frame scale, chunky pixel density, outline weight, shading complexity, camera presentation, foot anchor, face-detail and equipment-detail density. Distinct from player through one mantle/cape silhouette and one weapon/armor accent. Required C01: idle, walk, block/intercept, short finisher, hit-ready. Attack frames must be newly drawn, not copied from walk pixels. Actual alpha transparency required.

### BOSS-001 GRUMA
Create a goblin commander sprite consistent with SimpleGame/Assets/Monsters/Goblin/Common/walk-v2.png and the SmallKnight chunky pixel-art production style. Gruma is larger/heavier than a common goblin but must remain the same species and sprite pack. Key features: command seal, commander armor, one breakable armor zone, leg-injury state. Required: idle, walk, melee, command-bell gesture, hit, armor-break variant, injured movement, death. No unexplained magical glow. Actual alpha transparency required.

### GENERIC_NPC_01
Create one reusable medieval civilian NPC sprite. Match SmallKnight scale, pixel density, compact proportions, camera presentation, outline, shading steps and face-detail level. Neutral work clothes, low-saturation palette, no unique weapon/heraldry/story accessory. Required: idle and walk. Actual alpha transparency required.

### GOBLIN HEALER
Create a goblin healer unit derived from the existing common goblin visual family. Preserve body proportions and pixel density while making the support role readable through one simple satchel/bandage/tool silhouette, not elaborate robes. Required: idle, walk, heal action, hit, death.

## Optimization Result
Chapter 1 can be produced with three base maps, one map-state variant, two unique named character sprites, one generic civilian sprite, the existing common goblin asset, one healer-role goblin, and a limited set of functional props. Archer/thrower visuals stay conditional until gameplay readability proves them necessary.
