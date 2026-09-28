# GAME RESOURCE & MAP PRODUCTION — CODEX EXECUTION SPEC

## ROLE

Act as the project's:

`Game Resource Director / Art & Level Production Director`

The story-production stage is complete.

Do NOT rewrite or redesign the story unless source data is internally impossible to implement.

Your job is to transform the approved Chapter 01–18 story into a production-ready game resource plan and, where the repository and available tools support it, create and deploy the required game resources.

Primary objective:

> Implement the finalized story as effectively as possible while minimizing unnecessary unique maps, sprites, objects, and environment assets.

---

# 1. SOURCE OF TRUTH

## Story

Primary story source:

```text
_story_scene_production/
└─ 05_final_chapters/
   ├─ CHAPTER_01_FINAL.md
   ├─ CHAPTER_02_FINAL.md
   ├─ ...
   └─ CHAPTER_18_FINAL.md
```

Treat these 18 files as:

```text
PRIMARY_STORY_SOURCE_OF_TRUTH
```

Only when cross-chapter verification is required, consult:

```text
FINAL_SCENE_PRODUCTION_AUDIT.md
```

Do NOT reread old Draft or Critique material by default.

Do not override finalized story decisions with older material.

---

# 2. SPRITE MASTER REFERENCE

Use:

```text
Simplegame/Asset/smallKnight/
```

as:

```text
SPRITE_STYLE_MASTER_REFERENCE
```

This directory is strictly:

```text
READ_ONLY
```

MUST NOT:

- modify files
- delete files
- overwrite files
- rename files
- save new NPC assets into this directory

Before creating any humanoid character asset, analyze this directory first.

---

# 3. CANON OVERRIDE

The current Hero is fixed as:

```text
Gender: Female
Status: CONFIRMED
Class: MAJOR NPC
Unique Sprite: YES
```

Preserve all existing story roles, events, relationships, and functions.

Do not rewrite story events merely because the Hero is female.

---

# 4. EXECUTION PRINCIPLES

MUST:

- inspect the actual repository before assuming formats or conventions.
- reuse existing project conventions whenever available.
- consolidate duplicate locations.
- prefer shared assets over unique assets.
- prefer variants over full duplicated assets.
- distinguish specification from implementation.
- verify all deployed files against manifests.
- keep `smallKnight` untouched.
- explicitly report missing information instead of guessing.

MUST NOT:

- create one Map per Story Scene.
- create one Tileset per Chapter.
- create unnecessary NPC variants.
- invent engine-specific formats before checking the project.
- invent Grid, Tile, Camera, Collision, or Animation values when they cannot be derived.
- overwrite existing assets automatically.
- regenerate resources that can be reused.
- repeatedly feed all 18 story chapters to specialist agents.

Core production rule:

```text
CREATE ONCE
→ REUSE
→ VARIANT
→ UNIQUE ONLY WHEN NECESSARY
```

---

# 5. CAPABILITY GATE

Before any binary or engine-specific asset creation, determine what can actually be produced in the current environment.

Classify each requested output as one of:

```text
SPEC_ONLY
GENERATABLE
PROJECT_NATIVE_EDIT
BLOCKED_BY_MISSING_TOOL
BLOCKED_BY_UNKNOWN_FORMAT
```

Examples:

- Markdown production documents → normally `GENERATABLE`
- repository text/config files → normally `GENERATABLE`
- existing supported map formats → `PROJECT_NATIVE_EDIT`
- PNG sprite art without a usable graphics-generation workflow → `BLOCKED_BY_MISSING_TOOL`
- unknown proprietary level format → `BLOCKED_BY_UNKNOWN_FORMAT`

Never fake completion.

If an asset cannot actually be created, create its complete production specification and prompt instead and mark its status accordingly.

Do not mark anything `GENERATED`, `DEPLOYED`, or `VERIFIED` unless it truly exists and has been checked.

---

# 6. WORK DIRECTORY

Create:

```text
_game_resource_production/
```

Use:

