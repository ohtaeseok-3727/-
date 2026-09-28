# FINAL ASSET MANIFEST — CHAPTER 01

| Asset ID | Type | Name | Reuse Class | Project Path | Status |
|---|---|---|---|---|---|
| CHAR-HERO-01 | CHARACTER | Female Hero | UNIQUE | SimpleGame/Assets/Characters/Hero/ | SPEC_READY |
| BOSS-001 | CHARACTER/BOSS | Gruma | UNIQUE | SimpleGame/Assets/Bosses/Gruma/ | SPEC_READY |
| CHAR-GENERIC-01 | CHARACTER | Generic NPC 01 | SHARED | SimpleGame/Assets/Characters/NPC/GenericNPC/ | SPEC_READY |
| ENEMY-GOB-01 | CHARACTER | Common Goblin | SHARED | SimpleGame/Assets/Monsters/Goblin/Common/ | VERIFIED_EXISTING |
| ENEMY-GOB-04 | CHARACTER | Goblin Healer | ROLE VARIANT | SimpleGame/Assets/Monsters/Goblin/Healer/ | SPEC_READY |
| MAP-001 | MAP | Border Village & Farms | BASE | level/environment data | SPEC_READY |
| MAP-002 | MAP | North Forest Trail & Bridge | BASE | level/environment data | SPEC_READY |
| MAP-003 | MAP | Abandoned Mine Base | BASE | level/environment data | SPEC_READY |
| MAP-003-B | MAP | Mine Command Arena Damaged | VARIANT | MAP-003 state | SPEC_READY |
| OBJ-006 | OBJECT | Supply Wagon | UNIQUE FUNCTIONAL | SimpleGame/Assets/Environment/Props/ | SPEC_READY |
| OBJ-009 | OBJECT | Command Seal | UNIQUE STORY | SimpleGame/Assets/Environment/Props/ | SPEC_READY |
| OBJ-012 | OBJECT | Command Bell | SHARED + VARIANT | SimpleGame/Assets/Environment/Props/ | SPEC_READY |
| OBJ-013 | OBJECT | Reinforcement Gate Mechanism | UNIQUE FUNCTIONAL | SimpleGame/Assets/Environment/Props/ | SPEC_READY |
| OBJ-014 | OBJECT | Command Ledger | UNIQUE STORY | SimpleGame/Assets/Environment/Props/ | SPEC_READY |
| KIT-MINE-01 | ENVIRONMENT | Mine Logistics Kit | SHARED | SimpleGame/Assets/Environment/ | SPEC_READY |
| VFX-RAIN-01 | VFX | Rain/Wet State | SHARED | project VFX path | PLANNED |

## Production Order
P0: map layouts, supply-wagon interaction, bells, reinforcement gate, common-goblin integration.
P1: female Hero, Gruma, healer, command seal/ledger, Generic NPC 01.
P2: tracking clues, mine logistics props, rain/wet state, crate variants.
P3: decorative clutter only after core assets are complete.

## Deployment Status
No new image PNG was generated in this pass. Existing assets were inspected and reused where valid. New visual assets are production-ready specifications and therefore remain SPEC_READY rather than GENERATED/DEPLOYED/VERIFIED.

## Safety/Integrity
- SmallKnight reference folder was not modified.
- Existing files were not overwritten.
- Existing common goblin asset remains the shared base enemy resource.
