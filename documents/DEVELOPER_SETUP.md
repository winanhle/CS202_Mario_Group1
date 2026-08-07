# Developer Setup Guide

## For All Developers

### Project Structure
```
Mario/
??? src/
?   ??? core/              [Core framework - DO NOT MODIFY]
?   ??? states/            [Game states - DO NOT MODIFY]
?   ??? interfaces/        [Module contracts - READ ONLY]
?   ??? world/             [Coordinator - DO NOT MODIFY]
?   ??? entities/          [Team implementations]
?   ??? ui/                [Team implementations]
??? CMakeLists.txt
??? ARCHITECTURE.md        [Read this first!]
```

### Build Instructions
```bash
mkdir build
cd build
cmake .. -G Ninja
ninja
```

### Running the Game
```bash
# From build directory
./Mario.exe
```

Currently shows:
- Blue box = Menu (Press SPACE)
- Green box = Gameplay (Press P to pause)
- Yellow box = Paused (Press P to resume)
- Red box = Game Over (Press SPACE)

---

## Le Tran - Player/Character Module

### Your Responsibility
Implement the Mario player character with controls and physics.

### Files to Modify
- `src/entities/PlayerManager.h` - Extend with your methods
- `src/entities/PlayerManager.cpp` - Implement player logic

### Interface Contract (MUST implement)
```cpp
class PlayerManager : public IPlayerManager
{
public:
    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    void handleInput(const sf::Event& event) override;

    bool isAlive() const override;
    int getScore() const override;
    int getLives() const override;
    float getPositionX() const override;
    float getPositionY() const override;
};
```

### Tasks
- [ ] Create Mario sprite/animation
- [ ] Implement gravity and jumping physics
- [ ] Handle left/right movement input
- [ ] Handle action button input
- [ ] Implement collision queries to check enemy/item hits
- [ ] Implement death and respawn logic
- [ ] Track score and lives

### Example: Using GameWorld to Query Other Modules
```cpp
// In PlayerManager.cpp
// To check if touching an enemy:
IEnemyManager* enemies = m_gameWorld->getEnemyManager();
if (enemies && enemies->getEnemyCount() > 0) {
    // Check collision
}

// To check if touching an item:
IItemManager* items = m_gameWorld->getItemManager();
if (items && items->getItemCount() > 0) {
    // Collect item
}
```

### Pattern for Score Increase
```cpp
void PlayerManager::collectCoin()
{
    m_score += 10;

    // Update HUD display
    IHUDManager* hud = m_gameWorld->getHUDManager();
    if (hud) {
        hud->updateScore(m_score);
    }
}
```

### TODO Markers in Code
Search for `// TODO: Le Tran` to find implementation points.

### Testing Your Module
```cpp
// Simple test - in a test file
PlayerManager player;
player.initialize();
sf::RenderWindow window(sf::VideoMode({800, 600}), "Test");
player.update(0.016f);  // 60 FPS frame
player.render(window);
```

---

## Dinh Anh - Enemy & Item Module

### Your Responsibility
Implement game enemies, items, and content spawning.

### Files to Modify
- `src/entities/EnemyManager.h` - Extend with your methods
- `src/entities/EnemyManager.cpp` - Implement enemy logic
- `src/entities/ItemManager.h` - Extend with your methods
- `src/entities/ItemManager.cpp` - Implement item logic

### Interface Contracts (MUST implement)

**EnemyManager:**
```cpp
class EnemyManager : public IEnemyManager
{
public:
    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    int getEnemyCount() const override;
};
```

**ItemManager:**
```cpp
class ItemManager : public IItemManager
{
public:
    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    int getItemCount() const override;
};
```

### Tasks - Enemies
- [ ] Define enemy types (Goomba, Koopa, etc.)
- [ ] Implement enemy AI behavior
- [ ] Implement enemy movement patterns
- [ ] Implement enemy collision/detection
- [ ] Implement enemy spawning system
- [ ] Track enemy count for HUD

### Tasks - Items
- [ ] Define item types (Coin, PowerUp, etc.)
- [ ] Implement item physics (gravity for falling items)
- [ ] Implement item collection detection
- [ ] Implement item spawning system
- [ ] Track item count for HUD

