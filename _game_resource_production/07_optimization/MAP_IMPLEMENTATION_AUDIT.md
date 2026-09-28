# MAP_IMPLEMENTATION_AUDIT — CHAPTER 01

## Inspected Existing Implementation
- TutorialLayout.cpp/.h
- LevelMap.cpp/.h
- LevelTypes.h
- ../../docs/implementation/LEVEL_ONE.md

## Existing Strengths Reusable
- free-form world-coordinate authored regions
- road network and area topology
- analytic collision
- 128-unit procedural prop cells
- 512-unit render chunks
- player-radius collision inset
- mine walkable-area checks
- landmark and encounter-center abstractions

## Gaps Against Final C01 Story
1. Current TutorialLayout represents an older story flow with farmland, riverside, woods, battlefield and outpost investigation structure.
2. Final C01 requires S01-S02 village/farms, S03-S04 north forest tracking/convoy, S05-S09 consolidated mine base.
3. Existing mine topology can inform spatial scale/collision, but final C01 additionally needs logistics investigation, command-seal evidence, three bell objectives, reinforcement gate, persistent damage state and Hero payoff triggers.
4. No current trigger catalog matching TRG IDs was identified.
5. No confirmed state-layer mechanism for VILLAGE_ALERT / FOREST_RAIN / COMMAND_DAMAGED was identified in inspected map files.
6. No standalone map-file storage convention was identified; current implementation is code-authored.
7. Camera view size and globally standardized door width were not confirmed.
8. Existing ../../docs/implementation/LEVEL_ONE.md reflects an older story design and must not override CHAPTER_01_FINAL.md.

## Required Implementation Work
- refactor/adapt TutorialLayout data to the three-base-map C01 model
- add map/zone state representation
- add trigger/object-state data
- add encounter definitions aligned with final story
- preserve current collision/world-coordinate conventions where compatible
- keep state variants as layers, not duplicate maps

## Status
DESIGN: SPEC_READY
BLOCKOUT: BLOCKOUT_READY
CODE IMPLEMENTATION: NOT STARTED in this resource-production pass
MAP DEPLOYMENT: NOT DEPLOYED
VERIFICATION: NOT VERIFIED