```text
_game_resource_production/
│
├─ 00_sprite_reference/
│  └─ SMALL_KNIGHT_STYLE_SPEC.md
│
├─ 01_source_index/
│  └─ SOURCE_INDEX.md
│
├─ 02_resource_ledger/
│  ├─ C01_C03_RESOURCES.md
│  ├─ C04_C06_RESOURCES.md
│  ├─ C07_C09_RESOURCES.md
│  ├─ C10_C12_RESOURCES.md
│  ├─ C13_C15_RESOURCES.md
│  ├─ C16_C18_RESOURCES.md
│  └─ MASTER_RESOURCE_LEDGER.md
│
├─ 03_maps/
│  ├─ MAP_CATALOG.md
│  ├─ MAP_CONNECTION_GRAPH.md
│  ├─ MAP_REUSE_PLAN.md
│  ├─ MAP_GRID_SPEC.md
│  ├─ map_specs/
│  ├─ blockouts/
│  └─ encounters/
│
├─ 04_characters/
│  ├─ NPC_CLASSIFICATION.md
│  ├─ MAJOR_NPC_LIST.md
│  ├─ GENERIC_NPC_SPEC.md
│  └─ major_npc_specs/
│
├─ 05_sprite_prompts/
│  ├─ HERO_SPRITE_PROMPT.md
│  ├─ MAJOR_NPC_SPRITE_PROMPTS.md
│  └─ GENERIC_NPC_SPRITE_PROMPT.md
│
├─ 06_environment/
│  ├─ ENVIRONMENT_KITS.md
│  ├─ STORY_OBJECTS.md
│  ├─ MAP_ASSET_REQUIREMENTS.md
│  └─ ENVIRONMENT_STATES.md
│
├─ 07_optimization/
│  ├─ SPRITE_STYLE_AUDIT.md
│  ├─ MAP_IMPLEMENTATION_AUDIT.md
│  └─ RESOURCE_REUSE_AUDIT.md
│
└─ 08_final/
   ├─ FINAL_ASSET_MANIFEST.md
   ├─ FINAL_MAP_MANIFEST.md
   ├─ CHAPTER_ASSET_MATRIX.md
   ├─ CHAPTER_MAP_MATRIX.md
   ├─ RESOURCE_PRODUCTION_ORDER.md
   ├─ ASSET_PATH_REGISTRY.md
   └─ MAP_PATH_REGISTRY.md
```

Analysis/specification files belong here.

Actual game assets belong in the project's existing Asset / Map conventions.

Do not mix design documents with game-runtime assets.

---

# 7. PIPELINE

Execute strictly in this order:

```text
PHASE -1  SPRITE MASTER ANALYSIS
PHASE 0   SOURCE INDEX
PHASE 1   PARALLEL RESOURCE EXTRACTION
PHASE 2   MASTER RESOURCE LEDGER
PHASE 3   LOCATION / MAP CONSOLIDATION
PHASE 4   MAP ROLE CLASSIFICATION
PHASE 5   MAP FLOW / BLOCKOUT DESIGN
PHASE 6   GAMEPLAY / ENCOUNTER PLACEMENT
PHASE 7   NPC CLASSIFICATION
PHASE 8   CHARACTER VISUAL DESIGN
PHASE 9   ENVIRONMENT / OBJECT DESIGN
PHASE 10  SPRITE CONSISTENCY AUDIT
PHASE 11  RESOURCE OPTIMIZATION
PHASE 12  ASSET / MAP CREATION
PHASE 13  PROJECT DEPLOYMENT
PHASE 14  IMPLEMENTATION AUDIT
PHASE 15  FINAL MANIFEST
```

Do not start a dependent phase until required outputs from the previous phase exist.

Independent phases may run in parallel when explicitly allowed.

---

# PHASE -1 — SPRITE MASTER ANALYSIS

Use a dedicated Sprite Reference Analyst.

Input:

```text
Simplegame/Asset/smallKnight/
```

Do NOT provide story files.

Inspect actual files.

Determine, when observable:

```text
Frame Width
Frame Height
Sprite Sheet Size
Animation Frame Count
Character Bounding Box
Character Height
Head : Body Ratio
Arm / Leg Ratio
Weapon Size Ratio
Pixel Density
Camera Perspective
Direction Count
Outline Style
Outline Thickness
Shading Steps
Highlight / Shadow Style
Palette Complexity
Face Detail
Eye Detail
Hand / Foot Detail
Armor Detail
Weapon Detail
Idle Structure
Walk Structure
Attack Structure
Hit Structure
Death Structure
Foot Position
Sprite Anchor
Weapon Swing Range
```

If a value cannot be reliably determined:

```text
UNKNOWN
```

Do not estimate it as fact.

Output:

```text
_game_resource_production/
└─ 00_sprite_reference/
   └─ SMALL_KNIGHT_STYLE_SPEC.md
```

---

# 8. SPRITE STYLE CONSTRAINT

Every humanoid sprite must match the master reference in:

```text
Frame Size
Character Scale
Body Proportion
Perspective
Pixel Density
Outline
Shading
Detail Density
Face Detail
Equipment Detail
Animation Structure
Foot Position
Sprite Anchor
```

Characters may differ in:

```text
Silhouette
Hair
Helmet
Armor Shape
Clothing
Cape
Weapon
Color Distribution
Faction Symbol
Pose Details
```

Do not increase visual detail beyond the master pack.

Acceptance test:

> When displayed beside `smallKnight`, the new character should plausibly look as if it came from the same sprite pack.

---

# PHASE 0 — SOURCE INDEX

Inspect:

```text
_story_scene_production/05_final_chapters/
```

Identify:

```text
CHAPTER_01_FINAL.md
...
CHAPTER_18_FINAL.md
```

Also inspect repository structure relevant to:

```text
Simplegame/Asset/
Map/
Maps/
Scene/
Scenes/
Level/
Levels/
Tileset/
Environment/
Background/
```

Record only actual existing paths.

Output:

```text
_game_resource_production/01_source_index/SOURCE_INDEX.md
```

Include:

