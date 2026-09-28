# C01 ENCOUNTERS AND TRIGGERS

## ENC-001 — Farm Raid
Map: MAP-001
Zone: ZONE-03
Entry: outer farm edges
Enemy: ENEMY-GOB-01 common goblins
Purpose: Enemy Introduction + Story Consequence + basic combat reinforcement.

## ENC-002 — Wrong-Path Ambush
Map: MAP-002
Zone: decoy branch
Entry: rear/flank after false trail commitment
Enemy: common goblins
Purpose: Mechanical Teaching + Resource Pressure. Wrong answer costs resources but never blocks progress.

## ENC-003 — Convoy Interception
Map: MAP-002
Zone: ZONE-04 bridge
Entry: convoy front/rear lanes
Enemy: common escort goblins; transport role reuses common sprite
Purpose: Story Consequence + object interaction teaching.
Objective: disable wagon, release prisoners, secure evidence.

## ENC-004 — Mine Gate
Map: MAP-003
Zone: ZONE-01
Enemy: common goblin patrol/guards
Purpose: route-choice contrast. Front gate is shorter and combat-heavy.

## ENC-005 — Internal Patrol / Training
Map: MAP-003
Zone: ZONE-03/04
Enemy: common goblins; ranged/thrower only where combat role needs it
Purpose: Escalation + environmental storytelling.

## ENC-006 — Gruma Duel
Map: MAP-003
Zone: ZONE-06
Enemy: BOSS-001 Gruma
Purpose: Boss Setup + prove player can fight Gruma evenly.
Arena: open enough for readable 1:1 mobility; avoid clutter.
State changes: armor break, leg injury.

## ENC-007 — Army Command Escalation
Map: MAP-003
Zone: ZONE-06 + upper approach
Enemy: common melee + ranged/thrower as needed + ENEMY-GOB-04 healer
Purpose: Escalation + Resource Pressure + reveal Gruma's real strength is command.
Objective shift: defeat boss → destroy bells ×3 + close reinforcement gate.

## ENC-008 — Hero Payoff
Map: MAP-003
Zone: ZONE-06
Type: CUTSCENE/RESULT
Purpose: payoff of player-created strategic conditions.
Direct control: none.
Constraint: shorter than ENC-007; story specifies roughly 30–45 seconds.

# Trigger Specs

## TRG-001
Map: MAP-001
Zone: ZONE-02
Type: INTERACT
Condition: player interacts with ration clerk/payment point.
Action: payout; optional relief choice becomes available.
One Shot: YES
Next: TRG-002/003.

## TRG-003
Map: MAP-001
Zone: ZONE-02/03
Type: OBJECT_STATE / ENTER_ZONE
Condition: opening interaction completed.
Action: trigger three-direction alarm; activate nearest-farm encounter.
One Shot: YES
Next: TRG-004.

## TRG-005
Map: MAP-001
Zone: ZONE-04
Type: INTERACT
Condition: required evidence categories discovered.
Action: mark organized logistics conclusion; enable north-route continuation.
One Shot: YES
Next: TRG-006.

## TRG-011
Map: MAP-002
Zone: ZONE-01/02
Type: INTERACT / ENTER_ZONE
Condition: player validates real trail using load-depth and physical clues.
Action: route guidance continues to ridge/convoy.
One Shot: YES
Next: TRG-013.

## TRG-014
Map: MAP-002
Zone: ZONE-04
Type: OBJECT_STATE
Condition: wagon pin/axle disabled.
Action: convoy movement stops.
One Shot: YES
Next: TRG-015.

## TRG-015
Map: MAP-002
Zone: ZONE-04
Type: INTERACT
Condition: prisoner restraints released.
Action: civilians escape; report path toward village established.
One Shot: YES
Next: TRG-016.

## TRG-023
Map: MAP-003
Zone: ZONE-06
Type: ENTER_ZONE / CUTSCENE
Condition: player reaches command arena.
Action: Gruma introduction and 1:1 combat start.
One Shot: YES
Next: TRG-024/025.

## TRG-026
Map: MAP-003
Zone: ZONE-06
Type: OBJECT_STATE
Condition: Gruma reaches escalation condition after player advantage.
Action: army reinforcements/healer enter; objective changes to bells + gate.
One Shot: YES
Next: TRG-027.

## TRG-027 / 028 / 029
Map: MAP-003
Zone: ZONE-06
Type: OBJECT_STATE
Condition: each command bell destroyed.
Action: reduce corresponding reinforcement/support behavior; after third, stop new reinforcement calls.
One Shot: YES
Next: TRG-030.

## TRG-030
Map: MAP-003
Zone: ZONE-06
Type: INTERACT
Condition: player completes hold interaction on reinforcement gate mechanism.
Action: close gate; isolate army; mark strategic mission success.
One Shot: YES
Next: TRG-031.

## TRG-032
Map: MAP-003
Zone: ZONE-06
Type: CUTSCENE
Condition: mission already successful; player becomes combat-incapacitated; Gruma attempts execution.
Action: Hero arrives and blocks execution.
One Shot: YES
Next: TRG-033.

## TRG-034
Map: MAP-003
Zone: ZONE-07
Type: INTERACT
Condition: Gruma dead; Hero/player aftermath active.
Action: inspect ledger; reveal Graywall Fortress orc movement.
One Shot: YES
Next: TRG-035.

## TRG-035
Map: MAP-003
Zone: ZONE-08
Type: MAP_EXIT
Condition: aftermath dialogue/ledger complete.
Action: Chapter 1 exit toward Chapter 2 goal.
One Shot: YES