### Example: Using GameWorld to Query Other Modules
```cpp
// In EnemyManager.cpp
// To check if player touched enemy:
IPlayerManager* player = m_gameWorld->getPlayerManager();
if (player) {
    float playerX = player->getPositionX();
    float playerY = player->getPositionY();

    // Check if enemy collides with player
    if (checkCollision(playerX, playerY)) {
        // Kill player or enemy
    }
}
```

### Pattern for HUD Updates
```cpp
void EnemyManager::update(float deltaTime)
{
    // ... update all enemies ...

    // Update HUD with current enemy count
    IHUDManager* hud = m_gameWorld->getHUDManager();
    if (hud) {
        hud->updateEnemyCount(m_enemyCount);
    }
}
```

### TODO Markers in Code
Search for `// TODO: Dinh Anh` to find implementation points.

---

## Nguyen Phuc - UI & Save System

### Your Responsibility
Implement the HUD/UI display and game save/load system.

### Files to Modify
- `src/ui/HUDManager.h` - Extend with your methods
- `src/ui/HUDManager.cpp` - Implement HUD rendering
- `src/ui/SaveManager.h` - Extend with your methods
- `src/ui/SaveManager.cpp` - Implement save/load logic

### Interface Contracts (MUST implement)

**HUDManager:**
```cpp
class HUDManager : public IHUDManager
{
public:
    void initialize() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) const override;
    void handleInput(const sf::Event& event) override;

    void updateScore(int score) override;
    void updateLives(int lives) override;
    void updateEnemyCount(int count) override;
};
```

**SaveManager:**
```cpp
class SaveManager : public ISaveManager
{
public:
    void initialize() override;
    bool saveGame() override;
    bool loadGame() override;
    bool hasSaveFile() const override;
};
```

### Tasks - HUD Manager
- [ ] Load fonts for text rendering
- [ ] Display player score
- [ ] Display player lives
- [ ] Display level information
- [ ] Display game timer
- [ ] Display enemy count (for debugging)
- [ ] Create pause menu UI
- [ ] Create game over screen
- [ ] Handle menu navigation input

### Tasks - Save Manager
- [ ] Create save file format
- [ ] Serialize game state (player pos, score, lives, etc.)
- [ ] Deserialize game state from file
- [ ] Handle save/load errors
- [ ] Implement autosave
- [ ] Implement load from main menu

### Example: Receiving Updates from GameWorld
```cpp
// HUDManager::updateScore() is called by GameWorld
void HUDManager::updateScore(int score)
{
    m_displayScore = score;
    // Update text rendering on next render()
}

// HUDManager::updateLives() is called by GameWorld
void HUDManager::updateLives(int lives)
{
    m_displayLives = lives;
}

// HUDManager::updateEnemyCount() is called by GameWorld
void HUDManager::updateEnemyCount(int count)
{
    m_displayEnemyCount = count;
}
```

### Example: Using Save Manager
```cpp
// In PlayState or GameWorld
if (shouldSave) {
    ISaveManager* saveManager = m_gameWorld->getSaveManager();
    if (saveManager) {
        saveManager->saveGame();
    }
}

if (shouldLoad) {
    ISaveManager* saveManager = m_gameWorld->getSaveManager();
    if (saveManager && saveManager->hasSaveFile()) {
        saveManager->loadGame();
    }
}
```

### TODO Markers in Code
Search for `// TODO: Nguyen Phuc` to find implementation points.

---

## Quynh Anh - Architect / Integration Lead

### Your Responsibility
Maintain core game loop, state management, and coordinate integration.

### Files You Maintain
- `src/core/Game.h` / `.cpp` - Game loop
- `src/core/StateManager.h` / `.cpp` - State management
- `src/states/PlayState.h` / `.cpp` - Module integration
- `src/world/GameWorld.h` / `.cpp` - Coordinator

### Do NOT Modify
These are owned by other developers:
- `src/entities/PlayerManager.h` / `.cpp` - Le Tran
- `src/entities/EnemyManager.h` / `.cpp` - Dinh Anh
- `src/entities/ItemManager.h` / `.cpp` - Dinh Anh
- `src/ui/HUDManager.h` / `.cpp` - Nguyen Phuc
- `src/ui/SaveManager.h` / `.cpp` - Nguyen Phuc