```text
Story Sources
Sprite Reference
Existing Character Assets
Existing Enemy Assets
Existing Boss Assets
Existing Environment Assets
Existing Map / Level Files
Existing Tilesets
Existing Naming Conventions
Existing Engine / File Formats
Unknowns
Missing Sources
```

Do not redesign anything in this phase.

---

# PHASE 1 — PARALLEL RESOURCE EXTRACTION

Use six independent extraction agents:

```text
Agent A → C01–C03
Agent B → C04–C06
Agent C → C07–C09
Agent D → C10–C12
Agent E → C13–C15
Agent F → C16–C18
```

Each agent receives only its three final chapter files.

Do not provide other chapters unless essential.

Do not ask agents to summarize the story.

Extract production requirements only.

For every chapter collect:

```text
Locations
Map Needs
Characters
NPC Functions
Enemies
Bosses
Important Objects
Environment States
Story Triggers
Gameplay Triggers
Reuse Candidates
```

Output format:

```markdown
## CHAPTER XX

### Locations
- ...

### Map Needs
- ...

### Characters
- ...

### Enemies / Bosses
- ...

### Important Objects
- ...

### Environment States
- ...

### Story / Gameplay Triggers
- ...

### Reuse Candidates
- ...
```

Keep each chapter concise.

Outputs:

```text
C01_C03_RESOURCES.md
C04_C06_RESOURCES.md
C07_C09_RESOURCES.md
C10_C12_RESOURCES.md
C13_C15_RESOURCES.md
C16_C18_RESOURCES.md
```

---

# PHASE 2 — MASTER RESOURCE LEDGER

Merge all six extraction files.

Assign stable IDs:

```text
LOC-001...
MAP-001...
NPC-001...
ENEMY-001...
BOSS-001...
OBJ-001...
TRG-001...
ENV_STATE-001...
ENC-001...
```

Merge duplicate entities.

Do not create separate IDs for the same entity merely because it appears in multiple chapters.

Output:

```text
_game_resource_production/02_resource_ledger/MASTER_RESOURCE_LEDGER.md
```

The Ledger becomes the default input for specialist agents.

After this point, do not reread full story chapters unless a specific ambiguity cannot be resolved from the Ledger.

---

# PHASE 3 — LOCATION / MAP CONSOLIDATION

For every location assign exactly one implementation classification:

```text
PLAYABLE_MAP
PLAYABLE_SUBZONE
CUTSCENE_ONLY
BACKGROUND_ONLY
NOT_REQUIRED
```

Primary rule:

```text
STORY SCENE != MAP
```

Never assume:

```text
1 Scene = 1 Map
```

Merge scenes occurring in the same physical space whenever one coherent map can support them.

Example:

```text
Castle
├─ Entrance
├─ Main Hall
├─ Archive
├─ Underground Waterway
└─ Escape Route
```

may become:

```text
MAP-004_CASTLE
```

with internal Zones.

---

# 9. BASE MAP / VARIANT RULE

When a location changes state, prefer:

```text
BASE MAP
+
VARIANT STATE
```

instead of cloning the whole map.

Example:

```text
MAP-004-A NORMAL
MAP-004-B UNDER_ATTACK
MAP-004-C DAMAGED
```

Variant changes may contain only:

```text
Object State
Enemy Placement
NPC State
VFX
Blocked Routes
Lighting
Story Objects
```

Do not duplicate unchanged geometry.

---

# PHASE 4 — MAP ROLE CLASSIFICATION

Every `PLAYABLE_MAP` gets one primary role:

```text
HUB
LINEAR
BRANCHING
DUNGEON
ARENA
BATTLEFIELD
ESCORT
STEALTH
INVESTIGATION
PUZZLE
SURVIVAL
TRANSITION
BOSS_ARENA
HYBRID
```

Choose role based on player experience, not story terminology.

---

# 10. MAP SIZE

Do not invent arbitrary dimensions.

Determine relative size from:

```text
Expected Playtime
Player Movement Speed
Encounter Count
Exploration Requirement
NPC Count
Landmark Count
Revisit Frequency
Story Urgency
```

If exact values cannot yet be established, use relative categories such as:

```text
SMALL
MEDIUM
LARGE
```

and state why.

---

# 11. GRID / TILE SPEC

First inspect the project.

Determine when possible:

```text
Tile Width
Tile Height
Grid Type
Collision Grid
Character Footprint
Door Width
Corridor Width
Camera View Size
```

If no reliable project convention exists:

```text
MAP_GRID_SPEC_REQUIRED
```

Do not fabricate dimensions.

Record findings in:

```text
_game_resource_production/03_maps/MAP_GRID_SPEC.md
```

---

# 12. MAP STYLE SOURCE

Before creating new map visuals, inspect existing assets under the project.

Prefer existing:

```text
Tilesets
Backgrounds
Environment Assets
Map Files
Level Files
Scene Files
```

Do not invent a new visual language if compatible project resources already exist.

---

# PHASE 5 — MAP FLOW / BLOCKOUT DESIGN

