# FINAL_MAP_MANIFEST — CHAPTER 01

| Map ID | Name | Role | Base / Variant | Chapters | Environment Kit | Implementation Status |
|---|---|---|---|---|---|---|
| MAP-001 | Border Village & Farms | HUB / INVESTIGATION / HYBRID | BASE | C01 | KIT-TOWN-01 | SPEC_READY |
| MAP-002 | North Forest Trail & Bridge | INVESTIGATION / PUZZLE / HYBRID | BASE | C01 | KIT-FOREST-01 | SPEC_READY |
| MAP-003 | Abandoned Mine Base | DUNGEON / BRANCHING / BOSS_ARENA / HYBRID | BASE | C01 | KIT-MINE-01 | SPEC_READY |
| MAP-003-B | Mine Command Arena Damaged | BOSS/AFTERMATH STATE | VARIANT LAYER | C01 | KIT-MINE-01 | SPEC_READY |

Three base playable maps + one state variant layer. No Scene-to-Map 1:1 duplication.

Current project levels are authored in C++ through TutorialLayout / LevelMap rather than a standalone map-file format. Exact deployment path/format remains TBD until implementation.
