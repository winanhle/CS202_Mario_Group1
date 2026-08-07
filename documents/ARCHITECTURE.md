# Mario Game - Team Development Architecture

## Overview

This document describes the modular architecture designed for team-based development on the Mario game project. The architecture enables 4 developers to work independently on different modules with minimal merge conflicts and clear extension points.

---

## Team Structure & Responsibilities

| Developer | Responsibility | Module | Files |
|-----------|-----------------|--------|-------|
| **Quynh Anh** | Architect / Integration Lead | Core Game Loop, StateManager, PlayState | `core/`, `states/` |
| **Le Tran** | Character Implementation | Player/Mario character | `entities/PlayerManager.h/.cpp` |
| **Dinh Anh** | Game Content | Enemies, Items, Levels | `entities/EnemyManager.h/.cpp`, `entities/ItemManager.h/.cpp` |
| **Nguyen Phuc** | UI & Persistence | HUD, UI, Save/Load | `ui/HUDManager.h/.cpp`, `ui/SaveManager.h/.cpp` |

---

## Architecture Diagram

```
???????????????????????????????????????????????????????????????
?                        Game (core/Game.h)                   ?
?                    - Owns SFML Window                       ?
?                    - Owns StateManager                      ?
?                    - Main Loop: 60 FPS                      ?
???????????????????????????????????????????????????????????????
                         ?
                         ?
???????????????????????????????????????????????????????????????
?                  StateManager (core/)                       ?
?              - Manages Game States                          ?
?              - Delegates to Active State                    ?
???????????????????????????????????????????????????????????????
                         ?
         ?????????????????????????????????
         ?               ?               ?
         ?               ?               ?
    MenuState        PlayState      PauseState    GameOverState
    [Blue Box]       [Central]      [Yellow Box]  [Red Box]
                         ?
                         ?
          ????????????????????????????????
          ?      GameWorld (world/)      ?
          ?  - Central Coordinator       ?
          ?  - Module Mediator           ?
          ?  - Extension Points          ?
          ????????????????????????????????
                 ?       ?        ?
         ??????????????????????????????????
         ?          ?       ?             ?
         ?          ?       ?             ?
    PlayerMgr   EnemyMgr  ItemMgr     HUDMgr    SaveMgr
    [Le Tran]   [Dinh]    [Dinh]    [Nguyen]   [Nguyen]
    [Red Box]   [Yellow]  [Magenta] [Cyan]     [Abstract]
```

---

## Core Principles

### 1. **Dependency Inversion**
- High-level modules (Game, StateManager) do NOT depend on low-level modules (PlayerManager, EnemyManager)
- Both depend on abstractions (IPlayerManager, IEnemyManager)
- This allows modules to be developed independently

### 2. **Module Isolation**
- Each module communicates ONLY through GameWorld
- No direct coupling between PlayerManager, EnemyManager, ItemManager, etc.
- Each module can be tested independently

### 3. **Extension Points**
- GameWorld has setter methods for each manager interface
- Developers implement their interfaces and pass instances to GameWorld
- No need to modify GameWorld, PlayState, or StateManager

### 4. **Ownership & Lifecycle**
- PlayState owns GameWorld (unique_ptr)
- GameWorld owns all manager interfaces (shared_ptr)
- Managers are optional (can be nullptr)
- Clean destruction cascade

---

## Module Communication Flow

### Update Cycle (Per Frame)
```
Game.run() [60 FPS]
  ?? handleEvents()
  ?   ?? StateManager.handleInput()
  ?       ?? PlayState.handleInput()
  ?           ?? GameWorld.handleInput()
  ?               ?? PlayerManager.handleInput()
  ?               ?? HUDManager.handleInput()
  ?
  ?? update(deltaTime)
  ?   ?? StateManager.update(deltaTime)
  ?       ?? PlayState.update(deltaTime)
  ?           ?? GameWorld.update(deltaTime)
  ?               ?? EnemyManager.update()
  ?               ?? ItemManager.update()
  ?               ?? PlayerManager.update()
  ?               ?? HUDManager.update()
  ?
  ?? render()
      ?? StateManager.render(window)
          ?? PlayState.render(window)
              ?? GameWorld.render(window)
                  ?? EnemyManager.render()
                  ?? ItemManager.render()
                  ?? PlayerManager.render()
                  ?? HUDManager.render()
```

