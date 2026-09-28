# MAP_GRID_SPEC

## Existing Project Convention
Source inspected: TutorialLayout / LevelMap / LevelTypes.

- World coordinate type: floating-point X/Y WorldPosition.
- Procedural prop cell spacing: 128 world units.
- Render chunk size: 512 world units.
- Current player collision radius used by map collision: 11 world units.
- Procedural obstacle blocked radius including player clearance: 44 world units.
- Current authored road half-widths: approximately 48–80 world units.
- Mine corridor/path half-widths in current prototype: approximately 48–60 world units.
- River crossing and mine walkable tests inset by player radius 11.

## Important Distinction
The project currently does NOT use a tilemap grid as the authoritative level representation. Spatial topology is authored in C++ through TutorialLayout and LevelMap. Therefore:
- Tile Width: NOT APPLICABLE / NOT CONFIRMED.
- Tile Height: NOT APPLICABLE / NOT CONFIRMED.
- Grid Type: free-form world coordinates + 128-unit procedural prop cells.
- Collision Grid: analytic/world-space collision, not a confirmed tile collision grid.
- Character Footprint: radius 11 in current prototype implementation.
- Door Width: not globally standardized.
- Corridor Width: current prototype paths imply roughly 96–120 total walkable width before player inset, but this is implementation precedent, not story canon.
- Camera View Size: NOT CONFIRMED from the inspected map files.

## C01 Rule
Use current world-coordinate and collision conventions for blockout compatibility, but do not freeze exact map dimensions or door widths until the latest Chapter 1 map is implemented against current movement/camera behavior.
