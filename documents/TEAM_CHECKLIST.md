# Team Development Checklist

## ?? Project Setup - COMPLETE ?

### Core Architecture
- [x] Game loop implemented (Game class)
- [x] State management (StateManager)
- [x] Game states created (Menu, Play, Pause, GameOver)
- [x] GameWorld coordinator created
- [x] Module interfaces defined
- [x] Stub implementations for all modules
- [x] CMakeLists.txt updated with all files
- [x] Project compiles successfully

### Documentation
- [x] ARCHITECTURE.md - Full system design
- [x] DEVELOPER_SETUP.md - Developer guide per role
- [x] QUICK_REFERENCE.md - Quick lookup guide
- [x] ARCHITECTURE_DIAGRAMS.md - Visual documentation

### Code Quality
- [x] All headers use `#pragma once`
- [x] PascalCase for class names
- [x] camelCase for methods
- [x] m_ prefix for member variables
- [x] const correctness applied
- [x] Smart pointers used for ownership
- [x] No circular dependencies
- [x] Interfaces defined and stable

---

## ?? Developer Responsibilities

### ?? Le Tran - Player/Character Module

**Files to Implement:**
- [ ] src/entities/PlayerManager.h - Extend with methods
- [ ] src/entities/PlayerManager.cpp - Implement player logic

**Interface Methods (REQUIRED):**
- [ ] `void initialize()` - Load player sprites, set initial state
- [ ] `void update(float deltaTime)` - Update physics, position, state
- [ ] `void render(sf::RenderWindow&)` - Draw player character
- [ ] `void handleInput(const sf::Event&)` - Handle keyboard input
- [ ] `bool isAlive() const` - Query alive status
- [ ] `int getScore() const` - Query current score
- [ ] `int getLives() const` - Query remaining lives
- [ ] `float getPositionX() const` - Query X position
- [ ] `float getPositionY() const` - Query Y position

**Additional Methods to Implement:**
- [ ] Jump physics
- [ ] Left/right movement
- [ ] Gravity simulation
- [ ] Animation state machine
- [ ] Collision detection with enemies
- [ ] Collision detection with items
- [ ] Death/respawn logic
- [ ] Score tracking

**Integration Points:**
- [ ] Query enemy positions via GameWorld
- [ ] Query item positions via GameWorld
- [ ] Update HUD when score changes
- [ ] Update HUD when lives change

**TODO Items in Code:**
```bash
grep -r "TODO: Le Tran" src/
```

**Milestone Checklist:**
- [ ] Player renders on screen
- [ ] Player can move left/right
- [ ] Player can jump
- [ ] Gravity is applied
- [ ] Collision with enemies works
- [ ] Collision with items works
- [ ] Score updates correctly
- [ ] Lives decrease on damage
- [ ] Game over when lives = 0

---

### ?? Dinh Anh - Enemy & Item Module

**Files to Implement:**
- [ ] src/entities/EnemyManager.h - Extend with methods
- [ ] src/entities/EnemyManager.cpp - Implement enemy logic
- [ ] src/entities/ItemManager.h - Extend with methods
- [ ] src/entities/ItemManager.cpp - Implement item logic

**EnemyManager Interface Methods (REQUIRED):**
- [ ] `void initialize()` - Spawn initial enemies
- [ ] `void update(float deltaTime)` - Update enemy AI and positions
- [ ] `void render(sf::RenderWindow&)` - Draw all enemies
- [ ] `int getEnemyCount() const` - Query enemy count

**ItemManager Interface Methods (REQUIRED):**
- [ ] `void initialize()` - Spawn initial items
- [ ] `void update(float deltaTime)` - Update item physics
- [ ] `void render(sf::RenderWindow&)` - Draw all items
- [ ] `int getItemCount() const` - Query item count

**Enemy Implementation Tasks:**
- [ ] Define enemy types (Goomba, Koopa, etc.)
- [ ] Implement enemy movement patterns
- [ ] Implement enemy AI (patrol, chase, etc.)
- [ ] Implement enemy collision detection
- [ ] Implement enemy spawning system
- [ ] Implement enemy death/removal
- [ ] Implement difficulty scaling

