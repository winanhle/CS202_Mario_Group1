# Map Editor Game Mode — Implementation Plan

A new in-game map editor that lets players design custom levels using the game's tilesets, place enemies/objects, then immediately test-play the result.

---

## Background & Architecture Context

The existing codebase already has key scaffolding ready:
- `IMapManager` already declares `editTile()`, `undoEdit()`, `redoEdit()`, `saveToTMX()`, `addEnemySpawn()`, `removeEnemySpawn()`.
- `MapManager` already implements these (undo/redo stacks via `TileEdit` Command pattern).
- `GameConfig::customMapPath` already allows `PlayState` to load a custom map path.
- `GameState` base class handles the state machine cleanly.

---

## SOLID Design Principles Applied

| Principle | How |
|---|---|
| **SRP** | Each class has one job: `EditorTilePalette` = tile selection; `EditorCanvas` = draw/erase; `EditorToolbar` = tools/actions; `EditorEntityPlacer` = enemy spawns |
| **OCP** | New tool types added by subclassing `IEditorTool` without touching canvas logic |
| **LSP** | All tools are interchangeable via `IEditorTool*` |
| **ISP** | Editor-specific map API kept in `IMapManager` already as a separate section; editor state never calls game-only APIs |
| **DIP** | `MapEditorState` talks to `IMapManager*`, not `MapManager*` |

---

## New Files Overview

```
src/states/
  MapEditorState.h / .cpp     ← the main editor GameState

src/editor/
  IEditorTool.h               ← abstract tool interface (ISP/OCP)
  DrawTool.h / .cpp           ← place tile under cursor
  EraseTool.h / .cpp          ← erase tile (set to EMPTY)
  FillTool.h / .cpp           ← flood-fill region
  EntityPlaceTool.h / .cpp    ← place/remove enemy spawn markers
  EditorTilePalette.h / .cpp  ← scrollable tile picker from TSX
  EditorToolbar.h / .cpp      ← toolbar buttons (save, load, play, undo, redo)
  EditorCanvas.h / .cpp       ← handles grid view, camera pan, hover highlight
  EditorMapSerializer.h / .cpp← save/load custom maps (wraps IMapManager::saveToTMX)
```

---

## Proposed Changes

---

### New: Menu Entry Point

#### [MODIFY] [MenuState.cpp](file:///e:/CS202_Mario_Group1/src/states/MenuState.cpp)
- Add a **"Map Editor"** button to the main menu.
- On press, transition to `MapEditorState` (passing `m_settings` and `m_saveManager`).

---

### Editor Infrastructure

#### [NEW] `src/editor/IEditorTool.h`
```cpp
class IEditorTool {
public:
    virtual ~IEditorTool() = default;
    virtual void onMousePress  (int gx, int gy, IMapManager&) = 0;
    virtual void onMouseDrag   (int gx, int gy, IMapManager&) = 0;
    virtual void onMouseRelease(int gx, int gy, IMapManager&) = 0;
    virtual std::string getName() const = 0;
};
```

#### [NEW] `src/editor/DrawTool.h/.cpp`
- Holds a `TileType m_paintType` selected from the palette.
- `onMousePress` / `onMouseDrag` → calls `map.editTile(gx, gy, m_paintType)`.

#### [NEW] `src/editor/EraseTool.h/.cpp`
- Calls `map.editTile(gx, gy, TileType::EMPTY)`.

#### [NEW] `src/editor/FillTool.h/.cpp`
- BFS / flood-fill that calls `editTile()` on every contiguous matching cell — each individual call recorded to the undo stack.

#### [NEW] `src/editor/EntityPlaceTool.h/.cpp`
- On press: opens an inline entity selector (Goomba, Koopa, BuzzyBeetle, etc.).
- On confirm: calls `map.addEnemySpawn(EntitySpawnData{type, worldX, worldY})`.
- On right-click: removes nearest spawn within snap radius via `map.removeEnemySpawn(index)`.
- Renders spawn markers as colored circles with a label.

---

### Palette

#### [NEW] `src/editor/EditorTilePalette.h/.cpp`
- Loads `world1_1.tsx`, `world1_3.tsx`, `world1_4.tsx` (and their PNGs) at construction.
- Builds a flat list of `{gid, TileType, textureRect}` for each tile with a known `type` property (excludes pure `EMPTY`/`BACKGROUND` decorative tiles unless opted in).
- Renders as a scrollable vertical strip in a fixed right-side panel (200px wide).
- Clicking a tile: selects `TileType` and tells `DrawTool` what to paint.

```
OCP: Adding a new TSX is just appending to a list; no palette logic changes.
```

---

### Canvas

#### [NEW] `src/editor/EditorCanvas.h/.cpp`
- Owns the world-space viewport: `sf::View m_editorView`.
- Middle-mouse drag or **WASD** pans the camera.
- Scroll-wheel zooms in/out.
- Converts `sf::Mouse::getPosition(window)` → grid `(gx, gy)` via the view transform.
- Renders:
  1. All map tiles via `IMapManager::render()`.
  2. Grid overlay lines.
  3. Hovered cell highlight (yellow outline).
  4. Enemy spawn markers (from `EntitySpawnData` list).
  5. PlayerSpawn marker (orange diamond).

---

### Toolbar