### Inter-Module Communication (Query Pattern)
```
Example: HUDManager needs player score

HUDManager:
  IPlayerManager* player = gameWorld->getPlayerManager();
  if (player) {
      int score = player->getScore();
      hudManager->updateScore(score);
  }
```

---

## Ownership Model

### PlayState
```
PlayState
?? owns: GameWorld (unique_ptr)
?? responsibility: Create GameWorld, set up modules, delegate game loop
```

### GameWorld
```
GameWorld
?? owns: IPlayerManager (shared_ptr)
?? owns: IEnemyManager (shared_ptr)
?? owns: IItemManager (shared_ptr)
?? owns: IHUDManager (shared_ptr)
?? owns: ISaveManager (shared_ptr)
?? responsibility: Coordinate modules, provide interfaces for queries
```

### Module Managers (PlayerManager, EnemyManager, etc.)
```
Each Manager
?? owns: Game-specific data (entities, state)
?? implements: Module interface
?? responsibility: Handle module-specific logic
```

---

## File Structure

```
src/
??? core/                          [QUYNH ANH - DO NOT MODIFY]
?   ??? Game.h
?   ??? Game.cpp
?   ??? GameState.h                [Abstract base]
?   ??? StateManager.h
?   ??? StateManager.cpp
?
??? states/                         [QUYNH ANH - DO NOT MODIFY]
?   ??? MenuState.h/.cpp
?   ??? PlayState.h/.cpp           [Quynh integrates modules here]
?   ??? PauseState.h/.cpp
?   ??? GameOverState.h/.cpp
?
??? interfaces/                    [TEAM - Read only, contracts]
?   ??? IPlayerManager.h           [Le Tran implements]
?   ??? IEnemyManager.h            [Dinh Anh implements]
?   ??? IItemManager.h             [Dinh Anh implements]
?   ??? IHUDManager.h              [Nguyen Phuc implements]
?   ??? ISaveManager.h             [Nguyen Phuc implements]
?
??? world/                         [QUYNH ANH - Central coordinator]
?   ??? GameWorld.h
?   ??? GameWorld.cpp
?
??? entities/                      [TEAM - Individual implementations]
?   ??? PlayerManager.h/.cpp       [LE TRAN]
?   ??? EnemyManager.h/.cpp        [DINH ANH]
?   ??? ItemManager.h/.cpp         [DINH ANH]
?
??? ui/                            [TEAM - UI implementations]
    ??? HUDManager.h/.cpp          [NGUYEN PHUC]
    ??? SaveManager.h/.cpp         [NGUYEN PHUC]
```

---

## Extension Points

### 1. Adding a Player Feature (Le Tran)

**Location:** `src/entities/PlayerManager.h`

```cpp
class PlayerManager : public IPlayerManager
{
    // Implement interface:
    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    void handleInput(const sf::Event& event) override;

    // Add player-specific methods:
    void jump();
    void moveLeft();
    void moveRight();
};
```

**Integration:** Already integrated in PlayState constructor
```cpp
m_gameWorld->setPlayerManager(std::make_shared<PlayerManager>());
```

**Query by other modules:**
```cpp
IPlayerManager* player = m_gameWorld->getPlayerManager();
float x = player->getPositionX();
float y = player->getPositionY();
```

### 2. Adding Enemy Feature (Dinh Anh - Enemies)

**Location:** `src/entities/EnemyManager.h`

```cpp
class EnemyManager : public IEnemyManager
{
    // Implement interface:
    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    // Add enemy-specific methods:
    void spawnEnemy(float x, float y);
    bool checkCollision(float x, float y);
};
```

