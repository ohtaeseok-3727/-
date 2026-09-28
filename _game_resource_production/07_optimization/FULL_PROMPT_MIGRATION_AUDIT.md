# FULL PROMPT MIGRATION AUDIT

Governing prompt from this point forward:
../../docs/prompts/game_resource_and_map_production_full_prompt.md

## Chapter 1 gaps found and addressed
- Map Role Classification: ADDED.
- Entry / Exit: ADDED.
- Critical Path / Optional Path: ADDED.
- Zone structure: ADDED.
- NPC placement: ADDED.
- Enemy / Encounter placement: ADDED.
- Trigger IDs and Trigger specs: ADDED.
- Cutscene trigger rationale: ADDED.
- Collision notes: ADDED.
- Visibility / guidance: ADDED.
- Environment states / variant layers: ADDED.
- Map Connection Graph: ADDED.
- Map Grid Spec based on current project implementation: ADDED.
- ASCII Blockouts: ADDED.
- FINAL_MAP_MANIFEST: ADDED.
- CHAPTER_MAP_MATRIX: ADDED.
- MAP_PATH_REGISTRY: ADDED.
- Sprite Style Audit: ADDED.
- Map Implementation Audit against existing TutorialLayout/LevelMap: ADDED.

## Still intentionally not completed
- Actual new sprite PNG generation/deployment.
- Actual map-code rewrite/deployment.
- Pixel-level QA of not-yet-generated sprites.
- Runtime/build verification.
- Exact camera view size and global door-width standard, because they were not confirmed by inspected map files.

## Priority Rule
If older game_resource_production_prompt.md conflicts with ../../docs/prompts/game_resource_and_map_production_full_prompt.md, use the full prompt as the governing production workflow while keeping final story canon in CHAPTER_XX_FINAL.md and confirmed user settings in GAME_DESIGN.md.
