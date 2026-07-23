# Quick Reference - Module Integration Points

## Current Status: ? Ready for Team Development

The project is fully set up with stub implementations. Each module is compilable and ready for your implementation.

---

## Your Task (Based on Role)

### ?? Le Tran - Player/Mario
**File:** `src/entities/PlayerManager.h` / `.cpp`

**What to implement:**
```cpp
class PlayerManager : public IPlayerManager {
    // Player character logic
    // Movement, jumping, gravity
    // Input handling
    // Collision detection with enemies/items
};
```

**Extension points:**
```cpp
// Query enemies
IEnemyManager* enemies = m_gameWorld->getEnemyManager();

// Query items  
IItemManager* items = m_gameWorld->getItemManager();

// Update HUD
IHUDManager* hud = m_gameWorld->getHUDManager();
hud->updateScore(m_score);
hud->updateLives(m_lives);
```

---

### ?? Dinh Anh - Enemies & Items
**Files:** 
- `src/entities/EnemyManager.h` / `.cpp`
- `src/entities/ItemManager.h` / `.cpp`

**What to implement:**
```cpp
class EnemyManager : public IEnemyManager {
    // Enemy AI and movement
    // Spawn/despawn logic
    // Collision detection
};

class ItemManager : public IItemManager {
    // Item spawning and physics
    // Collection detection
    // Item types (coins, powerups, etc)
};
```

**Extension points:**
```cpp
// Query player position
IPlayerManager* player = m_gameWorld->getPlayerManager();
float playerX = player->getPositionX();
float playerY = player->getPositionY();

// Update HUD
IHUDManager* hud = m_gameWorld->getHUDManager();
hud->updateEnemyCount(m_enemyCount);
```

---

### ?? Nguyen Phuc - UI & Save System
**Files:**
- `src/ui/HUDManager.h` / `.cpp`
- `src/ui/SaveManager.h` / `.cpp`

**What to implement:**
```cpp
class HUDManager : public IHUDManager {
    // HUD rendering (score, lives, etc)
    // Menu UI
    // Text rendering with fonts
};

class SaveManager : public ISaveManager {
    // Game state serialization
    // File I/O
    // Load/save logic
};
```

**Extension points:**
```cpp
// Receive updates from GameWorld
void HUDManager::updateScore(int score) override;
void HUDManager::updateLives(int lives) override;
void HUDManager::updateEnemyCount(int count) override;
```

---

### ??? Quynh Anh - Architecture/Integration
**Files:** (READ ONLY - MAINTAIN ONLY)
- `src/core/Game.h` / `.cpp`
- `src/core/StateManager.h` / `.cpp`
- `src/states/PlayState.h` / `.cpp`
- `src/world/GameWorld.h` / `.cpp`

**What to do:**
- ? Modules are already integrated in PlayState
- ? GameWorld is set up as coordinator
- Implement state transitions when modules are ready
- Fix build/link issues as team develops

---

## File Ownership Map

```
? STABLE (DO NOT MODIFY)
??? src/core/Game.h/.cpp                [Quynh Anh]
??? src/core/GameState.h                [Quynh Anh]
??? src/core/StateManager.h/.cpp        [Quynh Anh]
??? src/states/MenuState.h/.cpp         [Quynh Anh]
??? src/states/PlayState.h/.cpp         [Quynh Anh - INTEGRATION POINT]
??? src/states/PauseState.h/.cpp        [Quynh Anh]
??? src/states/GameOverState.h/.cpp     [Quynh Anh]
??? src/world/GameWorld.h/.cpp          [Quynh Anh]
??? src/interfaces/I*.h                 [TEAM - READ ONLY]

?? IMPLEMENTATION (YOUR SECTION)
??? src/entities/PlayerManager.h/.cpp   [LE TRAN] ??
??? src/entities/EnemyManager.h/.cpp    [DINH ANH] ??
??? src/entities/ItemManager.h/.cpp     [DINH ANH] ??
??? src/ui/HUDManager.h/.cpp            [NGUYEN PHUC] ??
??? src/ui/SaveManager.h/.cpp           [NGUYEN PHUC] ??
```

---

## How to Find TODO Items

Search your code for tasks:

```bash
# Find your tasks
grep -r "TODO: Le Tran" src/           # Find all Le Tran tasks
grep -r "TODO: Dinh Anh" src/          # Find all Dinh Anh tasks
grep -r "TODO: Nguyen Phuc" src/       # Find all Nguyen Phuc tasks
grep -r "TODO: Quynh Anh" src/         # Find all Quynh Anh tasks
```

---

## Module Communication Examples