### 3. Adding Item Feature (Dinh Anh - Items)

**Location:** `src/entities/ItemManager.h`

```cpp
class ItemManager : public IItemManager
{
    // Implement interface:
    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;

    // Add item-specific methods:
    void spawnItem(float x, float y);
    bool checkCollection(float x, float y);
};
```

### 4. Adding HUD Feature (Nguyen Phuc - UI)

**Location:** `src/ui/HUDManager.h`

```cpp
class HUDManager : public IHUDManager
{
    // Implement interface:
    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    void handleInput(const sf::Event& event) override;

    // Add HUD-specific methods:
    void drawScore();
    void drawLives();
    void showGameOver();
};
```

### 5. Adding Save/Load Feature (Nguyen Phuc - Save System)

**Location:** `src/ui/SaveManager.h`

```cpp
class SaveManager : public ISaveManager
{
    // Implement interface:
    void initialize() override;
    bool saveGame() override;
    bool loadGame() override;
    bool hasSaveFile() const override;

    // Add save-specific methods:
    void serializeGameState();
    void deserializeGameState();
};
```

---

## Module Interface Contracts

### IPlayerManager
**Implemented by:** Le Tran
**Required methods:**
- `initialize()` - Setup player
- `update(deltaTime)` - Update player physics/state
- `render(window)` - Draw player
- `handleInput(event)` - Process input
- `isAlive()` - Get alive status
- `getScore()` - Query score
- `getLives()` - Query lives
- `getPositionX()` / `getPositionY()` - Query position

### IEnemyManager
**Implemented by:** Dinh Anh
**Required methods:**
- `initialize()` - Spawn initial enemies
- `update(deltaTime)` - Update enemy AI
- `render(window)` - Draw enemies
- `getEnemyCount()` - Query enemy count

### IItemManager
**Implemented by:** Dinh Anh
**Required methods:**
- `initialize()` - Spawn initial items
- `update(deltaTime)` - Update items
- `render(window)` - Draw items
- `getItemCount()` - Query item count

### IHUDManager
**Implemented by:** Nguyen Phuc
**Required methods:**
- `initialize()` - Setup UI
- `update(deltaTime)` - Update UI animations
- `render(window)` - Draw UI
- `handleInput(event)` - Handle menu clicks
- `updateScore(score)` - Update displayed score
- `updateLives(lives)` - Update displayed lives
- `updateEnemyCount(count)` - Update enemy display

### ISaveManager
**Implemented by:** Nguyen Phuc
**Required methods:**
- `initialize()` - Check for save files
- `saveGame()` - Save current state
- `loadGame()` - Load saved state
- `hasSaveFile()` - Check if save exists

---

## Development Guidelines

### DO NOT Modify
- `src/core/Game.h` and `Game.cpp` - Quynh Anh maintains the game loop
- `src/core/StateManager.h` and `.cpp` - Quynh Anh manages states
- `src/core/GameState.h` - Base class is stable
- `src/states/PlayState.h` and `.cpp` - Integration point, Quynh maintains
- `src/world/GameWorld.h` and `.cpp` - Quynh maintains coordinator

### CAN Modify (Follow Your Section)
- `src/entities/PlayerManager.h/.cpp` - **Le Tran implements**
- `src/entities/EnemyManager.h/.cpp` - **Dinh Anh implements**
- `src/entities/ItemManager.h/.cpp` - **Dinh Anh implements**
- `src/ui/HUDManager.h/.cpp` - **Nguyen Phuc implements**
- `src/ui/SaveManager.h/.cpp` - **Nguyen Phuc implements**

### Interface Implementation Rules
1. DO NOT modify interface files (`src/interfaces/I*.h`)
2. DO implement all required methods from your interface
3. DO follow the interface contract exactly
4. DO add additional public methods as needed
5. DO add private helper methods as needed

---

## Communication Protocol

