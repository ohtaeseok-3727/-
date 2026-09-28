# MAP-004 — Graywall Fortress & Front

## Identity
Map ID: MAP-004
Role: HUB / INVESTIGATION / FRONTLINE / HYBRID
Chapters: C02
Base / Variant: BASE + FRONT_BATTLE

## Purpose
Establish the apparent frontal siege, let the player inspect evidence that the army is a lure, and stage the first costly trust decision when the Hero abandons a possible clean pursuit.

## Entry / Exit
- ENTRY-01: arrival from Chapter 1 route.
- EXIT-01: outer scout route into MAP-005.
- EXIT-02: battle departure toward King's Canyon after S03.

## Critical Path
Operations room → battlement observation → outer watchpost/bridge approach → return to front line → signalman/beacon read → departure to canyon.

## Zones
- ZONE-01 Operations Room — DIALOGUE / INVESTIGATION.
- ZONE-02 Battlement — INVESTIGATION.
- ZONE-03 Outer Watchpost / Wagon Road — EXPLORATION / INVESTIGATION.
- ZONE-04 Front Defense Line — COMBAT.
- ZONE-05 Signal Flank — COMBAT / INVESTIGATION.

## Landmarks
Graywall gatehouse; front bridge; ruined watchpost.

## NPC Placement
- Hero: follows player, enters frontline combat at ZONE-04.
- Graywall commander: ZONE-01 and command edge of ZONE-04; reuse UNIT-MERC-01 visual unless later identification requires a unique sprite.
- Fortress soldiers: crowd/background/combat-support zones.

## Encounters
- ENC-009 outer scouting: low pressure, clue reading.
- ENC-010 frontal orc line: common orcs + signaler.

## Story Objects
OBJ-016, OBJ-017, OBJ-018, OBJ-019, OBJ-020.

## Triggers
TRG-040 operations briefing; TRG-041 evidence mismatch; TRG-042 schedule acquired; TRG-043 frontal battle; TRG-044 signal flare; TRG-045 pursuit-stop decision; TRG-046 exit to canyon.

## Collision
Walkable: corridors, battlements, front road, bridge.
Blocked: walls, parapets, collapsed watchpost, siege clutter.
Hazard: cliff/bridge edges blocked; no free fall assumed.
Interaction: map table, tracks, corpse/schedule, signal flare evidence.

## Visual Guidance
Gatehouse and bridge establish orientation; smoke and flare direction point toward King's Canyon.

## Variant
FRONT_BATTLE changes enemies, damaged props, fires/smoke and soldier placement without duplicating base geometry.
