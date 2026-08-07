# TODO Items - Task List for Each Developer

## ?? How to Use This List

Each developer can grep their TODO items:

```bash
# Find your tasks
grep -r "TODO: Le Tran" src/
grep -r "TODO: Dinh Anh" src/
grep -r "TODO: Nguyen Phuc" src/
grep -r "TODO: Quynh Anh" src/
```

---

## ?? Le Tran - Player/Character Module

### Located: `src/entities/PlayerManager.h` and `.cpp`

**Implementation Tasks:**
```cpp
// In PlayerManager.cpp

// TODO: Le Tran - Initialize player character
// - Load sprite
// - Set initial position
// - Initialize animation state

// TODO: Le Tran - Update player logic
// - Apply gravity
// - Handle movement
// - Check collisions with enemies and items
// - Update animation

// TODO: Le Tran - Render player
// - For now, draw a placeholder rectangle
// - Replace with sprite when ready

// TODO: Le Tran - Handle player input
// - Left/Right movement
// - Jump
// - Action buttons
```

### Checklist:
- [ ] Load Mario sprite/animation
- [ ] Implement gravity physics (F = 9.81 m/s²)
- [ ] Implement jumping (vertical velocity)
- [ ] Handle left movement (decrease X)
- [ ] Handle right movement (increase X)
- [ ] Prevent moving outside screen bounds
- [ ] Implement animation state machine (idle, walking, jumping, falling)
- [ ] Query EnemyManager for collisions
- [ ] Query ItemManager for collisions
- [ ] Handle enemy collision (take damage, knockback)
- [ ] Handle item collision (collect, add score)
- [ ] Implement lives system
- [ ] Implement respawning
- [ ] Implement death condition
- [ ] Implement score tracking
- [ ] Update HUD when score changes
- [ ] Update HUD when lives change
- [ ] Test independently with GameWorld

### Key Methods to Implement:
```cpp
void PlayerManager::initialize()
void PlayerManager::update(float deltaTime)
void PlayerManager::render(sf::RenderWindow&)
void PlayerManager::handleInput(const sf::Event&)

// Query methods
bool isAlive() const
int getScore() const
int getLives() const
float getPositionX() const
float getPositionY() const
```

### Communication Pattern:
```cpp
// In update() - Query enemies for collision
IEnemyManager* enemies = m_gameWorld->getEnemyManager();

// In update() - Query items for collision
IItemManager* items = m_gameWorld->getItemManager();

// In update() - Update HUD
IHUDManager* hud = m_gameWorld->getHUDManager();
if (hud) {
    hud->updateScore(m_score);
    hud->updateLives(m_lives);
}
```

---

## ?? Dinh Anh - Enemy & Item Module

### Located: `src/entities/EnemyManager.h/.cpp` and `src/entities/ItemManager.h/.cpp`

**EnemyManager Implementation Tasks:**

```cpp
// In EnemyManager.cpp

// TODO: Dinh Anh - Initialize enemy manager
// - Load enemy sprites
// - Spawn initial enemies
// - Set up AI parameters

// TODO: Dinh Anh - Update all enemies
// - Apply AI logic
// - Move enemies
// - Check boundaries
// - Handle spawning/despawning

// TODO: Dinh Anh - Render all enemies
// - For now, draw placeholder rectangles
// - Replace with sprites when ready
```

**ItemManager Implementation Tasks:**

```cpp
// In ItemManager.cpp

// TODO: Dinh Anh - Initialize item manager
// - Load item sprites
// - Spawn initial items
// - Set up item properties

// TODO: Dinh Anh - Update all items
// - Apply physics (gravity for falling items)
// - Handle animations
// - Check for collected items
// - Spawn new items

// TODO: Dinh Anh - Render all items
// - For now, draw placeholder rectangles
// - Replace with sprites when ready
```

### Enemy Checklist:
- [ ] Define Goomba enemy type
- [ ] Define Koopa enemy type (optional: multiple types)
- [ ] Implement Goomba AI (walk left-right, turn at walls)
- [ ] Implement Koopa AI (walk, shell throw)
- [ ] Implement boundary detection
- [ ] Implement enemy spawning at level start
- [ ] Implement wave spawning system
- [ ] Implement enemy collision with terrain
- [ ] Implement enemy collision with player (notify player)
- [ ] Implement enemy death
- [ ] Implement difficulty scaling
- [ ] Track enemy count
- [ ] Update HUD with enemy count
- [ ] Test independently with GameWorld

