# MAP-002 — North Forest Trail & Bridge

## Identity
Map ID: MAP-002
Name: North Forest Trail & Bridge
Type: INVESTIGATION / PUZZLE / ESCORT-INTERCEPT / HYBRID
Base / Variant: BASE + RAIN state
Used Chapters: C01

## Purpose
Turn tracking into play, teach that enemies use deception, then combine convoy interception, prisoner rescue and supply denial in one continuous route.

## Story Functions
- distinguish decoy and real wagon tracks
- recover path after creek
- discover prisoners and military supplies
- send military mark/mine location back toward village

## Gameplay Functions
Tracking puzzle, optional wrong-route ambush, combat, wagon disable interaction, prisoner release.

## Expected Playtime
Medium route. Wrong path adds resource cost but never blocks progress.

## Entry Points
- ENTRY-01 from MAP-001 north exit.

## Exit Points
- EXIT-01 toward MAP-003 mine exterior.

## Critical Path
ENTRY-01 → ZONE-01 fork clues → ZONE-02 creek clue → ZONE-03 ridge sightline → ZONE-04 convoy bridge → EXIT-01.

## Optional Paths
- decoy branch: optional ambush and return loop. Purpose = resource pressure / teaching.

## Major Zones
- ZONE-01 Track Fork — INVESTIGATION / PUZZLE.
- ZONE-02 Creek Crossing — INVESTIGATION.
- ZONE-03 Ridge Observation — EXPLORATION.
- ZONE-04 Wooden Bridge Convoy — COMBAT / INTERACTION.
- ZONE-05 Mine Route Exit — TRANSITION.

## Landmarks
Shallow creek; narrow wooden bridge; convoy wagon.

## NPC Placement
Rescued civilians grouped as a background/escape crowd in ZONE-04; no unique civilian sprites.

## Enemy / Encounter Placement
- ENC-002 wrong-path rear ambush.
- ENC-003 convoy guard fight at bridge.
- transport role reuses common goblin unless silhouette clarity fails.

## Interactive Objects
OBJ-005 tracking clue set; OBJ-006 wagon disable points; OBJ-007 restraints; OBJ-008 shipment invoice.

## Story Objects
Military mark and mine destination evidence.

## Trigger Zones
TRG-010 enter track fork.
TRG-011 choose/validate real trail.
TRG-012 wrong-path ambush if decoy chosen.
TRG-013 convoy spotted.
TRG-014 wagon disabled.
TRG-015 prisoners released.
TRG-016 evidence handed to escaping resident / route continues.
TRG-017 map exit to mine.

## Checkpoints
No new checkpoint system assumed.

## Shortcuts
No permanent shortcut required; decoy branch reconnects to critical route.

## Hazards
Rain reduces clue urgency narratively; water is collision/terrain boundary except at valid crossing.

## Collision Notes
Bridge must preserve player clearance; river blocked except crossing. Use current analytic collision conventions as precedent.

## Visibility / Guidance
Wheel-track depth, dropped grain, scraped bark and rope fiber are visual guidance. Ridge creates a readable reveal of the convoy.

## Environment Kit
KIT-FOREST-01.

## Unique Assets
Supply wagon interaction prop and tracking clue decals.

## Shared Assets
Forest props, common goblin, generic civilians.

## Variants
ENV_STATE-C01-02 RAIN: rain VFX, wet-ground state, clue visibility treatment.
