# MAP-003 — Abandoned Mine Base

## Identity
Map ID: MAP-003
Name: Abandoned Mine Base
Type: DUNGEON / BRANCHING / STEALTH / BOSS_ARENA / HYBRID
Base / Variant: BASE + COMMAND_DAMAGED variant
Used Chapters: C01

## Purpose
Show a functioning small army headquarters, allow multiple approach styles, stage Gruma's 1:1 duel and army-command escalation, then preserve the player's strategic success into the Hero payoff.

## Story Functions
- verify organized logistics/training
- identify command seal and succession
- reach Gruma
- 1:1 boss phase
- destroy three bells and close reinforcement gate
- Hero arrival and Gruma death
- inspect ledger and leave toward Graywall Fortress

## Gameplay Functions
Front-gate combat or drainage infiltration, investigation, risk-based route selection, boss duel, multi-enemy pressure, object-destruction objective, hold interaction.

## Expected Playtime
Longest C01 map. Exact duration TBD; S07 must be longest encounter, S08 shorter.

## Entry Points
- ENTRY-01 Front Gate.
- ENTRY-02 Drainage infiltration.

## Exit Points
- EXIT-01 Mine exit after aftermath, leading toward Chapter 2 route.

## Map Connections
MAP-002 EXIT-01 → MAP-003 ENTRY-01/02.
MAP-003 EXIT-01 → C02 Graywall route.

## Critical Path
ENTRY → logistics/training evidence → depth route → command arena → bell objectives → reinforcement gate → aftermath/ledger → EXIT.

## Optional Paths
- front gate: shorter, more combat.
- drainage: longer, lower initial combat.
- investigation of shift bell / cart records / seal route reduces approach risk.
Optional paths provide tactical information, not hard locks.

## Major Zones
- ZONE-01 Exterior / Front Gate — COMBAT.
- ZONE-02 Drainage Entry — STEALTH / EXPLORATION.
- ZONE-03 Relay Storehouse — INVESTIGATION.
- ZONE-04 Training / Repair Area — INVESTIGATION / COMBAT.
- ZONE-05 Lift / Depth Transition — TRANSITION.
- ZONE-06 Command Arena — BOSS / SURVIVAL.
- ZONE-07 Command Room / Aftermath — DIALOGUE / INVESTIGATION / SAFE.
- ZONE-08 Exit — TRANSITION.

## Landmarks
Mine gate; rail/lift system; main reinforcement gate.

## NPC Placement
- BOSS-001 Gruma: ZONE-06.
- NPC-HERO-01 Hero: enters ZONE-06 only after player strategic objective succeeds; remains ZONE-07 for dialogue.
- UNIT-GUARD-01 village guards arrive near exit/outer mine in aftermath if story state calls for it.
- rescued civilian is not physically required here; report has already traveled.

## Enemy / Encounter Placement
ENC-004 front-gate guards.
ENC-005 internal patrol / training group.
ENC-006 Gruma 1:1.
ENC-007 army-command escalation: melee, ranged/thrower support as required, healer.
ENC-008 Hero vs Gruma payoff: cutscene/result encounter, 30–45 sec target from story, no direct player control.

## Interactive Objects
OBJ-009 command seal; OBJ-010 patrol bell; OBJ-011 lift/records; OBJ-012 command bells x3; OBJ-013 reinforcement gate mechanism; OBJ-014 command ledger.

## Story Objects
Command seal succession evidence and ledger pointing to Graywall Fortress.

## Trigger Zones
TRG-020 approach choice.
TRG-021 logistics evidence threshold.
TRG-022 depth route access.
TRG-023 Gruma duel start.
TRG-024 armor-break state.
TRG-025 leg-injury state.
TRG-026 army escalation.
TRG-027 bell 1 destroyed.
TRG-028 bell 2 destroyed.
TRG-029 bell 3 destroyed.
TRG-030 reinforcement gate hold-complete.
TRG-031 player combat-incapacitated after mission success.
TRG-032 Hero arrival cutscene.
TRG-033 Gruma death.
TRG-034 ledger interaction.
TRG-035 C02 route/exit.

## Cutscene Triggers
TRG-032/033 only. Reason: player is already incapacitated and the story specifically requires payoff of conditions the player created, not a new playable boss phase.

## Checkpoints
Do not invent a new save/checkpoint system. If existing session restart support is later extended, candidate points are pre-command-arena and post-Gruma death.

## Shortcuts
If revisit occurs, lift/rail route may serve as a learned shortcut; do not add unless revisit is implemented.

## Hazards
No mandatory damage hazard beyond enemies. Narrow corridors and gate pressure are spatial hazards.

## Collision Notes
Mine floor/walls use current analytic bounds convention. Doors/corridors must exceed the current player footprint; exact standard width remains TBD.

## Visibility / Guidance
Shift bell rhythm, rail direction, supply flow, lighting toward depth, reinforcement gate prominence. Do not depend solely on markers.

## Environment Kit
KIT-MINE-01.

## Unique Assets
Command seal, bell states, reinforcement gate mechanism, ledger, boss-specific arena state.

## Shared Assets
Mine kit, crates, rails, timber supports, common goblins.

## Variants
MAP-003-B / ENV_STATE-C01-03 COMMAND_DAMAGED:
broken bells; healing inactive; gate closed; debris; Gruma armor/leg damage continuity; trapped troops audible/visible beyond gate.

## Implementation Notes
Current prototype mine rooms can inform scale/collision conventions but its older story topology must be replaced/adapted to this final Chapter 1 flow.
