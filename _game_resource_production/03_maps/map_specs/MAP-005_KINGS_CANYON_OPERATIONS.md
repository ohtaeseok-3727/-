# MAP-005 — King's Canyon Operations

## Identity
Map ID: MAP-005
Role: STRATEGIC CHOICE / MULTI-OBJECTIVE / RESCUE / DEMOLITION / HYBRID
Chapters: C02
Base / Variant: BASE + dynamic objective-state layers

## Purpose
Deliver the chapter's core choice: the player knowingly prioritizes one of three simultaneous crises and sees the other two worsen in real time.

## Entry / Exit
- ENTRY-01: from MAP-004 after pursuit is abandoned.
- EXIT-01: route to MAP-006 Old Human Fortress after Barkan's command location is recovered.
- RETURN-01: aftermath loop used in S09.

## Critical Path
Observation point → choose target order → objective 1 full stage → objective 2 emergency stage → objective 3 damage-control stage → recover command-location evidence → MAP-006.

## Optional Structure
No optional target is skippable for story completion, but order is player-controlled and changes losses. Do not display a recommended order.

## Zones
- ZONE-06 Observation / Tactical Map — SAFE / INVESTIGATION.
- ZONE-07 Beacon Tower — STEALTH / COMBAT / INTERACTION.
- ZONE-08 Siege Workshop — COMBAT / DEMOLITION PUZZLE.
- ZONE-09 Prisoner Camp — RESCUE / DEFENSE.
- ZONE-10 Main Bridge / Convoy Route — TRANSITION / ENVIRONMENT STATE.
- ZONE-11 Relief Camp — SAFE / AFTERMATH.

## Target Rules
First selected target: full length, best possible result.
Second target: visible partial damage, half-length emergency response.
Third target: full success impossible; minimize losses only.

## State Variables
- C02_BEACON_DELAY
- C02_BRIDGE_DAMAGE
- C02_REFUGEE_LOSS
- C02_SUPPLY_LOSS
- C02_ELITE_REINFORCEMENT
- C02_CHAPEL_ROUTE_INFO

## Encounters
- ENC-011 Beacon Tower infiltration.
- ENC-012 Siege Workshop demolition defense.
- ENC-013 Prisoner Camp rescue defense.

## Story Objects
OBJ-020 through OBJ-023, OBJ-030.

## Collision
Cliff edges and river/void blocked; bridges define crossings. Workshop charges use interaction collision; cages/restraints use interaction collision. Damaged bridge state blocks only destroyed segments, never the sole critical route before the chapter moves on.

## Visual Guidance
Three distant smoke/fire signatures remain visible from ZONE-06. Damage intensifies at unchosen targets after each objective transition.

## Environment States
TRIAGE_ACTIVE; ONE_SAVED; TWO_DAMAGED; FINAL_DAMAGE_CONTROL; AFTERMATH. Implement as state layers, not duplicate maps.