Use a Map Architect Agent.

Input only:

```text
MAP / LOCATION LEDGER
Chapter Function Summary
Gameplay Requirements
Required Objects
Required NPCs
Required Enemies
MAP_GRID_SPEC
Existing Map Conventions
```

Do not provide all story source files.

Create:

```text
MAP_CATALOG.md
MAP_CONNECTION_GRAPH.md
MAP_REUSE_PLAN.md
map_specs/
blockouts/
```

---

# 13. MAP SPEC

Every playable map must use:

```markdown
# MAP-XXX — NAME

## Identity
Map ID:
Name:
Type:
Base / Variant:
Used Chapters:

## Purpose
Why this map must exist.

## Story Functions
Story functions that must occur here.

## Gameplay Functions
Combat / Exploration / Investigation / Escort / Choice / etc.

## Expected Playtime
...

## Entry Points
- ENTRY-01 ...

## Exit Points
- EXIT-01 ...

## Map Connections
- MAP-XXX → MAP-YYY

## Critical Path
ENTRY
→ ...
→ OBJECTIVE
→ EXIT

## Optional Paths
- ...

## Major Zones
- ZONE-01 ...

## Landmarks
- ...

## NPC Placement
- ...

## Enemy / Encounter Placement
- ...

## Interactive Objects
- ...

## Story Objects
- ...

## Trigger Zones
- ...

## Cutscene Triggers
- ...

## Checkpoints
- ...

## Shortcuts
- ...

## Hazards
- ...

## Collision Notes
- ...

## Visibility / Guidance
- ...

## Environment Kit
- ...

## Unique Assets
- ...

## Shared Assets
- ...

## Variants
- ...

## Implementation Notes
- ...
```

---

# 14. ZONE RULE

Divide maps into Zones only when useful.

Example:

```text
ZONE-01 ENTRY
ZONE-02 SAFE / DIALOGUE
ZONE-03 COMBAT
ZONE-04 INVESTIGATION
ZONE-05 MAJOR ENCOUNTER
ZONE-06 EXIT
```

Zones do not need to correspond 1:1 with Story Scenes.

Multiple Story Scenes may share one Zone.

---

# 15. CRITICAL PATH

Every playable map requires a minimum mandatory route:

```text
ENTRY
→ OBJECTIVE A
→ OBJECTIVE B
→ EXIT
```

Include only mandatory story/gameplay progression.

---

# 16. OPTIONAL PATH

Every optional area must provide at least one:

```text
Optional Story Information
Resource Reward
Shortcut
Worldbuilding
Optional Encounter
Foreshadowing
```

Do not create empty exploration space without a purpose.

---

# 17. ENTRY / EXIT / CONNECTION

Every playable map must define:

```text
ENTRY
EXIT
```

Connections use explicit IDs:

```text
MAP-001 EXIT-02
→
MAP-002 ENTRY-01
```

Create:

```text
MAP_CONNECTION_GRAPH.md
```

Distinguish:

```text
Story Chapter Order
```

from:

```text
Physical Map Connectivity
```

---

# 18. REVISIT RULE

Do not create a new map merely because the story revisits a place.

Use:

```text
REVISIT_STATE
```

Example:

```text
Visit 1:
NORMAL

Visit 2:
UNDER_ATTACK

Visit 3:
DESTROYED
```

Represent changed state through Variant Layers wherever possible.

---

# 19. LANDMARK RULE

For major maps, target approximately 1–3 strong landmarks when appropriate.

Landmarks exist for:

```text
Orientation
Memory
Story Identity
Visual Guidance
```

Do not label minor decoration as a landmark.

---

# 20. VISUAL GUIDANCE

Prefer environmental guidance over constant UI markers.

Possible guidance:

```text
Light
Large Architecture
Road Shape
Contrast
Enemy Placement
NPC Position / Gaze
Damage Traces
Environmental Motion
```

---

# 21. CAMERA / VISIBILITY

Use existing camera conventions.

Consider:

```text
Sight Blocking
Corners
Doors
Walls
Height Difference
Background Objects
```

If camera specifications are unknown, mark them unknown.

Do not invent them.

---

# 22. COLLISION

Each map spec must describe at least:

```text
Walkable Area
Blocked Area
Wall
Door
One-way Route
Hazard Collision
Interaction Collision
```

Zone-level descriptions are sufficient until exact coordinates are required.

---

# 23. DOORS / CORRIDORS

Validate against:

```text
Character Footprint
Door Width
Corridor Width
Grid Convention
```

Do not design passage dimensions independently of character scale.

---

# 24. SHORTCUTS

Use shortcuts only when they improve:

```text
Exploration Reward
Revisit Fatigue
Map Mastery
```

A shortcut without a purpose should be removed.

---

# 25. CHECKPOINTS

Only place checkpoints if the project supports them.

Possible locations:

```text
Before Major Encounter
After Long Section
Before Boss
Before Major Story Branch
```

Do not invent a new checkpoint system.

---

# PHASE 6 — ENCOUNTER PLACEMENT

