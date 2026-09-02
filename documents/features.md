# Game Features

## Player & Movement

1. **Two playable characters** — Mario (faster run, standard jump) and Luigi (higher jump, slightly slower) each have distinct physical stats and their own sprite sheet.
2. **Three-tier power-up progression** — the player upgrades from Normal (small) → Super (Mushroom) → Fire (Fire Flower), and downgrades one tier on damage.
3. **Variable-height jumping** — holding the jump key longer reduces mid-air gravity, producing a higher arc; releasing early cuts the jump short.
4. **Run mechanic** — holding the dedicated run key unlocks a higher top speed with NES-accurate momentum that must be bled off gradually when the key is released.
5. **Skid braking** — reversing direction at speed triggers a skid deceleration stronger than normal friction, visually indicating the player is fighting momentum.
6. **Fireball shooting** — in Fire form the player can shoot bouncing fireballs at a per-shot cooldown; fireballs kill most enemies on contact and explode on solid tiles.
7. **Star invincibility** — collecting a Star grants 10 seconds of immunity, killing enemies on contact, accompanied by a cycling four-color flash overlay.
8. **Invincibility frames (i-frames)** — after taking damage the player is immune to further hits for a fixed duration, preventing chain-death from overlapping enemies.
9. **Three-phase death animation** — on death the player freezes in a death pose for 0.35 s, launches upward at −420 px/s, then falls through the floor under gravity with tile collision disabled until off-screen.
10. **Pipe-travel animation** — the player slides smoothly into a pipe entrance, the screen fades, and the player emerges from the exit pipe in the destination map.
11. **Flagpole slide** — reaching the flagpole triggers a scripted slide-down animation; the height at which the player grabs the pole determines the score bonus awarded.
12. **Castle walk & disappear** — after the flagpole sequence the player walks to the castle entrance and disappears inside, completing the stage clear cinematic.

---

## Enemies

13. **Goomba** — walks in one direction, reverses on walls and ledge edges; dies instantly when stomped.
14. **Koopa Troopa** — stomped into a retreating shell that can be kicked to slide across the stage, killing other enemies in its path.
15. **Buzzy Beetle** — behaves like Koopa Troopa but is completely immune to fireballs, requiring a stomp or shell kick to defeat.
16. **Boss** — a multi-phase enemy that fires `BossFireball` projectiles; on stomp it transitions to a Koopa-shell phase before being finally defeated.
17. **Enemy factory** — all enemies are spawned at runtime from TMX map object data via `EnemyFactory`; no enemy positions are hardcoded in C++.
18. **Brick-break enemy kill** — breaking a brick while an enemy stands directly on top of it counts as a stomp-kill, awarding score and triggering the enemy's death animation.

---

## Items & Collectibles

19. **Mushroom** — slides out of a struck block and walks across the stage; collected on contact to upgrade the player from Normal to Super form.
20. **Fire Flower** — spawns from a struck block; collected to upgrade a Super player to Fire form.
21. **Star** — bounces energetically across the stage; collected to activate Star invincibility mode.
22. **Coin collection** — coins pop out of Question and Multi-Coin blocks with an animation, and static coins placed in the map are collected by walking through them; each increments the coin HUD counter.
23. **Floating score popups** — whenever the player earns points (kill, collect, time bonus) a numeric label appears at the world position of the event and floats upward before fading.

---

## Tile & Map System

24. **TMX map loading** — every level is a `.tmx` file authored in Tiled; `MapManager` parses the tile layer into a typed `TileType` grid and the object layer into spawn-data structs at runtime.
25. **Question blocks** — bumping from below awards one coin or spawns a power-up depending on the block's configuration; the block then turns into a depleted used-block tile.
26. **Multi-coin blocks** — can be bumped repeatedly for a fixed number of coins before becoming exhausted; each bump resets a short timer.
27. **Hidden blocks** — render as empty space and become solid only when bumped from below, at which point they reveal themselves and can award a coin or 1-Up.
28. **Brick breaking** — a Super or Fire player who bumps a normal brick from below shatters it into four flying fragments; a Normal player only bounces off.
29. **Death-zone tiles** — any tile typed as `DEATH_ZONE` (pits, lava) instantly kills the player on contact, bypassing form downgrades.
30. **Pipe warp** — `PIPE_ENTRANCE` tiles detect a standing-and-pressing-down input, start a pipe-travel cutscene, and load the target map at a configurable exit coordinate.

---

## Level Objects

31. **Moving platforms (lifts)** — oscillate horizontally or vertically with configurable range and speed; the player is carried along when standing on top and is blocked by their sides and bottom.
32. **Rotating fire bars** — rings of fireballs anchored to a center tile that spin at a configurable angular velocity and direction; any fireball that overlaps the player's hitbox deals damage.
33. **Peach NPC** — a non-interactive sprite of Princess Peach appears at the end of the final stage to indicate game completion.

---

## Game Flow & Modes

34. **Single-player mode** — one player controls Mario or Luigi through all stages with a shared lives pool.
35. **Two-player local co-op** — both players are on screen simultaneously; lives are shared; the camera tracks the midpoint between both players; score and coins are tracked per-player.
36. **Stage intermission screen** — a "STAGE X CLEAR!" screen is displayed between levels showing the cleared stage number, the next world label, the player character icon, and remaining lives, before automatically advancing.
37. **Countdown timer** — each stage has a visible countdown timer; reaching zero kills the player; remaining time at stage clear is converted to bonus score via a rapid drain animation.
38. **In-game pause** — pressing the pause key overlays a pause menu on top of the running game with options to resume, access settings, save and quit to menu, or quit without saving.
39. **Level select with unlock progression** — stages are locked until cleared; `SaveManager` tracks the maximum unlocked stage, and the Level Select screen renders locked stages as inaccessible.
40. **Built-in map editor** — a tile-paint editor reachable from the main menu lets the player draw tile types, place enemy and item spawn points, undo/redo edits, and export the result directly to a `.tmx` file playable in the game.

---

## HUD & UI

41. **Persistent HUD** — a fixed overlay displays the current score, coin count, remaining lives, world/stage number, and countdown timer throughout gameplay in the style of the original NES game.
42. **Achievement toast banners** — when a career achievement is unlocked a banner slides in at the top of the screen showing the achievement title for a few seconds before fading.

---

## Persistence & Settings

43. **Save & continue** — the full game state (current stage, lives, score, coins, game mode, and character selection) is serialized to disk as a Memento snapshot; on next launch the player can continue from exactly where they left off.
44. **High-score leaderboard** — the top five scores are stored persistently; after a game-over or win the player enters their initials, which are saved and displayed on the leaderboard screen.
45. **Career statistics** — the game tracks cumulative counts (enemies killed, coins collected, deaths, stages completed, etc.) across all sessions and displays them on a dedicated stats screen.