**Item Implementation Tasks:**
- [ ] Define item types (Coin, PowerUp, etc.)
- [ ] Implement item gravity/physics
- [ ] Implement item collection detection
- [ ] Implement item spawning system
- [ ] Implement item effects
- [ ] Implement item animations

**Integration Points:**
- [ ] Query player position for AI
- [ ] Notify HUD of enemy count changes
- [ ] Check collision with player
- [ ] Communicate with player for damage/collection

**TODO Items in Code:**
```bash
grep -r "TODO: Dinh Anh" src/
```

**Milestone Checklist - Enemies:**
- [ ] Enemies spawn and render
- [ ] Enemies move with AI
- [ ] Enemies detect boundaries
- [ ] Enemies can collide with player
- [ ] Multiple enemy types exist
- [ ] Difficulty increases over time

**Milestone Checklist - Items:**
- [ ] Items spawn and render
- [ ] Items can be collected
- [ ] Items have effects on player
- [ ] Multiple item types exist
- [ ] Item physics work correctly

---

### ?? Nguyen Phuc - UI & Save System Module

**Files to Implement:**
- [ ] src/ui/HUDManager.h - Extend with methods
- [ ] src/ui/HUDManager.cpp - Implement HUD rendering
- [ ] src/ui/SaveManager.h - Extend with methods
- [ ] src/ui/SaveManager.cpp - Implement save/load logic

**HUDManager Interface Methods (REQUIRED):**
- [ ] `void initialize()` - Load fonts, create UI elements
- [ ] `void update(float deltaTime)` - Update animations
- [ ] `void render(sf::RenderWindow&)` - Draw HUD
- [ ] `void handleInput(const sf::Event&)` - Handle menu clicks
- [ ] `void updateScore(int score)` - Display score
- [ ] `void updateLives(int lives)` - Display lives
- [ ] `void updateEnemyCount(int count)` - Display enemy count

**SaveManager Interface Methods (REQUIRED):**
- [ ] `void initialize()` - Check for existing saves
- [ ] `bool saveGame()` - Save game state
- [ ] `bool loadGame()` - Load game state
- [ ] `bool hasSaveFile() const` - Check save file exists

**HUD Implementation Tasks:**
- [ ] Load font files
- [ ] Create score display
- [ ] Create lives display
- [ ] Create level display
- [ ] Create pause menu
- [ ] Create game over screen
- [ ] Create pause overlay
- [ ] Implement menu navigation
- [ ] Handle menu input

**Save System Implementation Tasks:**
- [ ] Define save file format
- [ ] Serialize player state
- [ ] Serialize enemy state
- [ ] Serialize item state
- [ ] Serialize level state
- [ ] Implement file I/O
- [ ] Handle save errors
- [ ] Implement autosave
- [ ] Implement load validation

**Integration Points:**
- [ ] Receive updates from GameWorld
- [ ] Display player score/lives
- [ ] Display enemy count
- [ ] Save/restore from menu states
- [ ] Query all managers for state

**TODO Items in Code:**
```bash
grep -r "TODO: Nguyen Phuc" src/
```

**Milestone Checklist - HUD:**
- [ ] Score displays correctly
- [ ] Lives display correctly
- [ ] Enemy count displays
- [ ] HUD updates in real-time
- [ ] Pause menu renders
- [ ] Game over screen renders
- [ ] Menu is functional
- [ ] Font rendering works

**Milestone Checklist - Save System:**
- [ ] Game can be saved
- [ ] Game can be loaded
- [ ] Save file persists on disk
- [ ] Autosave works
- [ ] Load on startup works
- [ ] Save validation works
- [ ] Error handling works

---

### ?? Quynh Anh - Architect / Integration Lead