#### [NEW] `src/editor/EditorToolbar.h/.cpp`
- A horizontal strip at the top of the screen (32px tall).
- Buttons: **Draw | Erase | Fill | Entity | Undo | Redo | Save | Load | Play | Back**
- Uses `sf::Text` + `sf::RectangleShape`.
- Issues callbacks to `MapEditorState` when clicked.
- Displays the currently selected tool and tile name.

---

### Serializer

#### [NEW] `src/editor/EditorMapSerializer.h/.cpp`

SRP-focused wrapper around `IMapManager::saveToTMX()`:

```cpp
class EditorMapSerializer {
public:
    // Returns absolute path of saved file; "" on failure
    std::string saveCustomMap(IMapManager& map, const std::string& baseName);

    // Returns "" on success, error string on failure
    std::string loadIntoMap(IMapManager& map, const std::string& path);

    // Returns list of previously saved custom map paths
    std::vector<std::string> listCustomMaps() const;
};
```
- Saves into `assets/map/custom/` directory.
- Ensures a `PlayerSpawn` object is injected at a default position if none exists.

---

### Main Editor State

#### [NEW] `src/states/MapEditorState.h/.cpp`

```cpp
class MapEditorState : public GameState {
public:
    explicit MapEditorState(std::shared_ptr<ISettingsManager> settings,
                            std::shared_ptr<ISaveManager>     saveManager);
    void handleInput(const sf::Event& event) override;
    void update(float deltaTime)             override;
    void render(sf::RenderWindow& window)    const override;

private:
    void onPlay();         // save + launch PlayState with customMapPath
    void onSave();
    void onLoad();
    void onUndo();
    void onRedo();
    void onToolSelected(std::unique_ptr<IEditorTool> tool);

    std::shared_ptr<IMapManager>  m_mapManager;
    std::unique_ptr<EditorCanvas> m_canvas;
    EditorTilePalette             m_palette;
    EditorToolbar                 m_toolbar;
    EditorMapSerializer           m_serializer;

    std::unique_ptr<IEditorTool>  m_activeTool;
    std::string                   m_currentMapPath;

    sf::Font m_font;
    std::shared_ptr<ISettingsManager> m_settings;
    std::shared_ptr<ISaveManager>     m_saveManager;
};
```

**Key flow: Play**
1. `onPlay()` → `m_serializer.saveCustomMap(*m_mapManager, "custom_temp")` → gets path.
2. Builds `GameConfig{.customMapPath = path}`.
3. Calls `getStateManager()->changeState(std::make_unique<CharacterSelectState>(config, ...))`.
4. `PlayState` picks up `customMapPath` from `GameConfig` and loads it directly.

> [!NOTE]
> This reuses the existing `GameConfig::customMapPath` field which `PlayState` already respects, so no PlayState changes are needed.

---

### Minor Additions

#### [MODIFY] [MapManager.cpp](file:///e:/CS202_Mario_Group1/src/entities/map/MapManager.cpp)
- Implement `editTile()`, `undoEdit()`, `redoEdit()`, `saveToTMX()`, `addEnemySpawn()`, `removeEnemySpawn()` if they are currently stubs.
- `saveToTMX()` writes a well-formed TMX (with CSV layer + `<objectgroup>` for player/enemy spawns) compatible with the existing `loadMapTMX()`.

#### [MODIFY] [GameConfig.h](file:///e:/CS202_Mario_Group1/src/core/GameConfig.h) *(if needed)*
- No changes needed; `customMapPath` field already exists.

---

## UI Layout Sketch

```
┌──────────────────────────────────────────────────────────────────┬──────────┐
│  [Draw] [Erase] [Fill] [Entity]   [Undo] [Redo]   [Save] [Load] [Play] [←] │  Toolbar (32px)
├──────────────────────────────────────────────────────────────────┼──────────┤
│                                                                  │ PALETTE  │
│                    MAP CANVAS                                    │          │
│   (scroll with WASD/middle-mouse, zoom with scroll wheel)        │  tiles   │
│   Grid lines + hover highlight + spawn markers                   │  list    │
│                                                                  │ (scroll) │
└──────────────────────────────────────────────────────────────────┴──────────┘
```

---

## Open Questions

> [!IMPORTANT]
> **Q1**: Should the editor start with a blank 200×16 grid (new map), or let the player choose to start from one of the existing campaign stages?

> [!IMPORTANT]
> **Q2**: Should the player always go through Character Select → Mode Select after pressing Play, or directly jump into 1P Mario with the custom map?

> [!NOTE]
> **Q3**: Which tile types should be visible in the palette? (All, or only "gameplay-meaningful" ones like GROUND, BRICK, QUESTION_COIN, PIPE, ENEMY spawns?)

> [!NOTE]
> **Q4**: Should previously saved custom maps appear in the existing `LevelSelectState` listing? (`discoverMaps()` already scans the map directory, so they'd appear automatically.)

---

## Verification Plan

### Automated Tests
- No automated tests currently; manual integration verification.

### Manual Verification
1. From main menu, navigate to **Map Editor**.
2. Select tiles from the palette and draw on the grid.
3. Use **Erase** and **Fill** tools.
4. Place a **PlayerSpawn** marker and enemy spawns.
5. **Undo / Redo** several operations correctly.
6. **Save** → file appears in `assets/map/custom/`.
7. **Play** → Character Select appears → game launches the custom map.
8. Complete or die → return to editor.
9. **Load** → reopen saved map back in editor.