### Item Checklist:
- [ ] Define Coin item type
- [ ] Define PowerUp item type (optional: multiple)
- [ ] Implement coin physics (static or fall)
- [ ] Implement power-up physics (fall with slight movement)
- [ ] Implement item collection detection
- [ ] Implement item spawning at level start
- [ ] Implement random spawning
- [ ] Implement item effects on player
- [ ] Implement item animations
- [ ] Track item count
- [ ] Update HUD with item count
- [ ] Test independently with GameWorld

### Key Methods to Implement:

**EnemyManager:**
```cpp
void initialize()
void update(float deltaTime)
void render(sf::RenderWindow&)
int getEnemyCount() const
```

**ItemManager:**
```cpp
void initialize()
void update(float deltaTime)
void render(sf::RenderWindow&)
int getItemCount() const
```

### Communication Patterns:

**EnemyManager:**
```cpp
// Query player position
IPlayerManager* player = m_gameWorld->getPlayerManager();
if (player) {
    float playerX = player->getPositionX();
    float playerY = player->getPositionY();
}

// Update HUD
IHUDManager* hud = m_gameWorld->getHUDManager();
if (hud) {
    hud->updateEnemyCount(m_enemyCount);
}
```

**ItemManager:**
```cpp
// Update HUD
IHUDManager* hud = m_gameWorld->getHUDManager();
if (hud) {
    hud->updateEnemyCount(m_itemCount);  // Or separate method
}
```

---

## ?? Nguyen Phuc - UI & Save System

### Located: `src/ui/HUDManager.h/.cpp` and `src/ui/SaveManager.h/.cpp`

**HUDManager Implementation Tasks:**

```cpp
// In HUDManager.cpp

// TODO: Nguyen Phuc - Initialize HUD manager
// - Load fonts
// - Set up UI elements
// - Initialize display values

// TODO: Nguyen Phuc - Update HUD logic
// - Animate HUD elements
// - Handle UI state changes

// TODO: Nguyen Phuc - Render HUD
// - For now, draw placeholder rectangles for debug info
// - Replace with proper text rendering when ready

// TODO: Nguyen Phuc - Handle HUD input
// - Menu navigation
// - Button clicks
```

**SaveManager Implementation Tasks:**

```cpp
// In SaveManager.cpp

// TODO: Nguyen Phuc - Initialize save manager
// - Check for existing save files
// - Set up save directory

// TODO: Nguyen Phuc - Implement game save
// - Serialize game state
// - Write to file
// - Handle errors

// TODO: Nguyen Phuc - Implement game load
// - Read from save file
// - Deserialize game state
// - Restore state
```

### HUD Checklist:
- [ ] Load font file (Arial or similar)
- [ ] Create score display text
- [ ] Create lives display text
- [ ] Create level display text
- [ ] Create pause menu UI
- [ ] Create game over screen
- [ ] Implement score update method
- [ ] Implement lives update method
- [ ] Implement enemy count update
- [ ] Implement real-time HUD updates
- [ ] Handle pause menu input (arrow keys, enter)
- [ ] Implement pause menu navigation
- [ ] Implement game over menu
- [ ] Position UI elements nicely
- [ ] Test HUD independently

### Save System Checklist:
- [ ] Define save file format (JSON or binary)
- [ ] Serialize player state (score, lives, position)
- [ ] Serialize enemy state (positions, types)
- [ ] Serialize item state (positions, types)
- [ ] Serialize level state (current level, progress)
- [ ] Implement file write to save.dat or similar
- [ ] Implement file read from save.dat
- [ ] Implement save validation
- [ ] Implement error handling
- [ ] Implement autosave on level complete
- [ ] Implement quick save (Ctrl+S)
- [ ] Implement quick load (Ctrl+L)
- [ ] Test save/load cycle
- [ ] Test corrupted file handling

### Key Methods to Implement:

**HUDManager:**
```cpp
void initialize()
void update(float deltaTime)
void render(sf::RenderWindow&)
void handleInput(const sf::Event&)
void updateScore(int score)
void updateLives(int lives)
void updateEnemyCount(int count)
```

**SaveManager:**
```cpp
void initialize()
bool saveGame()
bool loadGame()
bool hasSaveFile() const
```

### Communication Patterns:

**HUDManager:**
```cpp
// Receives updates from GameWorld
void updateScore(int score) {
    m_displayScore = score;
}

void updateLives(int lives) {
    m_displayLives = lives;
}
```

**SaveManager:**
```cpp
// Called from menu or pause state
bool SaveManager::saveGame() {
    // Serialize all game state
    // Write to file
    // Return success
}

bool SaveManager::loadGame() {
    // Read from file
    // Deserialize game state
    // Restore to all managers
    // Return success
}
```