**Files to Maintain:**
- [ ] src/core/Game.h/.cpp
- [ ] src/core/StateManager.h/.cpp
- [ ] src/core/GameState.h
- [ ] src/states/PlayState.h/.cpp
- [ ] src/world/GameWorld.h/.cpp

**Integration Tasks:**
- [ ] Verify all modules compile
- [ ] Test module integration
- [ ] Implement state transitions
- [ ] Handle game flow (win/lose)
- [ ] Fix build/link issues
- [ ] Coordinate with team
- [ ] Review code quality
- [ ] Manage merge conflicts

**Module Integration Checklist:**
- [ ] PlayerManager integrates correctly
- [ ] EnemyManager integrates correctly
- [ ] ItemManager integrates correctly
- [ ] HUDManager integrates correctly
- [ ] SaveManager integrates correctly

**State Transition Implementation:**
- [ ] MenuState ? PlayState (on SPACE)
- [ ] PlayState ? PauseState (on P)
- [ ] PauseState ? PlayState (on P)
- [ ] PlayState ? GameOverState (on player death)
- [ ] GameOverState ? MenuState (on SPACE)
- [ ] Any state ? MenuState (on ESC)

**Testing Checklist:**
- [ ] Game loop runs at 60 FPS
- [ ] All inputs work correctly
- [ ] State transitions work
- [ ] Module communication works
- [ ] No memory leaks
- [ ] Clean shutdown

**Documentation Tasks:**
- [ ] Review all .md files
- [ ] Verify code examples work
- [ ] Update with team feedback
- [ ] Create contribution guidelines

**TODO Items in Code:**
```bash
grep -r "TODO: Quynh Anh" src/
```

---

## ?? Pre-Implementation Checklist

### Before Starting Implementation
- [ ] Read ARCHITECTURE.md completely
- [ ] Read DEVELOPER_SETUP.md for your role
- [ ] Read QUICK_REFERENCE.md
- [ ] Review ARCHITECTURE_DIAGRAMS.md
- [ ] Find all TODO items for your module: `grep -r "TODO: [Your Name]" src/`
- [ ] Build current project: `ninja`
- [ ] Run current game: `./Mario.exe`
- [ ] Open your module files in editor
- [ ] Set up git branch if using version control

---

## ?? Implementation Workflow

### Step 1: Planning Phase
```
For your module:
1. List all TODO items
2. Prioritize by dependency
3. Create implementation plan
4. Estimate time needed
```

### Step 2: Core Implementation
```
1. Implement required interface methods
2. Add internal data structures
3. Implement core logic
4. Test independently (no full game needed)
```

### Step 3: Integration
```
1. Notify Quynh Anh
2. Run full game with module
3. Debug any issues
4. Verify communication with other modules
```

### Step 4: Polish
```
1. Add animations/effects
2. Optimize performance
3. Add error handling
4. Clean up code
```

### Step 5: Testing
```
1. Unit test your module
2. Integration test with GameWorld
3. Full game testing
4. Edge case testing
```

---

## ?? Troubleshooting Guide

### Build Error: "undefined reference"
- [ ] Check if .cpp file is in CMakeLists.txt
- [ ] Check includes are correct
- [ ] Rebuild: `ninja clean && ninja`

### Build Error: "no matching function"
- [ ] Check SFML 3.x API (Vector2f for setPosition)
- [ ] Check method signatures match interface
- [ ] Check parameter types

### Runtime Error: Module returns nullptr
- [ ] Check GameWorld.set*Manager() called
- [ ] Check gameWorld->initialize() called
- [ ] Check manager not deleted early

### Crash on module access
- [ ] Add null checks before using m_gameWorld
- [ ] Verify manager was set
- [ ] Check for use-after-free

### Merge Conflict
- [ ] Verify you edited only your files
- [ ] Communicate with team member
- [ ] Keep interface files unchanged
- [ ] Resolve in version control

---

## ? Final Checklist Before Submission

### Code Quality
- [ ] Code follows conventions
- [ ] No compiler warnings
- [ ] No memory leaks (use valgrind if available)
- [ ] Const correctness applied
- [ ] Smart pointers used correctly
- [ ] No direct coupling between modules
- [ ] Comments added for complex logic

