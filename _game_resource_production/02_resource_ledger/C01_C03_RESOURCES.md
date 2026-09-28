# C01_C03_RESOURCES

## C01

### Locations
- LOC-001 Border Village Distribution Area — S01-S02 — PLAYABLE_MAP — village hub, ration station, adjacent farms.
- LOC-002 North Forest Trail — S03 — PLAYABLE_MAP — tracking route, creek, fork.
- LOC-003 Forest Wooden Bridge — S04 — PLAYABLE_MAP — convoy interception and prisoner rescue.
- LOC-004 Abandoned Mine Complex — S05-S09 — PLAYABLE_MAP — exterior, gate, drain, relay storehouse, training area, command depth.

### Playable Maps
- MAP-001 Border Village & Farms
- MAP-002 North Forest Trail & Bridge
- MAP-003 Abandoned Mine Base
- MAP-003-B Command Arena Damaged variant

### Characters
- NPC-HERO-01 Hero — MAJOR — female — ally/combatant/story.
- BOSS-001 Gruma — MAJOR/BOSS — commander, 1:1 boss, army-control boss.
- NPC-GEN-01 Generic civilian — GENERIC — farmer, resident, rescued prisoner, ration clerk representation.
- UNIT-GUARD-01 Village guard — generic allied unit; may reuse common humanoid unit presentation if available.
- ENEMY-GOB-01 Common Goblin — existing common enemy resource reuse.
- ENEMY-GOB-02 Goblin Archer — SPEC_REQUIRED only if existing common goblin cannot communicate ranged role through weapon-only variant.
- ENEMY-GOB-03 Goblin Thrower — SPEC_REQUIRED only if combat readability requires a separate weapon silhouette.
- ENEMY-GOB-04 Goblin Healer — SPEC_REQUIRED due to gameplay target priority.
- ENEMY-GOB-05 Goblin Transport/Guard role — reuse common goblin where combat behavior does not demand a unique silhouette.

### Important Objects
- OBJ-001 ration crate / relief donation box.
- OBJ-002 sealed front-line supply crate.
- OBJ-003 conscription notice / harvest-shortage note.
- OBJ-004 investigation evidence props: grain, salt, bandage box, lamp oil, whetstone, cart parts.
- OBJ-005 wheel tracks / dropped grain / rope fibers tracking clues.
- OBJ-006 military supply wagon with disable points.
- OBJ-007 prisoner restraints.
- OBJ-008 mine shipment invoice with mine mark.
- OBJ-009 command seal.
- OBJ-010 patrol/shift bell.
- OBJ-011 mine cart lift / transport record.
- OBJ-012 command bells ×3; one shared bell asset instanced three times.
- OBJ-013 main reinforcement gate drop mechanism.
- OBJ-014 command ledger.
- OBJ-015 damaged armor state marker for Gruma.

### Environment States
- Village: late afternoon → alarm/damaged farms → post-chapter recovery hint.
- Forest: cloudy evening → rain.
- Mine: night, active logistics base.
- Command arena: intact → boss armor damage → bells destroyed → healing disabled → reinforcement gate closed.

### Possible Reuse
- Existing Goblin/Common walk-v2.png and attack-v2.png.
- Existing Props rock/dead-tree/ruined-wall/campfire only where QA permits; wide halo issue remains and should not be treated as VERIFIED.
- One generic civilian sprite for ration clerk, farmers, residents and rescued prisoners.
- One bell sprite reused for shift bell and command bells, with state variant for destroyed.
- One supply crate family reused for ration/supply/logistics with decal/label variants.
- One mine architecture kit reused across exterior, corridors, relay storehouse and command depth.

## C02-C03
Not processed in this Chapter 1-only delivery.