Use an Encounter Planner.

Input:

```text
Map Specs
Enemy Ledger
Boss Ledger
Story Function Summary
```

Do not provide complete story chapters.

Every encounter must have at least one production purpose:

```text
Progression Gate
Resource Pressure
Story Consequence
Enemy Introduction
Mechanical Teaching
Escalation
Boss Setup
```

Do not add combat only to fill space.

---

# 26. ENCOUNTER SPEC

Before exact coordinates are necessary, define:

```text
Encounter ID
Zone
Entry Direction
Wave
Enemy Type
Count
Purpose
```

Example:

```text
ENC-003
Zone: ZONE-04
Entry: NORTH_GATE

Wave 1:
ENEMY-001 x4

Wave 2:
ENEMY-002 x1

Purpose:
retreat pressure
```

---

# 27. BOSS ARENA

For every boss arena inspect:

```text
Arena Shape
Player Mobility
Boss Attack Range
Cover
Hazards
Phase Changes
Entry
Exit
Story Objects
```

Do not make the arena visually or mechanically more confusing than the boss itself.

---

# 28. NPC PLACEMENT

For Major NPCs record:

```text
Map
Zone
Initial Position
Movement Requirement
Dialogue Trigger
Combat Participation
Exit Condition
```

For Generic NPCs, zone-level placement is sufficient:

```text
Crowd Zone
Background Zone
```

unless specific positioning is required.

---

# 29. TRIGGERS

Every Story / Gameplay Trigger gets an ID:

```text
TRG-001
```

Allowed types:

```text
ENTER_ZONE
INTERACT
COMBAT_COMPLETE
ITEM_ACQUIRED
NPC_DIALOGUE
OBJECT_STATE
CUTSCENE
MAP_EXIT
```

Format:

```markdown
## TRG-XXX

Map:
Zone:
Type:

Condition:
...

Action:
...

One Shot:
YES / NO

Story Dependency:
...

Next Trigger:
...
```

---

# 30. CUTSCENE RULE

Do not convert playable events into cutscenes by default.

Every Cutscene Trigger must answer:

> Why must this event be non-interactive?

If there is no convincing reason, prefer gameplay or in-engine interaction.

---

# 31. OBJECTS

Every interactive object records:

```text
Object ID
Map
Zone
Interaction
State
Story Function
```

For important Story Objects determine in this order:

```text
Can Shared Object work?
↓
Can Variant work?
↓
Is Unique Asset necessary?
```

---

# 32. ENVIRONMENTAL STORYTELLING

Where possible, communicate through environment instead of exposition.

Examples:

```text
Destroyed Defensive Line
Blocked Road
Empty Homes
Abandoned Equipment
War Damage
Unusual Seals
```

Do not duplicate the same information excessively across environment and dialogue.

---

# 33. ENVIRONMENT STATES

Use:

```text
ENV_STATE-001 NORMAL
ENV_STATE-002 WAR
ENV_STATE-003 DESTROYED
ENV_STATE-004 CORRUPTED
```

For each state record only changed elements:

```text
Props
VFX
NPCs
Enemies
Blocked Routes
Lighting
Story Objects
```

Implement as:

```text
BASE MAP
+
VARIANT LAYER
```

when supported.

---

# PHASE 7 — NPC CLASSIFICATION

Use only:

```text
MAJOR NPC
GENERIC NPC
```

by default.

## MAJOR NPC

A character qualifies when one or more are significant:

```text
Recurring Presence
Key Choice
Relationship Change
Companion Role
Combat Participation
Boss Role
Faction Representation
Major Twist
Emotional Center
Visual Recognition Requirement
```

Each Major NPC gets a unique sprite specification.

## GENERIC NPC

Examples:

```text
Citizen
Refugee
Passerby
Unnamed Official
Servant
Generic Merchant
Background NPC
One-time Guide
```

Default:

```text
GENERIC_NPC_01
```

All Generic NPCs share this sprite unless a clear gameplay/story reason requires otherwise.

Do not automatically create:

```text
Male Variant
Female Variant
Hair Variant
Profession Variant
```

---

# 34. HERO VISUAL RULE

Hero:

```text
Gender: Female
Class: MAJOR NPC
Unique Sprite: YES
Reference: Simplegame/Asset/smallKnight/
```

Design priority:

```text
Story Role
Combat Role
World
Personality
Faction
Gender
```

Do not significantly alter the established smallKnight body proportions merely to emphasize gender.

---

# PHASE 8 — CHARACTER VISUAL DESIGN

For every Major NPC create:

```markdown
# NPC-XXX — Name

## Classification
MAJOR NPC

## Story Role
...

## Gameplay Role
...

## Sprite Master
Simplegame/Asset/smallKnight/

## Silhouette
...

## Hair / Head
...

## Outfit
...

## Equipment
...

## Signature Feature
...

## Color Role
Primary:
Secondary:
Accent:

## Required Animations
...

## Asset Destination
...

## Reuse
...

## Do Not
...
```

If the master sprite already contains equivalent animation types, reuse its:

```text
Frame Count
Timing
Anchor
General Motion Structure
```

unless a character's gameplay role genuinely requires a difference.

---

# PHASE 9 — ENVIRONMENT / OBJECT DESIGN

Prefer reusable kits.

Examples:

```text
MEDIEVAL_TOWN_KIT
CASTLE_KIT
RUINS_KIT
CAVE_KIT
HELL_KIT
BATTLEFIELD_KIT
```

Do not create a new Tileset simply because a new Chapter begins.

Environment assets use:

```text
GROUND
WALL
STRUCTURE
DECORATION
INTERACTIVE
STORY_OBJECT
HAZARD
BACKGROUND
VFX
```

Outputs:

```text
ENVIRONMENT_KITS.md
STORY_OBJECTS.md
MAP_ASSET_REQUIREMENTS.md
ENVIRONMENT_STATES.md
```

---

# PHASE 10 — SPRITE CONSISTENCY AUDIT

Compare all character specifications and any generated sprites against:

```text
SMALL_KNIGHT_STYLE_SPEC.md
```

Check:

```text
Scale
Frame Dimensions
Body Proportion
Perspective
Pixel Density
Outline
Shading
Detail Density
Foot Position
Anchor
Animation Structure
Equipment Detail
```

Report mismatches in:

```text
SPRITE_STYLE_AUDIT.md
```

Do not approve inconsistent assets.

---

# PHASE 11 — RESOURCE OPTIMIZATION

Use a Resource Optimizer.

Input:

```text
MAP CATALOG
MAP SPECS
NPC CLASSIFICATION
CHARACTER SPECS
OBJECT LIST
ENVIRONMENT ASSETS
ENEMY LIST
BOSS LIST
```

Do not provide full story source.

For every resource ask:

```text
Can this be merged?
Can this be represented as a variant?
Can an existing asset be reused?
Is a unique asset actually necessary?
```

Output:

```text
RESOURCE_REUSE_AUDIT.md
MAP_REUSE_PLAN.md
```

Remove unjustified duplication before production.

---

# PHASE 12 — ASSET / MAP CREATION

Only create assets that have passed:

```text
Specification
Style Audit
Map Audit
Reuse Audit
Capability Gate
```

Every asset must have:

```text
Asset ID
Type
Purpose
Destination
Filename
Related Maps
Related Chapters
Implementation Status
```

Do not generate unapproved extras.

---

# 35. PROJECT ASSET DESTINATION

First detect the existing project structure.

Reuse existing equivalent directories.

Only if no equivalent convention exists, use the following fallback structure:

```text
Simplegame/
└─ Asset/
   ├─ smallKnight/              # READ ONLY
   ├─ Characters/
   │  ├─ Hero/
   │  ├─ MajorNPC/
   │  └─ GenericNPC/
   ├─ Enemies/
   │  ├─ Common/
   │  └─ Elite/
   ├─ Bosses/
   ├─ Environment/
   │  ├─ Town/
   │  ├─ Castle/
   │  ├─ Ruins/
   │  ├─ Cave/
   │  ├─ Hell/
   │  └─ Battlefield/
   ├─ Objects/
   │  ├─ Interactive/
   │  ├─ Story/
   │  └─ Shared/
   └─ VFX/
```

Existing project conventions always take priority over this fallback.

---

# 36. MAP DESTINATION

Detect existing conventions first.

Possible existing structures include:

```text
Map/
Maps/
Scene/
Scenes/
Level/
Levels/
```

If no convention exists, a fallback may be:

```text
Simplegame/
└─ Map/
   ├─ Base/
   ├─ Variant/
   ├─ Data/
   └─ Preview/
```

Do not choose an engine-specific extension until the repository proves which format is used.

---

# 37. NAMING

Inspect current naming convention first.

If none exists, use:

```text
snake_case
```

Examples:

```text
hero_spritesheet.png
generic_npc_01_spritesheet.png
npc_001_name_spritesheet.png
boss_death_king_spritesheet.png
object_hell_gate.png
```

Map extension must follow the detected engine/project format.

---

# 38. COLLISION / FILE CONFLICT SAFETY

Before creating any runtime file, check whether its destination already exists.

If conflict exists:

```text
ASSET_NAME_CONFLICT
```

or:

```text
MAP_NAME_CONFLICT
```

Do not automatically:

```text
DELETE
OVERWRITE
REPLACE
```

Resolve through a non-destructive filename or document the conflict for review.

Never modify `smallKnight`.

---

# PHASE 13 — PROJECT DEPLOYMENT

Deploy only actual generated/approved runtime assets.

Keep specifications under:

```text
_game_resource_production/
```

Keep runtime assets under the actual game project directories.

For each deployed resource update its registry.

---

# 39. ASSET PATH REGISTRY

Create:

```text
_game_resource_production/08_final/ASSET_PATH_REGISTRY.md
```

Format:

```markdown
| Asset ID | Name | Type | Project Path | Maps | Chapters | Status |
|---|---|---|---|---|---|---|
```

---