### Integration Points (Already Set Up)
In `src/states/PlayState.cpp`, modules are integrated:
```cpp
// These are already done - leave them as-is
m_gameWorld->setPlayerManager(std::make_shared<PlayerManager>());
m_gameWorld->setEnemyManager(std::make_shared<EnemyManager>());
m_gameWorld->setItemManager(std::make_shared<ItemManager>());
m_gameWorld->setHUDManager(std::make_shared<HUDManager>());
m_gameWorld->setSaveManager(std::make_shared<SaveManager>());
m_gameWorld->initialize();
```

### Your Tasks
- [ ] Implement state transitions (Menu ? Play ? Pause ? GameOver)
- [ ] Handle window closing
- [ ] Manage game flow (win/lose conditions)
- [ ] Handle pause/resume
- [ ] Coordinate save/load from state machine
- [ ] Test module integration
- [ ] Fix any build/linking issues

### TODO Markers in Code
Search for `// TODO: Quynh Anh` to find tasks.

### Integration Checklist
- [ ] All modules compile without errors
- [ ] GameWorld initializes all managers
- [ ] Input flows through state ? world ? managers
- [ ] Update cycle runs for all managers
- [ ] Render cycle draws all managers
- [ ] HUD updates with current game state
- [ ] Save/load works from menu

---

## Common Patterns

### Pattern 1: Module Needs Data from Another Module
```cpp
// In your manager's method:
IPlayerManager* player = m_gameWorld->getPlayerManager();
if (player) {
    float playerX = player->getPositionX();
    float playerY = player->getPositionY();
    // Use the data
}
```

### Pattern 2: Module Updates HUD
```cpp
// In your manager's update():
IHUDManager* hud = m_gameWorld->getHUDManager();
if (hud) {
    hud->updateScore(m_score);
    hud->updateLives(m_lives);
}
```

### Pattern 3: Handling Input in Your Module
```cpp
void PlayerManager::handleInput(const sf::Event& event)
{
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->code == sf::Keyboard::Key::Left) {
            moveLeft();
        }
        else if (keyEvent->code == sf::Keyboard::Key::Right) {
            moveRight();
        }
        else if (keyEvent->code == sf::Keyboard::Key::Space) {
            jump();
        }
    }
}
```

### Pattern 4: Adding Public Methods to Your Manager
```cpp
// In PlayerManager.h
class PlayerManager : public IPlayerManager
{
public:
    // Required by interface
    void initialize() override;
    void update(float deltaTime) override;
    // ... etc ...

    // Additional methods (not in interface)
    void takeDamage(int amount);
    void collectCoin(int value);
    void resetPosition();
};
```

---

## Troubleshooting

### Build Fails: "undefined reference to ..."
- Make sure your implementation file (.cpp) is in CMakeLists.txt
- Check that you included necessary headers

### Build Fails: "no overloaded function..."
- SFML 3.x uses different APIs than 2.x
- Use `sf::Vector2f` for positions: `.setPosition({x, y})`
- Check SFML 3.x documentation

### Runtime: Manager returns nullptr
- Check that GameWorld.set*Manager() was called in PlayState constructor
- Verify manager was initialized: `gameWorld->initialize()`

### Merge Conflicts
- Follow the file ownership rules (stay in your section)
- Minimal conflicts expected since each developer has separate files

---

## Key Points to Remember

1. **Never modify files outside your section** - You will cause merge conflicts
2. **Always use GameWorld to access other modules** - No direct coupling
3. **Implement all interface methods** - Contract must be fulfilled
4. **Add TODO comments for major tasks** - Helps Quynh track progress
5. **Test your module independently** - Don't wait for everything else
6. **Keep interfaces stable** - Don't change them once implemented
7. **Use shared_ptr for ownership** - GameWorld manages lifetime

---

## Getting Help

If you need to:
1. **Add a new interface method** - Talk to Quynh Anh
2. **Access data from another module** - Add it to that module's interface
3. **Report a bug** - Check if it's in your module first
4. **Modify core files** - Contact Quynh Anh before changing

---

## Progress Tracking

Each module has TODO comments. Search your code for:
- `// TODO: [Your Name]` - Things you need to implement
- `// TODO: Quynh Anh` - Integration tasks

Completed tasks: Replace TODO with DONE comment for tracking.

---

Happy coding! ??
