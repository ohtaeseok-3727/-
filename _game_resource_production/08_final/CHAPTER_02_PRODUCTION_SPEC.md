# CHAPTER 02 PRODUCTION SPEC — 협곡의 미끼

## Production Summary
Chapter 2 uses three base maps and dynamic state layers. It reuses the Hero, Generic NPC and existing human mercenary visual; new character production focuses on the Orc family and Barkan.

## Maps
1. MAP-004 Graywall Fortress & Front — briefing, evidence, frontal battle, pursuit-abandon decision.
2. MAP-005 King's Canyon Operations — three simultaneous objectives and visible consequence state changes.
3. MAP-006 Old Human Fortress — route-dependent infiltration, erased mural, tactical separation, Barkan boss.

## Core Choice System
The player chooses the order of Beacon Tower / Siege Workshop / Prisoner Camp. The first is fully saveable, second partially damaged, third can only be damage-controlled. Persist loss flags through boss setup and aftermath.

## Major New Character
BOSS-002 Barkan — unique orc general, SmallKnight rendering style, Common Orc species family.

## Enemy Family
Common Orc plus Signaler, Siege Engineer and Elite role variants.

## Key Story Asset
OBJ-024 seven-figure chapel mural with one later-erased face/name and the erased figure extending a hand to a fallen non-human warrior. The asset must show the contradiction without identifying the figure.

## Deployment Targets
- Orcs/Barkan: `SimpleGame/Assets/Monsters/Orc/`
- Environment: existing `SimpleGame/Assets/Environment/` hierarchy with new Fortress/Canyon/Ruins folders as needed.
- Story/interactive props: existing `Environment/Props/` hierarchy; use a Chapter02 subfolder only to avoid naming collisions.

## Status
Design: SPEC_READY
Blockout: BLOCKOUT_READY
New image assets: GENERATED only after successful image creation and alpha/style QA.
Map code implementation: not part of this resource-only pass unless explicitly requested.
