# Cryptmarch

A 2D top-down dungeon crawler inspired by *Gauntlet*, written in C++ for the WM908 Programming and Fundamental Algorithms coursework. Built on the provided **GamesEngineeringBase** framework, using no STL containers.

## Requirements

- Windows 10 or 11
- Visual Studio 2022 or later with the **Desktop development with C++** workload
- No external libraries needed

## Build and Run

1. Open `Cryptmarch.slnx` in Visual Studio.
2. Choose **Release | x64** (or **Debug | x64** for logging and hitbox outlines).
3. Build with **Ctrl+Shift+B**, then run with **Ctrl+F5**.

If you run the `.exe` directly, launch it from the project root so the `resources/` folder is found.

## Controls

### Menus

| Key | Action |
| --- | --- |
| Up / Down arrows | Change selection |
| Enter | Confirm / start game / return to menu |

### In game

Player 2 only joins when **Multiplayer** is picked on the player select screen.

| Action | Player 1 | Player 2 |
| --- | --- | --- |
| Move up | Up arrow | W |
| Move down | Down arrow | S |
| Move left | Left arrow | A |
| Move right | Right arrow | D |
| Primary attack | L | B |
| Secondary skill (costs mana) | K | V |

## Features

- Three heroes (Dwarf, Archer, Wizard), each with their own primary attack and skill
- Tile-based maps loaded from text files, with wall collision and a following camera
- Enemy spawners, melee and ranged enemies, projectiles and contact damage
- Menu, gameplay and tally-screen state machine, health bar and FPS counter
- Own dynamic array template (`Vector<T>`) in place of `std::vector`

## Credits

- Art: 32rogues tileset (32x32)
- HUD font and UI: Kenney Shooter Pack
- Framework: GamesEngineeringBase (MSc Games Engineering Team, MIT License)
