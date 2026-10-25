# Cryptmarch

A small 2D top-down dungeon crawler in the style of *Gauntlet* (1985), written in C++ on the **GamesEngineeringBase** framework.

Built for **WM908-15 Programming and Fundamental Algorithms** (WMG, University of Warwick), individual project, 2026/27.

> **Status: work in progress.** See [Feature Status](#feature-status) for what currently works. The day-by-day development record is in [`build-log.md`](build-log.md).

---

## Table of Contents

1. [Overview](#overview)
2. [Feature Status](#feature-status)
3. [Requirements](#requirements)
4. [Building and Running](#building-and-running)
5. [Controls](#controls)
6. [Project Structure](#project-structure)
7. [Architecture](#architecture)
8. [Technical Notes](#technical-notes)
9. [Map File Format](#map-file-format)
10. [Performance](#performance)
11. [Assignment Constraints](#assignment-constraints)
12. [Assets and Credits](#assets-and-credits)
13. [Known Issues](#known-issues)

---

## Overview

The game is built around a core game loop (input, update, draw, present) driven by a simple state machine: **Main Menu → Gameplay → Tally screen → Main Menu**. In gameplay the player controls a hero in a tile-based dungeon, with a camera that follows them, enemies that chase them, melee combat with knockback, and a health bar. When the hero dies, the tally screen shows time survived and enemies killed.

The emphasis of the assignment is on **data structures and algorithms**, so the project uses its own container class (`Vector<T>`) and a custom software renderer that writes directly into the framework's back buffer, with no STL containers.

## Feature Status

Legend: ✅ done · 🔧 in progress / partial · ⬜ not started

### 1. Core Video Experience and Game Flow
| Feature | Status | Notes |
| --- | --- | --- |
| Player-controlled hero with weapon and health | ✅ | Dwarf hero with melee weapon, health and mana |
| Camera follows hero | ✅ | Clamped to the map edges on fixed maps |
| Multiple selectable heroes | 🔧 | `Player` base class in place; Dwarf done, Wizard and Archer planned; no hero select screen yet |
| Game level runs for at least two minutes | ⬜ | Needs a larger level with spawners and an exit |
| Intro / Menu / Gameplay / Tally loop | 🔧 | Menu, Gameplay and Tally loop works; no separate intro screen yet |
| Two levels (one fixed, one infinite) | 🔧 | Fixed map works; `map_endless.txt` exists but wrapping is not implemented |
| Score and stats on tally screen | ✅ | Time survived and enemies killed |
| FPS shown throughout | ✅ | Bitmap-font counter, updated every 0.1 s |

### 2. Enemy Entities and Combat
| Feature | Status | Notes |
| --- | --- | --- |
| Static spawners that create NPCs | ⬜ | Enemies are currently placed in `GameController::startGame` |
| 3+ mobile entity types (appearance, health, speed) | 🔧 | Different sprites and speeds; planned Slime / Skeleton / Ogre types not yet split out |
| NPCs move directly towards the player | ✅ | Per-axis movement with wall checks |
| Spawners stop when sufficiently damaged | ⬜ | |
| Melee attack | ✅ | Dwarf: directional swing, one hit per enemy per swing, knockback |
| Projectile attack | 🔧 | `Projectile` struct and update/collision code written, not yet fired by anything |
| Magic attack | ⬜ | Planned for the Wizard |
| Faster movement skill | ✅ | Dwarf secondary: speed boost for 3 s (costs mana) |
| Stronger defence skill | 🔧 | Dwarf secondary also heals 30 HP; no damage reduction yet |
| Magic spell (hits N highest-health enemies) | ⬜ | Planned for the Wizard |
| Entities despawn at zero health | ✅ | Backwards loop with `removeSwap` in `EntityManager::cleanupKills` |

### 3. Collision and Grid Systems
| Feature | Status | Notes |
| --- | --- | --- |
| NPCs damage heroes on contact | ✅ | With invincibility frames and knockback |
| Heroes damage entities while attacking | ✅ | Melee hitbox vs enemy hitbox |
| Impassable terrain blocks | ✅ | Player and enemies |
| Hero projectiles damage entities | 🔧 | Code written in `EntityManager::updateProjectiles`, not yet called |
| Enemy projectiles damage hero | 🔧 | As above |
| Mobile entities vs impassable terrain | ✅ | |
| Tile-based background (32x32 tiles) | ✅ | |
| Multiple tile types, with impassable walls | ✅ | 7 tile definitions, 2 impassable |
| Data-driven map loading from file | ✅ | See [Map File Format](#map-file-format) |
| Fixed-boundary world larger than the screen | ✅ | |
| Unbounded world with repeating tiles | ⬜ | |

### 4. Collectables, Traps and Advanced Features
| Feature | Status |
| --- | --- |
| Health and mana potions | ⬜ |
| Locked door and key | ⬜ |
| Entity that damages the player on contact | ✅ (enemies) |
| Entity that launches projectiles at the player | ⬜ |
| Trigger entity that switches other entities | ⬜ |
| Multiplayer (simultaneous) | ⬜ (per-player `Controls` and a `Vector<Player*>` are already in place) |
| Save and load game state | ⬜ |

*Update this table as features land.*

## Requirements

- Windows 10 or 11 (the framework uses Direct3D 11, XAudio2, XInput and WIC)
- Visual Studio 2022 or later with the **Desktop development with C++** workload
- Platform toolset **v145**, C++ language standard **C++20** (set in the project file)
- A GPU supporting Direct3D 11 feature level 11.0

No external libraries are needed. The framework links `D3D11`, `D3DCompiler`, `WindowsCodecs` and `xinput` itself via `#pragma comment`.

## Building and Running

1. Clone the repository.
2. Open `Cryptmarch.slnx` in Visual Studio.
3. Select a configuration: **Release | x64** for performance measurements, **Debug | x64** for verbose logging and green hitbox outlines.
4. Build with **Ctrl+Shift+B**.
5. Run with **Ctrl+F5**.

The working directory must be the project root so that relative asset paths such as `resources/32rogues/rogues.png` and `resources/maps/map_arena.txt` resolve. Visual Studio uses the project directory by default; if you launch the `.exe` manually, run it from the project root.

The include path is set relative to the project (`$(ProjectDir)include`), so the solution builds from any folder.

## Controls

| Key | Action |
| --- | --- |
| Arrow keys | Move (diagonal movement is normalised) |
| Space | Primary attack (Dwarf: melee swing in the facing direction) |
| Shift | Secondary skill (Dwarf: speed boost + 30 HP heal, costs 25 mana) |
| Enter | Start game from the menu / return to the menu from the tally screen |
| Esc | Quit |

Controls are stored per player in a `Controls` struct, so a second player can be given a different set of keys.

## Project Structure

```
Cryptmarch/
├── Cryptmarch.slnx                 Visual Studio solution
├── Cryptmarch.vcxproj              Project file
├── build-log.md                    Development log (feeds the report)
├── include/
│   ├── GamesEngineeringBase.h      Provided framework (window, input, images, sound, timer)
│   ├── GameController.h            Game state machine (Menu / Play / Tally) and game stats
│   ├── Entities/
│   │   ├── Entity.h                Abstract base: health, i-frames, position, hitbox
│   │   ├── Player.h                Abstract hero: controls, mana, movement, knockback
│   │   ├── Players/Dwarf.h         Melee hero with speed-boost/heal skill
│   │   ├── Enemy.h                 Chases a target entity, contact damage, knockback
│   │   ├── Projectile.h            Projectile data (position, direction, owner, lifetime)
│   │   └── EntityManager.h         Owns all players, enemies and projectiles
│   ├── Map/
│   │   ├── Map.h                   Tile grid, file loading, wall collision, drawing
│   │   └── TileDefinition.h        Tile type: sprite offset and walkability
│   ├── Rendering/
│   │   ├── Graphics.h              Clipped, alpha-blended drawing, text and debug outlines
│   │   └── Camera.h                World-to-screen conversion and follow logic
│   ├── HUD/
│   │   ├── FPS.h                   FPS calculation and bitmap-font display
│   │   └── Status.h                Hero health bar
│   └── Utilities/
│       ├── Vector.h                Own dynamic array template (replaces std::vector)
│       ├── Collisions.h            AABB struct and overlap test
│       ├── AssetManager.h          Loads each sprite sheet once; entities share it
│       └── Logger.h                Level-based logging (Trace/Debug/Info/Error/Critical)
├── src/
│   ├── main.cpp                    Entry point and game loop
│   ├── Entities/                   Entity, Player, Enemy, Players/Dwarf
│   ├── Map/Map.cpp
│   └── Rendering/                  Graphics, Camera
└── resources/
    ├── 32rogues/                   Character, monster, item and tile art (32x32)
    ├── ShooterPack/                Kenney UI sprites (HUD font and health bar)
    ├── maps/                       Level files
    ├── MainMenu.png
    └── Tally.png
```

## Architecture

```
main()  ── game loop
  │
  ├── GameController      state machine: MenuState → PlayState → TallyState → MenuState
  ├── AssetManager        one Image per sprite sheet, shared by pointer
  ├── EntityManager       update → collisions → cleanup → draw
  │     ├── Vector<Player*>      Player (abstract) ← Dwarf
  │     ├── Vector<Enemy*>       Enemy
  │     └── Vector<Projectile>   (by value)
  ├── Map                 tile grid + TileDefinition table
  ├── Camera              follows the first player
  └── HUD                 FPS, Status (health bar)
```

**Inheritance.** `Entity` is an abstract base class with pure virtual `update` and `draw` and a virtual destructor. `Player` adds input, mana and movement and declares a pure virtual `processAttack`, so each hero (Dwarf now; Wizard and Archer planned) supplies its own attack. `Enemy` derives directly from `Entity`.

**Ownership.** Players and enemies are created with `new` and owned by `EntityManager`, which deletes them when they die or when a new game starts. Projectiles are small structs stored by value, so they need no heap allocation.

**Shared assets.** Each sprite sheet is loaded once into `AssetManager`; every entity stores a pointer to the sheet plus its tile offset, instead of each enemy loading its own copy.

## Technical Notes

**Game loop.** Each frame: read delta time once, cap it at 0.05 s (so a long freeze cannot teleport entities through walls), clear the back buffer, update the current state, draw, then present. All movement is scaled by delta time. The FPS counter uses the uncapped delta so it reports the true frame rate.

**Rendering.** Drawing is done in software by writing pixels into the framework's RGB back buffer. `DrawImage` and `DrawTilemapImage` clip against the window edges first, which makes it safe to use the framework's unchecked pixel accessors. They then walk each row with pointers, skip fully transparent pixels, copy opaque ones and alpha-blend partial ones. Sprite sheets must be RGBA (4 channels). `DrawString` and `DrawNumber` render text from the HUD tile sheet.

**Containers.** `Vector<T>` is a hand-written dynamic array. Copying is disabled (rule of three) so two vectors can never free the same buffer.

| Operation | Complexity |
| --- | --- |
| `add` | amortised O(1) (starts at 8, capacity doubles when full) |
| `operator[]` | O(1), bounds-checked with an error log |
| `remove(value)` (keeps order) | O(n) |
| `removeSwap(value)` | O(n) search, O(1) removal |
| `removeSwap(index)` | O(1) |
| `clear` | O(1) (keeps the allocation) |

**Map and tile collision.** The world is a flat, row-major 1D array of tile indices (`index = y * width + x`), looked up through a `TileDefinition` table holding each tile's sprite position and walkability. `isBlockedByWall` converts the corners of an entity's hitbox into tile coordinates and checks only those few tiles, so a wall check costs O(1) regardless of map size, rather than O(n) over every wall.

**Entity collision.** Hitboxes are axis-aligned bounding boxes (`AABB`) computed on demand from the entity's current position, so they can never fall out of sync with it. Two boxes collide only when they overlap on **both** axes. Player-vs-enemy contact is currently a brute-force O(P × E) check, which is fine for a small number of entities.

**Combat.** The Dwarf's melee attack is a three-state machine (Ready → Active → Cooldown). The swing direction is locked when the attack starts, and a per-swing list prevents the same enemy being hit more than once by one swing. Damage uses invincibility frames, and both heroes and enemies are knocked back on hit for clearer feedback. The secondary skill only triggers once per key press (press/release check).

**Camera.** Stores a world-space position; `worldToScreen` subtracts it from an entity's world position. `follow` centres the target and clamps the camera to the map edges on fixed maps.

## Map File Format

Plain text. The first line is the map size in tiles, followed by one row of tile indices per line.

```
<width> <height>
<row 0: width space-separated indices>
<row 1: ...>
...
```

Tile indices refer to positions in the tile definition table:

| Index | Tile | Walkable |
| --- | --- | --- |
| 0 | Background | yes |
| 1-4 | Floor variants 1-4 | yes |
| 5 | Rock wall (vertical) | no |
| 6 | Rock wall (horizontal) | no |

Included maps in `resources/maps/`: `map_arena`, `map_cavern`, `map_crypt`, `map_maze`, `map_endless` (intended for the infinite level), plus the older `map1` and `map2`. The level currently loaded is set in `main.cpp` (`map_arena.txt`).

`map2.txt` uses an older 0/1/2 border/floor/wall scheme and will not display correctly with the current tile table.

## Performance

Measured on the development machine, 1024x768 window. Full detail in [`build-log.md`](build-log.md).

| Scene | Release FPS | Debug FPS |
| --- | --- | --- |
| Player + FPS counter only | – | ~6000 |
| Full-screen 32x24 map, first renderer | ~600 | 60-70 |
| After unchecked pixel access | 900-950 | ~130 |
| After pointer-based row drawing | ~1800 | ~700 |
| 48x36 arena + camera + tile collision | ~1800 | ~700 |
| As above, during player-enemy collisions | ~1700 | – |

Still to measure: FPS against enemy count (e.g. 10 / 50 / 200 / 500) and with many projectiles on screen.

## Assignment Constraints

This project follows the WM908 coursework brief:

- Written in C++ using the provided GamesEngineeringBase framework.
- **No STL data structures** (`vector`, `list`, `map`, etc.) in game code. Containers are implemented from scratch.
- No external APIs or libraries.
- All code is written by the author. No code copied from the internet, other students or AI tools.
- Code is commented and submitted with a report (max 4000 words) and an executable.

## Assets and Credits

- **32rogues** by Seth Boyles. Free for commercial and non-commercial use. Credit is appreciated but not required. Not to be used with generative AI or machine learning projects, and not to be redistributed or resold. See `resources/32rogues/LICENSE.txt`.
- **Desert Shooter Pack** by [Kenney](https://www.kenney.nl), licensed CC0. See `resources/ShooterPack/License.txt`.
- **GamesEngineeringBase** by the MSc Games Engineering Team (MIT licence), provided by the module.

## Known Issues

- **Knockback index bug:** in `EntityManager::update`, the player knockback reads `enemies[i]` (the player index) instead of `enemies[j]`. With more players than enemies this indexes out of range (the `Vector` logs an error and returns element 0).
- **Melee knockback direction:** `Dwarf::processAttack` pushes enemies along `facingDirX/Y` rather than the locked `attackDirX/Y`, so turning mid-swing changes the knockback direction.
- **Projectiles not wired up:** `updateProjectiles` is not called from the game loop and nothing creates projectiles yet. `EntityManager::clear()` also does not clear the projectile list between games.
- **Health bar scale:** `Status` assumes 100 max health, but the Dwarf has 150, so the bar reads full until health drops below 100.
- **Healing cap:** `Entity::heal` does not cap health at `maxHealth` (the Dwarf's skill caps it itself).
- **Map file trust:** the width and height header is trusted; a file with fewer indices than `width × height` is not detected.
- **Brute-force entity collision:** player-vs-enemy checks are O(P × E). A spatial grid would be needed for large enemy counts.
- **Git history** contains a 64.8 MB `.vs` database file from an early commit (removed from tracking afterwards).