### Example 1: Player Takes Damage from Enemy
```cpp
// In PlayerManager::update()
IEnemyManager* enemies = m_gameWorld->getEnemyManager();
if (enemies && checkCollisionWithEnemies()) {
    m_lives--;
    m_isAlive = (m_lives > 0);

    // Update HUD
    IHUDManager* hud = m_gameWorld->getHUDManager();
    if (hud) {
        hud->updateLives(m_lives);
    }
}
```

### Example 2: Player Collects Coin
```cpp
// In PlayerManager::update()
IItemManager* items = m_gameWorld->getItemManager();
if (items && checkCollisionWithItems()) {
    m_score += 10;

    // Update HUD
    IHUDManager* hud = m_gameWorld->getHUDManager();
    if (hud) {
        hud->updateScore(m_score);
    }
}
```

### Example 3: Enemy Checks Player Position
```cpp
// In EnemyManager::update()
IPlayerManager* player = m_gameWorld->getPlayerManager();
if (player && player->isAlive()) {
    float playerX = player->getPositionX();
    float playerY = player->getPositionY();

    // Adjust AI behavior based on player
    updateAI(playerX, playerY);
}
```

### Example 4: HUD Gets Updates from World
```cpp
// In GameWorld::update()
if (m_hudManager && m_playerManager) {
    m_hudManager->updateScore(m_playerManager->getScore());
    m_hudManager->updateLives(m_playerManager->getLives());
}

if (m_hudManager && m_enemyManager) {
    m_hudManager->updateEnemyCount(m_enemyManager->getEnemyCount());
}
```

---

## Build & Run

```bash
# Build
mkdir build && cd build
cmake .. -G Ninja
ninja

# Run
./Mario.exe

# Clean
ninja clean
```

---

## Testing Your Module

Each module can be tested independently without the full game:

```cpp
// Simple test example
#include "PlayerManager.h"
#include <SFML/Graphics.hpp>

int main() {
    PlayerManager player;
    player.initialize();

    sf::RenderWindow window(sf::VideoMode({800, 600}), "Player Test");

    sf::Clock clock;
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            player.handleInput(*event);
        }

        float dt = clock.restart().asSeconds();
        player.update(dt);

        window.clear(sf::Color::Black);
        player.render(window);
        window.display();
    }

    return 0;
}
```

---

## Critical Rules

### ? DO NOT
- Modify files outside your section
- Create direct dependencies between modules
- Change interface files (src/interfaces/I*.h)
- Call classes directly (use interfaces)
- Modify core game loop files
- Skip implementing interface methods

### ? DO
- Follow your interface contract exactly
- Use GameWorld to access other modules
- Keep your implementation minimal at first
- Test your module independently
- Add documentation to public methods
- Use TODO comments for tracking

---

## Next Steps

1. **Read ARCHITECTURE.md** - Full system design
2. **Read DEVELOPER_SETUP.md** - Your specific setup
3. **Grep for your TODO items** - Find tasks: `grep -r "TODO: [Your Name]" src/`
4. **Start implementing** - Replace stub with real implementation
5. **Test independently** - Build and verify your module works
6. **Notify Quynh Anh** - When ready to integrate
7. **Merge with team** - Git push for code review

---

## Interface Quick Reference

### IPlayerManager (Le Tran)
```cpp
class IPlayerManager {
    virtual void initialize() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) const = 0;
    virtual void handleInput(const sf::Event& event) = 0;
    virtual bool isAlive() const = 0;
    virtual int getScore() const = 0;
    virtual int getLives() const = 0;
    virtual float getPositionX() const = 0;
    virtual float getPositionY() const = 0;
};
```

### IEnemyManager (Dinh Anh)
```cpp
class IEnemyManager {
    virtual void initialize() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) const = 0;
    virtual int getEnemyCount() const = 0;
};
```

### IItemManager (Dinh Anh)
```cpp
class IItemManager {
    virtual void initialize() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) const = 0;
    virtual int getItemCount() const = 0;
};
```

### IHUDManager (Nguyen Phuc)
```cpp
class IHUDManager {
    virtual void initialize() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) const = 0;
    virtual void handleInput(const sf::Event& event) = 0;
    virtual void updateScore(int score) = 0;
    virtual void updateLives(int lives) = 0;
    virtual void updateEnemyCount(int count) = 0;
};
```

### ISaveManager (Nguyen Phuc)
```cpp
class ISaveManager {
    virtual void initialize() = 0;
    virtual bool saveGame() = 0;
    virtual bool loadGame() = 0;
    virtual bool hasSaveFile() const = 0;
};
```

---

## Getting Help

- **Architecture questions** ? Ask Quynh Anh
- **Interface clarification** ? Check ARCHITECTURE.md
- **Build errors** ? Check CMakeLists.txt
- **Module integration** ? Use GameWorld accessor methods
- **Git conflicts** ? Stay in your file section

---

Good luck! ??

Each team member should now independently implement their module following these guidelines.
