# GauntletProject

A small 2D top-down dungeon game in the style of *Gauntlet* (1985), written in C++ on the **GamesEngineeringBase** framework.

Built for **WM908-15 Programming and Fundamental Algorithms** (WMG, University of Warwick), individual project, 2026/27.

> **Status: work in progress.** See [Feature Status](#feature-status) for what currently works.

---

## Table of Contents

1. [Overview](#overview)
2. [Feature Status](#feature-status)
3. [Requirements](#requirements)
4. [Building and Running](#building-and-running)
5. [Controls](#controls)
6. [Project Structure](#project-structure)
7. [Technical Notes](#technical-notes)
8. [Map File Format](#map-file-format)
9. [Assignment Constraints](#assignment-constraints)
10. [Assets and Credits](#assets-and-credits)
11. [Known Issues](#known-issues)

---

## Overview

The game is built around a core game loop (input, update, draw, present) with a controllable hero, a tile-based world, and a camera that follows the player. The aim is a Gauntlet-like experience: heroes with different strengths, enemy spawners, collectables, doors and keys, traps, and both a fixed-size and an infinite level.

The emphasis of the assignment is on **data structures and algorithms**, so the project uses its own container classes and a custom software renderer built on top of the framework's back buffer, with no STL containers.

## Feature Status

Legend: ✅ done · 🔧 in progress · ⬜ not started

### 1. Core Video Experience and Game Flow
| Feature                                     | Status |
| ------------------------------------------- | ------ |
| Player-controlled hero (movement)           | ✅      |
| Camera follows hero                         | ✅      |
| FPS counter (bitmap font)                   | ✅      |
| Hero weapon and health                      | 🔧     |
| Multiple selectable heroes                  | ⬜      |
| Intro / Menu / Gameplay / Tally screen loop | ⬜      |
| Two levels (one fixed, one infinite)        | ⬜      |
| Score and stats on tally screen             | ⬜      |

### 2. Enemy Entities and Combat
| Feature                                      | Status |
| -------------------------------------------- | ------ |
| Static spawners and mobile NPCs (3+ types)   | ⬜      |
| NPC chase behaviour                          | ⬜      |
| Melee, projectile and magic attacks          | ⬜      |
| Hero skills (speed, defence, targeted spell) | ⬜      |
| Despawn on zero health                       | 🔧     |

### 3. Collision and Grid Systems
| Feature                                    | Status |
| ------------------------------------------ | ------ |
| Tile-based background (32x32 tiles)        | ✅      |
| Multiple tile types, with impassable walls | ✅      |
| Tile collision for the player              | ✅      |
| Data-driven map loading from file          | ✅      |
| Fixed-boundary world                       | ✅      |
| Unbounded world with repeating tiles       | ⬜      |
| Entity vs entity and projectile collisions | ⬜      |

### 4. Collectables, Traps and Advanced Features
| Feature | Status |
| --- | --- |
| Health and mana potions | ⬜ |
| Locked door and key | ⬜ |
| Damaging trap / projectile trap | ⬜ |
| Trigger entity that switches other entities | ⬜ |
| Multiplayer (simultaneous) | ⬜ |
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
2. Open `GauntletProject.slnx` in Visual Studio.
3. Select a configuration (**Release | x64** recommended for performance measurements; **Debug | x64** enables verbose logging).
4. Build with **Ctrl+Shift+B**.
5. Run with **Ctrl+F5**.

The working directory must be the project root so that relative asset paths such as `resources/32rogues/rogues.png` resolve. Visual Studio uses the project directory by default; if you launch the `.exe` manually, run it from the project root.

> **Note for markers:** if the include path in the project settings points to a folder on another machine, set *Project Properties > C/C++ > General > Additional Include Directories* to `$(ProjectDir)include`.

## Controls

| Key | Action |
| --- | --- |
| Arrow keys | Move the hero (diagonals are normalised) |
| Esc | Quit |

*More controls (attack, skills, menu navigation, save/load) will be listed here as they are added.*

## Project Structure

```
GauntletProject/
├── GauntletProject.slnx          Visual Studio solution
├── GauntletProject.vcxproj       Project file
├── include/
│   ├── GamesEngineeringBase.h    Provided framework (window, input, images, sound, timer)
│   ├── Graphics.h                Custom drawing helpers (clipping and alpha blending)
│   ├── Player.h                  Hero class
│   └── Utilities/
│       ├── Vector.h              Own dynamic array template (replaces std::vector)
│       ├── FPS.h                 FPS calculation and bitmap-font display
│       └── Logger.h              Level-based logging (Trace/Debug/Info/Error/Critical)
├── src/
│   ├── main.cpp                  Entry point and game loop
│   ├── Graphics.cpp              DrawImage, DrawTilemapImage
│   └── Player.cpp                Player input, movement, drawing
└── resources/
    ├── 32rogues/                 Character, monster, item and tile art (32x32)
    └── ShooterPack/              Kenney UI sprites (used for the FPS font)
```

## Technical Notes

**Game loop.** Each frame: read the timer's delta time (once per frame), clear the back buffer, update game objects, draw, then present. All movement is scaled by delta time so speed is independent of frame rate.

**Rendering.** Drawing is done in software by writing pixels into the framework's RGB back buffer. `Graphics::DrawImage` and `Graphics::DrawTilemapImage` clip against the window edges, skip fully transparent pixels, write opaque pixels directly and alpha-blend partially transparent ones. `DrawTilemapImage` draws one tile from a sprite sheet by tile coordinates `[x, y]`.

**Containers.** `Vector<T>` is a hand-written dynamic array:

| Operation | Complexity |
| --- | --- |
| `add` | amortised O(1) (capacity doubles when full) |
| `operator[]` | O(1) |
| `remove(value)` (keeps order) | O(n) |
| `removeSwap` (does not keep order) | O(n) by value, O(1) by index |
| `clear` | O(1) |

**Player.** Movement direction is built from key state, normalised when moving diagonally, then scaled by `speed * deltaTime`. Collision against impassable tiles is resolved against the tile grid.

**Map.** The world is a flat 1D array of tile indices (row-major), looked up through a table of tile definitions that records the sprite coordinates and whether each tile is walkable.

## Map File Format

Plain text. The first line is the map size, followed by one row of tile indices per line.

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

Maps written for an older tile table (e.g. a 0/1/2 border/floor/wall scheme) will not load correctly and need converting.

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

- Drawing a full-screen tile map is slow because every tile is drawn pixel by pixel each frame. The plan is to draw only tiles visible on screen and to reduce per-pixel overhead.
- The map is currently loaded from a fixed-size array; runtime-sized maps loaded from file are still being finished.
- `Vector<T>::operator[]` contains a bare `return;` in the out-of-range branch, which does not compile for a function returning `T&` once the template is instantiated. It needs to return a valid reference (for example, the first element) after logging.