---

## ??? Quynh Anh - Architect / Integration

### Located: `src/core/`, `src/states/`, `src/world/`

**Integration Tasks:**

```cpp
// In PlayState.cpp - Already done!
// Module managers are already injected:
m_gameWorld->setPlayerManager(std::make_shared<PlayerManager>());
m_gameWorld->setEnemyManager(std::make_shared<EnemyManager>());
m_gameWorld->setItemManager(std::make_shared<ItemManager>());
m_gameWorld->setHUDManager(std::make_shared<HUDManager>());
m_gameWorld->setSaveManager(std::make_shared<SaveManager>());

// TODO: Quynh Anh - Transition to PauseState
// - Detect P key press
// - Create PauseState
// - Push to StateManager

// TODO: Quynh Anh - Transition from PauseState to PlayState
// - Detect resume input
// - Pop PauseState
// - Return to PlayState

// TODO: Quynh Anh - Transition from PlayState to GameOverState
// - Detect player death
// - Create GameOverState
// - Push to StateManager

// TODO: Quynh Anh - Transition from GameOverState to MenuState
// - Detect restart input
// - Pop all states
// - Return to MenuState
```

### State Transition Checklist:
- [ ] MenuState ? PlayState (SPACE key)
- [ ] PlayState ? PauseState (P key)
- [ ] PauseState ? PlayState (P key)
- [ ] PlayState ? GameOverState (player dies)
- [ ] GameOverState ? MenuState (SPACE key)
- [ ] Any state ? MenuState (ESC key)
- [ ] Verify clean state cleanup
- [ ] Verify no memory leaks during transitions

### Integration Checklist:
- [ ] All modules compile together
- [ ] GameWorld initializes all managers
- [ ] Input flows through state ? world ? managers
- [ ] Update cycle runs all managers
- [ ] Render cycle draws all managers
- [ ] HUD receives updates correctly
- [ ] Save/load integrated
- [ ] No crashes during gameplay
- [ ] Frame rate stays at 60 FPS

### Testing Checklist:
- [ ] Play through entire game flow
- [ ] Test all state transitions
- [ ] Test player input responsiveness
- [ ] Test enemy behavior
- [ ] Test item collection
- [ ] Test HUD updates
- [ ] Test pause/resume
- [ ] Test game over
- [ ] Test save/load
- [ ] Test performance (60 FPS)

### TODO Items in Code:
```bash
grep -r "TODO: Quynh Anh" src/
```

---

## ?? Quick Statistics

### Total TODO Items: 40+

**By Developer:**
- Le Tran: 8-10 TODOs
- Dinh Anh: 12-14 TODOs  
- Nguyen Phuc: 10-12 TODOs
- Quynh Anh: 8-10 TODOs

**By Category:**
- Implementation: 25
- Testing: 8
- Integration: 4
- Optimization: 3

---

## ?? Using This List

### Find Your Tasks:
```bash
# Search for your name
grep -r "TODO: [Your Name]" src/

# Search all TODOs
grep -r "TODO:" src/

# Search in specific file
grep "TODO:" src/entities/PlayerManager.cpp
```

### Mark Completion:
```cpp
// When done, replace TODO with DONE:
// OLD: // TODO: Le Tran - Initialize player character
// NEW: // DONE: Le Tran - Initialize player character (2024-01-15)
```

### Track Progress:
1. Print this list
2. Check off items as you complete them
3. Update DONE comments in code
4. Commit with meaningful messages

---

## ?? Getting Started

### Step 1: Find Your Section
```bash
grep -r "TODO: [Your Name]" src/ | wc -l
```

### Step 2: List All Your Tasks
```bash
grep -r "TODO: [Your Name]" src/
```

### Step 3: Print and Plan
- Print your task list
- Prioritize by dependency
- Estimate time per task
- Create implementation plan

### Step 4: Start Implementing
- Pick first task
- Implement required methods
- Test independently
- Move to next task

### Step 5: Integrate
- Notify Quynh Anh
- Run full game
- Debug issues
- Mark tasks done

---

## ?? Tips

- **Small commits** - Commit after each TODO completion
- **Test often** - Test your module before moving to next task
- **Communicate** - Let team know progress
- **Document** - Add comments for complex logic
- **Review** - Check QUICK_REFERENCE.md for patterns

---

## Questions?

- See DEVELOPER_SETUP.md for your role
- See QUICK_REFERENCE.md for code patterns
- See ARCHITECTURE.md for system design
- Ask Quynh Anh for architecture questions

---

Good luck! Let's crush these TODOs! ??
