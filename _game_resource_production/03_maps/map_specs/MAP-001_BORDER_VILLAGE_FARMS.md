# MAP-001 — Border Village & Farms

## Identity
Map ID: MAP-001
Name: Border Village & Farms
Type: HUB / INVESTIGATION / HYBRID
Base / Variant: BASE + ALERT state
Used Chapters: C01

## Purpose
Introduce player, war-worn daily life, ration interaction, first attack response and evidence investigation without splitting the village into separate maps.

## Story Functions
- payout/relief choice
- simultaneous alarm
- nearby farm defense
- discovery that supplies and residents were taken
- three evidence categories establish organized logistics

## Gameplay Functions
Movement, interaction, short combat, free-order investigation.

## Expected Playtime
Short-to-medium opening area. Exact minutes TBD; size must be driven by current movement speed, not arbitrary acreage.

## Entry Points
- ENTRY-01: village approach / post-contract return point.

## Exit Points
- EXIT-01: north trail toward MAP-002.

## Critical Path
ENTRY-01 → ration zone → alarm → closest farm combat → investigation cluster → EXIT-01.

## Optional Paths
- relief donation interaction: optional world-state payoff.
- additional evidence points can be inspected in any order.

## Major Zones
- ZONE-01 Village Approach — SAFE / DIALOGUE.
- ZONE-02 Ration Depot — DIALOGUE / INTERACTION.
- ZONE-03 Farm Edge Combat — COMBAT.
- ZONE-04 Investigation Farms — INVESTIGATION.
- ZONE-05 North Exit — TRANSITION.

## Landmarks
Ration depot; closed mill; damaged farm edge.

## NPC Placement
- NPC-GEN-01: ration clerk at ZONE-02.
- generic civilians: background/crowd zones around village/farms.
- UNIT-GUARD-01: alarm response between ZONE-01/03.

## Enemy / Encounter Placement
- ENC-001 farm raid: common goblin group entering from outer farm routes. Purpose: enemy reintroduction + consequence.

## Interactive Objects
OBJ-001 relief/ration crate; OBJ-002 sealed military crate; OBJ-003 notice board; OBJ-004 evidence props.

## Story Objects
Empty civilian ration supply, sealed military priority supply, missing logistics categories.

## Trigger Zones
TRG-001 payout interaction.
TRG-002 optional relief donation.
TRG-003 alarm after ration interaction/exit condition.
TRG-004 combat complete.
TRG-005 evidence completion.
TRG-006 north-route unlock/transition.

## Checkpoints
No new checkpoint system assumed. Use existing session behavior only unless project checkpoint support is confirmed.

## Shortcuts
None required; compact hub should not need an artificial shortcut.

## Hazards
No environmental damage hazard required.

## Collision Notes
Walkable roads/farm paths; structures, shelves and major props blocked. Maintain current player-radius clearance precedent. Door width remains TBD.

## Visibility / Guidance
Use road continuity, depot landmark, alarm direction and damaged farm state. Avoid relying only on UI markers.

## Environment Kit
KIT-TOWN-01.

## Unique Assets
Investigation evidence set and ration/depot interaction props.

## Shared Assets
Generic civilian, common crates/sacks, common environment props after QA.

## Variants
ENV_STATE-C01-01 ALERT: displaced civilians, damaged/emptied farm props, alarm state.

## Implementation Notes
Current TutorialLayout represents an older story topology. MAP-001 should reuse its world-coordinate conventions but not its obsolete quest sequence.