### Manager-to-Manager Communication

**CORRECT (via GameWorld):**
```cpp
// In PlayerManager::update()
IEnemyManager* enemies = m_gameWorld->getEnemyManager();
if (enemies && enemies->getEnemyCount() > 0) {
    // Handle collision with enemy
}
```

**WRONG (direct coupling):**
```cpp
// DO NOT DO THIS - Creates direct dependency
EnemyManager* enemies = static_cast<EnemyManager*>(...);
```

### HUD Updates

**Pattern:** GameWorld calls HUD update methods

```cpp
// In GameWorld::update()
if (m_hudManager) {
    IPlayerManager* player = m_playerManager.get();
    if (player) {
        m_hudManager->updateScore(player->getScore());
        m_hudManager->updateLives(player->getLives());
    }
    IEnemyManager* enemies = m_enemyManager.get();
    if (enemies) {
        m_hudManager->updateEnemyCount(enemies->getEnemyCount());
    }
}
```

---

## Compilation & Build

The project uses CMake with Ninja generator:

```bash
mkdir build
cd build
cmake .. -G Ninja
ninja
```

All source files are listed in `CMakeLists.txt`:
```cmake
add_executable(Mario
    src/main.cpp
    src/core/Game.cpp
    src/core/StateManager.cpp
    src/states/MenuState.cpp
    src/states/PlayState.cpp
    src/states/PauseState.cpp
    src/states/GameOverState.cpp
    src/world/GameWorld.cpp
    src/entities/PlayerManager.cpp
    src/entities/EnemyManager.cpp
    src/entities/ItemManager.cpp
    src/ui/HUDManager.cpp
    src/ui/SaveManager.cpp
)
```

**When adding new files:**
1. Add `.cpp` file to CMakeLists.txt `add_executable()` section
2. No need to add headers (CMake will find them via includes)
3. Rebuild: `ninja`

---

## Current Status

### ? Completed (Stable)
- Core game loop (Game, StateManager)
- Game states (Menu, Play, Pause, GameOver)
- GameWorld coordinator
- All module interfaces defined
- Stub implementations for all modules
- Module integration points

### ?? In Progress (Team Development)
- Player module - Le Tran
- Enemy module - Dinh Anh
- Item module - Dinh Anh
- HUD module - Nguyen Phuc
- Save/Load module - Nguyen Phuc

### ?? TODO Markers
Each stub implementation contains TODO comments showing where actual logic goes. Search for "TODO:" to find:
- `TODO: Le Tran` - Player implementation tasks
- `TODO: Dinh Anh` - Enemy/Item implementation tasks
- `TODO: Nguyen Phuc` - UI/Save implementation tasks
- `TODO: Quynh Anh` - Integration/Architecture tasks

---

## FAQ

**Q: Can I call methods from another manager directly?**
A: NO. Always use the interface and go through GameWorld.getXxxManager()

**Q: What if I need data from another module?**
A: Add query methods to that module's interface and call them through GameWorld.

**Q: Can I modify the interface?**
A: NO. Interfaces are contracts. If you need new methods, add them as new public methods to your implementation (not in the interface).

**Q: What if two modules need to communicate a lot?**
A: Use GameWorld.update() to coordinate. Add necessary query methods to each interface.

**Q: How do I test my module independently?**
A: Each module can be tested by:
1. Creating an instance
2. Calling initialize()
3. Calling update() and render() in a test loop
4. No need to instantiate the entire game

**Q: What about collision detection?**
A: This is handled during update phase. PlayerManager queries EnemyManager via GameWorld to check collisions.

---

## Next Steps

1. **Le Tran**: Implement PlayerManager with Mario character logic
2. **Dinh Anh**: Implement EnemyManager and ItemManager with game content
3. **Nguyen Phuc**: Implement HUDManager and SaveManager with UI and persistence
4. **Quynh Anh**: Integrate state transitions and handle edge cases

Each developer should follow the extension points provided and implement their interface contract completely.