### Functionality
- [ ] All interface methods implemented
- [ ] Module initializes correctly
- [ ] Module updates correctly
- [ ] Module renders correctly
- [ ] Input handling works
- [ ] Queries return correct values
- [ ] Error handling implemented

### Integration
- [ ] Module accessible via GameWorld
- [ ] Can query other modules
- [ ] Can notify HUD
- [ ] No crashes when full game runs
- [ ] Game loop frame rate stable

### Documentation
- [ ] Code comments clear
- [ ] Public methods documented
- [ ] Complex algorithms explained
- [ ] Known limitations documented

### Testing
- [ ] Module tested independently
- [ ] Module tested in full game
- [ ] Edge cases handled
- [ ] Error conditions tested
- [ ] Performance acceptable

---

## ?? Progress Tracking

### Week 1: Architecture & Setup
- [x] All architecture files created
- [x] All interfaces defined
- [x] All stubs created
- [x] All documentation written
- [x] Project compiles

### Week 2: Individual Implementation
- [ ] Le Tran: PlayerManager implementation
- [ ] Dinh Anh: EnemyManager implementation
- [ ] Dinh Anh: ItemManager implementation
- [ ] Nguyen Phuc: HUDManager implementation
- [ ] Nguyen Phuc: SaveManager implementation

### Week 3: Integration & Testing
- [ ] All modules integrated
- [ ] State transitions working
- [ ] All inputs functioning
- [ ] Game flow complete
- [ ] Full game playable

### Week 4: Polish & Optimization
- [ ] Performance optimized
- [ ] Visual polish
- [ ] Sound (if applicable)
- [ ] Bug fixes
- [ ] Final testing

---

## ?? Communication Protocol

### Daily Standup Template
```
What I did yesterday:
- [List completed tasks]

What I'm doing today:
- [List today's tasks]

Blockers:
- [Any issues stopping progress]

Questions:
- [Ask team]
```

### Integration Points
```
When module is ready for integration:
1. Notify Quynh Anh
2. Provide test results
3. Mark TODO items as DONE
4. Request review
5. Merge with team code
```

### Code Review Checklist
Before merging:
- [ ] All interface methods implemented
- [ ] No compiler warnings
- [ ] No memory leaks
- [ ] Follows naming conventions
- [ ] Consistent with architecture
- [ ] Documentation complete
- [ ] Tests passing

---

## ?? Success Criteria

### Project Complete When:
- [x] Architecture established (DONE)
- [ ] All modules implemented
- [ ] All features working
- [ ] No merge conflicts
- [ ] All tests passing
- [ ] Documentation complete
- [ ] Performance acceptable
- [ ] Code quality high

### Module Complete When:
- [ ] All TODO items done
- [ ] Interface fully implemented
- [ ] Independently testable
- [ ] Integrates with GameWorld
- [ ] Communicates with other modules
- [ ] Documentation complete
- [ ] Code reviewed
- [ ] Ready for team integration

---

## ?? Useful Commands

### Build
```bash
cd build
cmake .. -G Ninja
ninja
```

### Find TODO Items
```bash
grep -r "TODO:" src/          # All TODOs
grep -r "TODO: Le Tran" src/  # Your TODOs
```

### Clean Build
```bash
ninja clean
ninja
```

### Run Game
```bash
./Mario.exe
```

### Check Code Style
```bash
# If clang-format available
clang-format -i src/**/*.cpp src/**/*.h
```

---

## ?? References

- ARCHITECTURE.md - Full system design
- DEVELOPER_SETUP.md - Your specific role
- QUICK_REFERENCE.md - Quick lookup
- ARCHITECTURE_DIAGRAMS.md - Visual guide

---

## ?? Ready to Start?

1. Choose your role
2. Read your developer guide
3. Find your TODO items
4. Start implementing
5. Notify team of progress
6. Celebrate completion! ??

Good luck team! ??
