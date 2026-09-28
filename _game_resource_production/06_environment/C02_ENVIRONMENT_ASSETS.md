# C02 ENVIRONMENT ASSETS

## KIT-FORTRESS-01
Use: MAP-004 Graywall Fortress.
Assets: military stone floor/wall, battlement/parapet, gatehouse pieces, bridge approach, operations-room table, banner/signage, watchpost debris, defensive clutter.
Reuse: stone/wood props where project assets pass QA.

## KIT-CANYON-01
Use: MAP-005 King's Canyon.
Assets: cliff ground/wall, canyon road, bridge deck/support, beacon-tower structure, temporary siege-workshop scaffolds, prisoner-camp fence/cage, relief-camp props.

## KIT-RUINS-01
Use: MAP-006 Old Human Fortress.
Strategy: damaged/aged variant of fortress stone kit plus chapel-specific pieces.
Assets: cracked walls, collapsed doorway, aqueduct pieces, chapel columns/floor, inner gates, control-platform structure.

## Unique Story Objects
- OBJ-019 scout schedule fragment — small document pickup, shared document style.
- OBJ-020 signal beacon — reusable intact/disabled state.
- OBJ-021 explosive charges — shared demolition object.
- OBJ-022 bridge support brace / demolition point — interactive state variants.
- OBJ-024 seven-figure mural — UNIQUE story art. Exactly seven returning human-like figures; one figure's face/name area visibly scratched out later; the erased figure reaches toward a fallen non-human warrior. Do not reveal identity.
- OBJ-025 mural rubbing/fragment — inventory/evidence version of OBJ-024.
- OBJ-028 retreat chain/drop-gate control — unique boss interaction.
- OBJ-029 Barkan command/demolition target — unique boss interaction.

## Environment States
- ENV_STATE-C02-01 GRAYWALL_FRONT_BATTLE.
- ENV_STATE-C02-02 CANYON_TRIAGE_ACTIVE.
- ENV_STATE-C02-03 CANYON_TARGET_DAMAGE.
- ENV_STATE-C02-04 CANYON_AFTERMATH.
- ENV_STATE-C02-05 OLD_FORTRESS_ROUTE_SET.
- ENV_STATE-C02-06 BARKAN_ARENA_DAMAGE.

## Reuse Cost Rule
Do not build separate tilesets for each state. Use base kit + damaged props + blocked routes + VFX + NPC/enemy placement changes.