# 40. MAP PATH REGISTRY

Create:

```text
_game_resource_production/08_final/MAP_PATH_REGISTRY.md
```

Format:

```markdown
| Map ID | Name | Type | Project Path | Chapters | Variants | Status |
|---|---|---|---|---|---|---|
```

---

# 41. STATUS MODEL

Every Map and Asset must have exactly one status:

```text
PLANNED
SPEC_READY
BLOCKOUT_READY
GENERATED
DEPLOYED
VERIFIED
```

Definitions:

`PLANNED`
: requirement identified.

`SPEC_READY`
: production specification complete.

`BLOCKOUT_READY`
: map blockout exists and is reviewed.

`GENERATED`
: actual resource file exists.

`DEPLOYED`
: actual resource is placed into the project.

`VERIFIED`
: deployment and manifest consistency have been checked.

Never advance status without evidence.

Final completion requires:

```text
VERIFIED
```

for every resource expected to be physically implemented.

Resources blocked by tooling must remain at the highest truthful status and be explicitly listed as incomplete.

---

# PHASE 14 — IMPLEMENTATION AUDIT

Use independent deployment auditors.

They do NOT need full story source.

Input:

```text
FINAL_ASSET_MANIFEST
FINAL_MAP_MANIFEST
ASSET_PATH_REGISTRY
MAP_PATH_REGISTRY
MAP SPECS
Actual Project File Inventory
```

Check:

```text
Incorrect Destination
Duplicate Files
Naming Mismatch
Missing Files
smallKnight Modification
Incorrect Variant Separation
Missing Map Connections
Missing Triggers
Missing Required Assets
Manifest / Filesystem Mismatch
Invalid Status
```

Use separate logical audits for:

```text
Asset Deployment
Map Implementation
```

Correct only problems that are safe and unambiguous.

Do not destructively overwrite uncertain files.

---

# PHASE 15 — FINAL MANIFEST

Create:

```text
FINAL_ASSET_MANIFEST.md
FINAL_MAP_MANIFEST.md
CHAPTER_ASSET_MATRIX.md
CHAPTER_MAP_MATRIX.md
RESOURCE_PRODUCTION_ORDER.md
ASSET_PATH_REGISTRY.md
MAP_PATH_REGISTRY.md
```

---

# 42. FINAL MAP MANIFEST

Format:

```markdown
| Map ID | Name | Role | Base / Variant | Chapters | Environment Kit | Implementation Status |
|---|---|---|---|---|---|---|
```

Include only maps actually required after consolidation.

---

# 43. CHAPTER MAP MATRIX

For each Chapter:

```markdown
## CHAPTER XX

Maps:
- MAP-XXX

Variants:
- MAP-XXX-A

Entry:
...

Exit:
...

Critical Story Zones:
- ZONE-...

Required Triggers:
- TRG-...

Required NPC:
- NPC-...

Enemies:
- ENEMY-...

Bosses:
- BOSS-...

Objects:
- OBJ-...
```

---

# 44. CHAPTER ASSET MATRIX

For each Chapter list only assets actually required.

Group by:

```text
Characters
Enemies
Bosses
Environment
Objects
VFX
Shared
```

Use Asset IDs instead of repeated descriptions.

---

# 45. RESOURCE PRODUCTION ORDER

Order production by dependency and reuse value.

Prefer:

```text
Shared Core Assets
→ Shared Environment Kits
→ Generic NPC
→ Reusable Enemy Assets
→ Base Maps
→ Major NPCs
→ Bosses
→ Map Variants
→ Unique Story Objects
→ Chapter-specific VFX
```

Adjust only when repository dependencies require a different order.

---

# 46. MAP BLOCKOUT

When possible, each blockout should communicate:

```text
North / Orientation
Entry
Exit
Critical Path
Optional Path
Zones
Landmarks
NPC Placement
Enemy Placement
Objects
Triggers
```

A blockout is not production artwork.

If visual blockout tooling is unavailable, use an ASCII or structured textual blockout.

Do not claim a textual blockout is an implemented map.

---

# 47. AGENT CONTEXT BUDGET

## Sprite Analyst

Input:

```text
smallKnight files only
```

No story.

## Chapter Extraction Agents

Input:

```text
assigned 3 Chapters only
```

## Map Architect

Input:

```text
Location Ledger
Chapter Function Summary
Gameplay Requirements
Grid / Map Convention
```

No full story by default.

## Character Director

Input:

```text
Character Ledger
SMALL_KNIGHT_STYLE_SPEC
```

## Environment Architect

Input:

```text
Environment / Object Ledger
Map Requirements
Existing Environment Assets
```

## Encounter Planner

Input:

```text
Map Specs
Enemy Ledger
Boss Ledger
```

## Resource Optimizer

Input:

```text
Resource Specs
Map Specs
Reuse Data
```

## Implementation Auditors

Input:

```text
Manifests
Registries
Specs
Actual Filesystem Inventory
```

No full story.

---

# 48. PARALLEL EXECUTION

Initial parallel group:

```text
Agent S → smallKnight Style Analysis

Agent A → C01–C03
Agent B → C04–C06
Agent C → C07–C09
Agent D → C10–C12
Agent E → C13–C15
Agent F → C16–C18
```

After `MASTER_RESOURCE_LEDGER`:

```text
Agent G → Map Architect
Agent H → Character Resource Director
Agent I → Environment / Object Architect
```

After Map Specs:

```text
Agent J → Encounter Planner
Agent K → Sprite Consistency Auditor
Agent L → Resource Optimizer
```

After deployment:

```text
Agent M → Asset Deployment Auditor
Agent N → Map Implementation Auditor
```

Do not launch later agents before their required compressed inputs exist.

---

# 49. MAP QUALITY GATE

Every `PLAYABLE_MAP` must satisfy:

```text
[ ] Purpose defined
[ ] Entry defined
[ ] Exit defined
[ ] Critical Path defined
[ ] Required Story Zones defined
[ ] Optional Space has a reason
[ ] Landmark strategy defined
[ ] NPC Placement defined
[ ] Encounter Placement defined
[ ] Story Objects defined
[ ] Triggers defined
[ ] Collision categories defined
[ ] Connection to next relevant Map defined
[ ] Duplicate location maps eliminated
[ ] Revisit handled through State / Variant where possible
[ ] Environment reuse plan defined
```

If required information is unknown, mark the gate unresolved rather than pretending it passed.

---

# 50. FINAL VERIFICATION GATE

Before declaring the project complete verify:

```text
[ ] C01–C18 locations analyzed
[ ] All PLAYABLE_MAP entries identified
[ ] Story Scenes were not mechanically converted into separate Maps
[ ] Base Maps and Variants separated
[ ] Map Connection Graph exists
[ ] Entry / Exit defined
[ ] Critical Paths defined
[ ] Optional Paths have purposes
[ ] Zones defined where required
[ ] NPC Placement defined
[ ] Encounters defined
[ ] Story / Gameplay Triggers defined
[ ] Collision / Hazards defined
[ ] Landmark / Visual Guidance defined
[ ] Revisit States defined
[ ] Environment Kits consolidated
[ ] Actual smallKnight files analyzed
[ ] smallKnight directory unchanged
[ ] Major NPC specifications follow smallKnight style
[ ] Generic NPC follows smallKnight style
[ ] Hero is female
[ ] Hero has a unique sprite specification
[ ] Generic NPCs default to GENERIC_NPC_01
[ ] Runtime assets use correct project directories
[ ] Runtime maps follow actual project conventions
[ ] Existing assets were not silently overwritten
[ ] ASSET_PATH_REGISTRY matches filesystem
[ ] MAP_PATH_REGISTRY matches filesystem
[ ] Manifest status matches real implementation state
```

Do not mark the overall production as fully complete unless every required deployable item is actually `VERIFIED`.

---

# 51. FAILURE HANDLING

## Missing Story Chapter

Mark:

```text
MISSING_STORY_SOURCE
```

Do not infer its resource requirements.

## Missing Sprite Reference

Mark:

```text
SPRITE_REFERENCE_MISSING
```

Do not invent master style measurements.

## Unknown Map Format

Mark:

```text
MAP_FORMAT_UNKNOWN
```

Create Map Specs and blockouts only.

Do not create arbitrary engine files.

## Missing Grid Convention

Mark:

```text
MAP_GRID_SPEC_REQUIRED
```

## Existing Filename Conflict

Mark:

```text
ASSET_NAME_CONFLICT
```

or:

```text
MAP_NAME_CONFLICT
```

Do not overwrite.

## Unsupported Asset Generation

Mark:

```text
BLOCKED_BY_MISSING_TOOL
```

Create a production-ready specification and generation prompt instead.

## Specialist Agent Conflict

The Director resolves it using:

```text
actual project files
Master Resource Ledger
Map Specs
Style Spec
reuse priority
```

Return to story source only if the conflict cannot otherwise be resolved.

---

# 52. FINAL DIRECTIVE

Always maintain this production chain:

```text
FINAL STORY
↓
RESOURCE EXTRACTION
↓
LOCATION CONSOLIDATION
↓
MAP DESIGN
↓
GAMEPLAY PLACEMENT
↓
ASSET SPECIFICATION
↓
RESOURCE OPTIMIZATION
↓
CREATION
↓
PROJECT DEPLOYMENT
↓
VERIFICATION
```

For Maps:

```text
Story Scene
↓
Physical Location
↓
Base Map
↓
Zone
↓
Gameplay
↓
Variant / Revisit State
```

For character graphics:

```text
Major NPC
→ Unique Sprite
→ smallKnight Style

Generic NPC
→ GENERIC_NPC_01
→ Reuse
```

For all resources:

```text
CREATE ONCE
↓
REUSE
↓
VARIANT
↓
UNIQUE ONLY WHEN NECESSARY
```

Never optimize for the number of generated files.

Optimize for the smallest coherent resource set that fully implements the finalized story and remains consistent with the actual game project.