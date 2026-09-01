# Super Mario Bros — Project Report

**[UNIVERSITY OF SCIENCE]**

**[FACULTY OF INFORMATION TECHNOLOGY]**

**OBJECT-ORIENTED PROGRAMMING – CS202**

**Class – Group:** [25A01] – Group 1

**Students:**
- Nguyễn Lê Trân – 25125039
- Lê Quang Nguyên Phúc - 25125070
- Lê Quỳnh Anh – 25125071
- Đinh Nguyễn Hồng ÁNh – 25125078

**Instructor:** 
- PhD Dinh Ba Tien
- MScTruong Phuoc Loc
- MSc Ho Tuan Thanh

---

## Table of Contents

- [1. Introduction](#1-introduction)
- [2. Team Composition](#2-team-composition)
- [3. Project Architecture](#3-project-architecture)
- [4. Applied Design Patterns and the Reasoning of Your Design](#4-applied-design-patterns-and-the-reasoning-of-your-design)
- [5. Project Achivements and Strength](#5-project-achivements-and-strength)
- [6. Technical Issues and Solution](#6-technical-issues-and-solution)
- [7. Conclusions](#7-conclusions)
- [8. Reference](#8-reference)

---

## 1. Introduction

This report documents the design and implementation of a C++ recreation of Super Mario Bros. (1985), built using the SFML graphics library. The project was undertaken as the team project for CS202, with the primary goal of applying object-oriented programming principles.

The game supports core mechanics from the original title, including player movement and power-ups (Mushroom, Fire Flower, Star), multiple enemy types with distinct behaviors (Goomba, Koopa Troopa, Buzzy Beetle, and a custom Boss enemy), tile-based level rendering and collision loaded from Tiled (`.tmx`) map files, moving platforms (lifts), rotating hazards (fire bars), and two-player local co-op support.

---

## 2. Team Composition

This project was developed by team of four members. Each member owned a distinct subsystem of the codebase, coordinated through a shared Git repository with feature branches merged into `main`.

| No. | Full Name | Student ID | Role / Responsibilities |
|---|---|---|---|
| 1 | Nguyễn Lê Trân |25125039 | |
| 2 | Lê Quang Nguyên Phúc|25125070 | |
| 3 | Lê Quỳnh Anh|25125071 | |
| 4 | Đinh Nguyễn Hồng Ánh|25125078 | |
---

## 3. Project Architecture

*[Insert the architecture diagram here once provided.]*

The codebase is organized into four broad layers, mirroring the folder structure under `src/`: `core` (the game loop and application entry point), `interfaces` (abstract contracts such as `IEnemyManager`, `IItemManager`, `IMapManager`, and `IPlayerManager`), `entities` (concrete game objects: enemies, items, the player, and the camera), and `world` (map loading, tile behavior, moving platforms, and hazards). `states/` holds top-level application states (e.g. menu, gameplay, pause), and `ui/` holds HUD rendering.

### 3.1 Layered, Manager-Based Design

Each category of game object — enemies, items, lifts, fire bars — is owned by a dedicated Manager class (`EnemyManager`, `ItemManager`, `LiftManager`, `FireBarManager`) that is responsible for that category's full lifecycle: loading its assets, spawning instances from parsed map data, driving per-frame physics and collision, and rendering. This keeps each Manager's responsibility narrow (Single Responsibility Principle) and keeps individual entity classes free of physics/collision boilerplate — an entity only implements what makes it behave differently from its siblings.

### 3.2 Data Flow: Map → Spawn → Simulation

Level content is authored in Tiled and stored as `.tmx` files. `MapManager` parses both the tile layer (converted into a `TileType` grid used for collision queries such as `isSolid()`) and the object layers (converted into typed spawn-data structs: `EntitySpawnData`, `PlayerSpawnData`, `LiftSpawnData`, `FireBarSpawnData`). These structs are pure data — they carry no behavior — and are handed to the relevant Manager (e.g. `EnemyManager::spawnFromMapData()`), which is solely responsible for turning that data into live, simulated objects. This separation means level design (Tiled) and gameplay code (C++) evolve independently: a designer can place a new enemy in a map without touching a single line of C++, provided the enemy type is already registered in the corresponding Manager.

### 3.3 Interfaces and Dependency Inversion

Managers do not depend on one another's concrete classes. Instead, cross-cutting concerns are expressed through interfaces: `EnemyManager` depends on `IPlayerManager` (to query the player's hitbox and apply damage) and `IMapManager` (to query tile collision), not on the concrete `PlayerManager` or `MapManager` classes. These dependencies are injected via setter methods (`setPlayerManager()`, `setMapManager()`) after construction, rather than being fixed at compile time. This follows the Dependency Inversion Principle: high-level modules (Managers) and low-level modules (concrete implementations) both depend on abstractions, which keeps the system testable and loosely coupled — a `MapManager` could be swapped for a mock implementation in a unit test without changing `EnemyManager` at all.

---

## 4. Applied Design Patterns

Several classic design patterns were used, each chosen to solve a concrete recurring problem encountered during development rather than applied speculatively.

### 4.1 Strategy Pattern (via Polymorphism)

**Reasoning:** Different enemy and item types need interchangeable "response algorithms" for the same triggering events — being stomped, touching the player, or being picked up — selected per object rather than branched on inside a single shared function.

**Proof:** `Enemy` declares `onStomp()` and `onPlayerCollision()` as pure virtual; `Goomba`, `KoopaTroopa`, `BuzzyBeetle`, `Boss`, and `BossFireball` each supply their own strategy — a `Goomba` dies outright, a `KoopaTroopa` retreats into a shell, a `BuzzyBeetle` flips onto its back temporarily, `Boss` unmasks into a Koopa shell. `Item` mirrors this with `OnInteract()`, implemented independently by `Coin`, `Mushroom`, `FireFlower`, and `Star`. `EnemyManager`/`ItemManager` call these through the base interface without any knowledge of which concrete strategy is behind them, so adding a new enemy or item type never requires modifying the manager's own code.

---

### 4.2 State Pattern

**Reasoning:** The application's overall flow is a long, order-sensitive sequence of distinct screens, each with entirely different input handling, rendering, and transition rules. Encoding this as one class with a giant switch statement would make every screen's logic fight for space in the same file; instead, each screen is its own object, and the class driving the app only needs to know how to hold "the current state" and swap it.
 
**Proof:** `core/GameState.h` defines the abstract state interface, and `core/StateManager.cpp/h` is the context that holds the active state and switches between concrete ones. The `states/` folder contains a full family of concrete implementations — `MenuState`, `ModeSelectState`, `CharacterSelectState`, `LevelSelectState`, `IntermissionState`, `GameOverState`, `LeaderboardState`, and `InitialsEntryState` — covering the entire menu-to-gameplay-to-endgame flow. Adding a new screen (e.g. a Settings menu) only requires a new `GameState` subclass; `StateManager` itself never needs to change.

---

### 4.3 Memento Pattern

**Reasoning:** Persisting and restoring game progress (unlocked levels, high scores, leaderboard entries) needs to capture a snapshot of scattered state at a point in time, without the component responsible for saving/loading needing to understand or reach into the internals of every object that contributed to that state.
 
**Proof:** `core/GameMemento.h` defines the opaque snapshot object, while `ISaveManager` (in `interfaces/`) and its implementation `SaveManager` (in `ui/`) act as the caretaker: they request, store, and later hand back a memento on behalf of the rest of the game, without needing to know the internal layout of whatever produced it. The presence of `LeaderboardState` and `InitialsEntryState` alongside `SaveManager` reflects a concrete use case for this: persisting a player's initials and score across sessions is a textbook memento scenario — a snapshot is taken at game-over, stored, and later restored for display on the leaderboard screen.

---

### 4.4 Observer Pattern

**Reasoning:** Several independent systems need to react whenever the active game state changes.
 
**Proof:** `StateManager` (in `core/`) drives transitions between `GameState`s, while independent systems such as `SoundManager` (`ISoundManager` in `interfaces/`, implemented in `ui/`) and `HUDManager` (`IHUDManager` in `interfaces/`, implemented in `ui/`) react to those transitions — swapping background music per state and showing/hiding HUD elements per state — without being merged into `StateManager` or `GameState` itself. Routing this through interfaces (`ISoundManager`, `IHUDManager`) rather than concrete classes keeps `StateManager` decoupled from exactly which systems are listening, and new observers (e.g. a future analytics or achievement system) could subscribe to state changes without `StateManager` being modified.

---

### 4.5 Factory Pattern

**Reasoning:** The concrete C++ class to instantiate for a given piece of level data should be resolved in one centralized place, so neither the map-loading code nor the level designer needs to know about C++ types directly.

**Proof:** `EnemyManager::spawnFromMapData()` switches on `EntitySpawnData::type` (a string such as `"Goomba"`, `"KoopaTroopa"`, `"Boss"`, parsed from a Tiled object's name/type field) to construct the matching concrete subclass. Everywhere else in the codebase — including the level data itself — enemies are only ever referred to as data (a type string plus a position), never as C++ types.

---

### 4.6 Command Pattern

**Reasoning:** An action triggered mid-update (spawning a new entity) needs to be deferred and executed as a discrete, later unit of work, rather than applied immediately at the point it's triggered.

**Proof:** `EnemyManager::update()` iterates `m_enemies` with a range-based for loop. If an entity spawned during that loop (e.g. a `BossFireball` fired by `Boss`) were inserted into `m_enemies` immediately, a vector reallocation could invalidate the iterator currently in use — undefined behavior surfacing as intermittent crashes. Instead, each spawn request is queued into `m_pendingSpawns` and only executed — merged into `m_enemies` — once the loop has fully finished, guaranteeing the container is never mutated while it is being iterated.

---

## 5. Project Achievements and Strengths

The finished project delivers a complete, playable slice of Super Mario Bros. rather than an isolated tech demo of any single subsystem:

- **Multiple stages:** the game ships with several distinct levels (`stage1`, `stage2`, `stage3`), plus a hidden-area variant (`stage1_hidden`) and a dedicated test map used for isolated feature testing — demonstrating that level content is fully data-driven rather than hardcoded per level.
- **A full item set:** Coin, Mushroom, Fire Flower, and Star are all implemented with distinct pickup effects (score, power-up progression, and temporary invincibility respectively), spawned dynamically from block interactions rather than only placed statically on the map.
- **A varied enemy roster:** from the simplest case (Goomba, a single death state) through increasingly stateful behavior (Koopa Troopa and Buzzy Beetle's shell/flip mechanics) up to a custom multi-phase Boss enemy with its own attack pattern and projectile (`BossFireball`) — showing the entity architecture scales from trivial to complex without needing to be redesigned.
- **Core player mechanics:** movement, jumping, power-up state progression (small → big → fire), taking damage with invincibility frames, Star-powered temporary invulnerability, and local two-player co-op support.
- **Decent UI management:** a persistent HUD displaying score, coin count, remaining lives, current world, and a countdown timer, kept visually consistent with the original game's presentation.
- **Clean organizational structure:** the project folder separates `core` (game loop/entry point), `interfaces` (abstract contracts), `entities` (enemies, items, player, camera), `world` (map/tile/lift/fire-bar systems), `states` (application states), and `ui` (HUD) into distinct, independently navigable layers — directly supporting the architectural goals described in Section 3.

---

## 6. Technical Issues and Solutions

**Merge Conflicts on Shared Files**

*The Problem:* Because several manager classes (`EnemyManager`, `MapManager`) and shared data contracts (`MapData.h`) are touched by multiple subsystems at once, team members editing these files in parallel repeatedly produced merge conflicts — particularly when two people extended the same `spawnFromMapData()`-style dispatch function or the same struct on different branches around the same time.

*The Solution:* The team moved to smaller, more frequent commits scoped to a single subsystem per branch, and adopted a light convention of flagging in advance when a shared file (rather than an individual entity file) needed to be touched, so changes could be sequenced instead of colliding. Where possible, additive changes (e.g. adding a new `else if` branch for a new enemy type, or a new field to a spawn-data struct) were kept structurally isolated from each other to reduce the chance of the same lines being edited on two branches simultaneously.

**Incomplete Features**

*The Problem:* Not everything originally scoped for the project was finished in time. Most notably, a free-form in-game map editor — which would have let players design and try out their own levels without hand-editing `.tmx` files — was not implemented.

*The Solution/Status:* A free map editor remains a natural candidate for future work, since the underlying data contract (`EntitySpawnData` and friends) was already designed to be format-agnostic and would not need to change to support a second, in-game authoring path.

**Missing Background Art**

*The Problem:* Several background sprites/decorative layers present in the original game were not added, leaving some scenes visually sparser than intended.

*The Solution/Status:* This was a scope/time trade-off rather than a technical blocker — the tile/rendering pipeline already supports background-type tiles (`TileType::BACKGROUND` is handled identically to empty space for collision purposes, purely decorative), so adding the remaining art is a content task, not an engineering one, and was left for a future asset pass once gameplay-critical systems were stable.

---

## 7. Conclusions

The project successfully reconstructs the core experience of Super Mario Bros. while meeting its underlying academic goal: demonstrating that object-oriented principles, combined with well-known design patterns can produce a real-time, interactive system that remains modular and extensible under active development. 

Multiple stages, a full item and enemy roster, core player mechanics, and a clean HUD together demonstrate a complete, playable game , and the folder-level architecture kept that scope manageable across a four-person team. 

The unresolved items are well understood, scoped, and left as clear next steps.

## 8. Reference
### Walkthough Gameplay
https://youtu.be/c_b9Yn34pdI?si=9ZAWERhmQPj2GZ9M

### Resources
https://sounds.spriters-resource.com/nes/supermariobros/ (sound)

https://www.mariomayhem.com/downloads/sprites/super_mario_bros_sprites.php (sprites)