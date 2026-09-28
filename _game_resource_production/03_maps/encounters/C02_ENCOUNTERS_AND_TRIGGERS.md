# C02 ENCOUNTERS AND TRIGGERS

## ENC-009 — Outer Evidence Sweep
Map: MAP-004
Zone: ZONE-03
Purpose: Mechanical Teaching / Investigation.
Enemy: minimal patrol pressure; evidence is primary.

## ENC-010 — Graywall Front
Map: MAP-004
Zone: ZONE-04/05
Entry: enemy approaches from frontal field and signal flank.
Wave: Common Orcs + one Signaler.
Purpose: Escalation + Story Consequence.

## ENC-011 — Beacon Tower
Map: MAP-005
Zone: ZONE-07
Enemy: Common Orcs, Signaler, late Elite if delayed.
Purpose: stealth/signal shutdown; controls elite reinforcement flag.

## ENC-012 — Siege Workshop
Map: MAP-005
Zone: ZONE-08
Enemy: Common Orcs + Siege Engineers.
Purpose: demolition disarm and bridge-loss control.

## ENC-013 — Prisoner Camp
Map: MAP-005
Zone: ZONE-09
Enemy: Common Orcs + Elite depending delay.
Purpose: rescue/defense; controls survivor count and chapel-route intel.

## ENC-014 — Fortress Entry
Map: MAP-006
Zone: ZONE-12/14
Enemy: Common Orcs, route-specific pressure.
Purpose: reflect prior S05 choice without creating separate maps.

## ENC-015 — Split Pressure
Map: MAP-006
Zone: ZONE-15/16
Hero is not losing; enemies are consuming her time. Player disables one demolition line and opens bypass.
Purpose: relationship reversal / tactical teaching.

## ENC-016 — Barkan
Map: MAP-006
Zone: ZONE-17
Enemy: BOSS-002 + optional Orc Elite reinforcements based on C02_ELITE_REINFORCEMENT.
Purpose: cooperative boss / chapter climax.
Player causes final opening by cutting retreat chain/drop gate and destroying exposed demolition/command target after Hero breaks armor stance.

# Trigger Specs

## TRG-040
Map: MAP-004 | Zone: ZONE-01 | Type: NPC_DIALOGUE
Condition: chapter entry.
Action: establish frontal-defense assumption and allow evidence inspection.
One Shot: YES
Next: TRG-041

## TRG-041
Map: MAP-004 | Zone: ZONE-02/03 | Type: INTERACT
Condition: footprint/wagon evidence inspected.
Action: mark siege-mismatch suspicion.
One Shot: YES
Next: TRG-042

## TRG-042
Map: MAP-004 | Zone: ZONE-03 | Type: ITEM_ACQUIRED
Condition: scout schedule fragment acquired.
Action: unlock schedule timing evidence.
One Shot: YES
Next: TRG-043

## TRG-044
Map: MAP-004 | Zone: ZONE-05 | Type: OBJECT_STATE
Condition: retreat signaler fires canyon beacon.
Action: reveal lure logic; enable pursuit-stop beat.
One Shot: YES
Next: TRG-045

## TRG-045
Map: MAP-004 | Zone: ZONE-04 | Type: NPC_DIALOGUE
Condition: player has schedule + beacon evidence.
Action: Hero abandons pursuit despite tactical cost.
One Shot: YES
Next: TRG-046

## TRG-050
Map: MAP-005 | Zone: ZONE-06 | Type: INTERACT
Condition: tactical map combines ledger/schedule/beacon direction.
Action: expose three simultaneous objectives and their explicit risks.
One Shot: YES
Next: TRG-051

## TRG-051
Map: MAP-005 | Zone: ZONE-06 | Type: INTERACT
Condition: player selects first objective.
Action: lock objective order; start degradation timers/state progression.
One Shot: YES
Next: TRG-052

## TRG-052 / 053 / 054
Map: MAP-005 | Type: COMBAT_COMPLETE / OBJECT_STATE
Condition: each objective resolved in chosen order.
Action: write corresponding loss flags; worsen remaining target states.
One Shot: YES
Next: next unresolved objective / TRG-055

## TRG-055
Map: MAP-005 | Type: ITEM_ACQUIRED
Condition: all three targets resolved.
Action: reveal Old Human Fortress command location; calculate route availability.
One Shot: YES
Next: TRG-060

## TRG-060
Map: MAP-006 | Zone: ZONE-12 | Type: ENTER_ZONE
Condition: S05 state flags loaded.
Action: open aqueduct / chapel / breach route set appropriate to prior results.
One Shot: YES
Next: TRG-061

## TRG-061
Map: MAP-006 | Zone: ZONE-13 | Type: INTERACT
Condition: mural inspected.
Action: acquire rubbing/fragment; record seven figures / six known names contradiction.
One Shot: YES
Next: TRG-062

## TRG-062
Map: MAP-006 | Zone: ZONE-15/16 | Type: OBJECT_STATE
Condition: split-pressure encounter active.
Action: player disables demolition line and opens bypass, then signals Hero to disengage.
One Shot: YES
Next: TRG-063

## TRG-063
Map: MAP-006 | Zone: ZONE-17 | Type: COMBAT_COMPLETE
Condition: Barkan opening created by player interaction and boss HP/phase threshold met.
Action: short joint finisher; Barkan defeated; bridge damage resolves.
One Shot: YES
Next: TRG-064

## TRG-064
Map: MAP-005 | Zone: ZONE-11 | Type: ENTER_ZONE
Condition: return after boss.
Action: materialize refugee/supply/wounded/bridge aftermath from S05 flags.
One Shot: YES
Next: TRG-065

## TRG-065
Map: MAP-005 | Zone: ZONE-11 | Type: INTERACT
Condition: mural evidence reviewed after aftermath.
Action: set Chapter 3 goal: compare official tradition with original records.
One Shot: YES
