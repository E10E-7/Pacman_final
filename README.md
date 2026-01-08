# Pac-Man Game - Advanced Programming Project

**Student:** Etienne  
**Student Number:** 20241443  
**Academic Year:** 2025-2026  
**Course:** Advanced Programming  

## Project Description

This is my implementation of the classic Pac-Man arcade game, built from scratch in C++ using SFML. The project focuses heavily on software architecture and design patterns rather than just getting the game to work. I've tried to create a clean, maintainable codebase that properly separates game logic from rendering code.

The implementation includes all the core gameplay mechanics from the original Pac-Man, including the four different ghost personalities, the fear mode when you eat a fruit, and a scoring system that rewards you for playing aggressively and collecting coins quickly.

## What's Implemented

### Core Gameplay Features

The game includes everything you'd expect from Pac-Man. You control Pac-Man through a maze, collecting coins while avoiding four ghosts. Each ghost has its own AI behavior:

- **Red Ghost:** Moves somewhat randomly, but can lock into a direction at intersections. This creates unpredictable but not overly challenging behavior.
- **Pink Ghost:** Tries to ambush you by targeting the tile in front of where you're heading. Makes you think ahead.
- **Blue Ghost:** Same ambush behavior as Pink, but waits 5 seconds before leaving the spawn area.
- **Orange Ghost:** Takes the direct approach and just chases you, but also waits 10 seconds to spawn.

When you collect a fruit, all the ghosts turn blue and run away from you. During this time you can eat them for bonus points. The first ghost is worth 200 points, then 400, 800, and finally 1600 if you manage to catch all four during one fear mode.

The scoring system rewards fast play. If you collect coins quickly in succession, you get multipliers:
- Under 1 second between coins: 30 points (3x multiplier)
- Under 3 seconds: 20 points (2x multiplier)  
- Over 3 seconds: 10 points (base score)

This creates an interesting risk-reward dynamic where you have to decide whether to play it safe or go for the high score.

### Movement System

One of the biggest technical challenges was getting the movement to feel right. I implemented a tile-based movement system similar to the original arcade game. Pac-Man's position is tracked logically as discrete tile coordinates, but visually it smoothly interpolates between tiles. 

Direction changes can only happen at tile centers, which matches the original game's behavior. The system also includes input buffering, so if you press a direction key slightly before reaching an intersection, it'll remember that and turn as soon as possible. This makes the controls feel responsive even though the movement is grid-based.

The system is completely frame-rate independent using delta time, so the game runs at the same speed whether you have a slow computer or a high-end gaming rig.

### Visual Features

All the graphics use the sprite sheet that was provided with the assignment. Pac-Man has animated mouth movements when he's moving, and there's a proper death animation that plays when you lose a life. The ghosts have walking animations and their sprites change based on which direction they're facing.

When fear mode is about to end (less than 2 seconds remaining), the ghosts start blinking between blue and white to warn you. This gives you a chance to get away before they become dangerous again.

The UI is fully responsive and handles window resizing properly. Everything repositions itself when you change the window size.

### Game States

The game uses a state machine to handle different screens:
- **Menu State:** Shows the top 5 high scores and a play button
- **Level State:** The actual gameplay
- **Pause State:** Overlay that appears when you press escape, lets you continue or return to menu
- **Game Over State:** Lets you enter your name if you got a high score

The state system uses a stack, so pausing the game just pushes a new state on top. When you unpause, it pops that state off and you're right back where you were.

## Technical Architecture

### Separation of Logic and Rendering

The most important architectural decision was keeping game logic completely separate from SFML code. The project is split into two main parts:

The **Logic** namespace contains all the game mechanics. This is compiled as a standalone library that has zero dependencies on SFML. In theory, you could compile this library and use it with a completely different graphics library, or even run it headless for testing.

The **Representation** namespace handles all the visual stuff using SFML. This part depends on the Logic library but not the other way around.

To make this work, all positions in the logic layer use normalized coordinates between -1 and 1. The Camera class then handles converting these to actual pixel positions based on your window size. This means the game logic has no idea what resolution your screen is, which is exactly what we want.

### Design Patterns

The project makes extensive use of design patterns:

**Model-View-Controller:** The separation between game entities (Model), their visual representation (View), and the World class that orchestrates everything (Controller).

**Observer Pattern:** Used in two ways. First, for keeping views synchronized with models - when an entity moves, it notifies its view to redraw itself. Second, for score calculation - the Score class observes game events like coin collection and calculates points accordingly.

**Abstract Factory:** The ConcreteFactory creates entities and automatically attaches their views. The World class can create entities without knowing anything about SFML.

**Singleton:** The Stopwatch and Random classes use singleton pattern since we only ever need one instance of each and they need to be accessible from anywhere.

**State Pattern:** The StateManager implements a state machine for handling different game screens.

### Code Quality

I've tried to follow best practices throughout:

- Smart pointers are used everywhere instead of raw pointers. Shared pointers for entities that are owned by multiple objects, unique pointers for exclusive ownership, and weak pointers in the Observer pattern to avoid circular references.

- Const correctness is maintained. Methods that don't modify state are marked const, and parameters are passed by const reference where appropriate.

- Exception handling is included for file operations and resource loading, with proper error messages.

- The code is extensively documented with Doxygen-style comments explaining not just what the code does but why certain design decisions were made.

## Project Structure
```
├── Logic/                      (Standalone library - no SFML)
│   ├── World.cpp/h            (Main game orchestrator)
│   ├── Observer.cpp/h         (Observer pattern implementation)
│   ├── Stopwatch.cpp/h        (Delta time management)
│   ├── Score.h                (Score calculation and persistence)
│   ├── Random.h               (Random number generation)
│   ├── AbstractFactory.h      (Factory interface)
│   └── Entities/              
│       ├── Entity.cpp/h       (Base class for all game objects)
│       ├── Pacman.cpp/h       (Pac-Man logic)
│       ├── Ghost.cpp/h        (Base ghost class with shared behavior)
│       ├── GhostsTypes.cpp/h  (Four specific ghost AI implementations)
│       ├── Coin.cpp/h         
│       ├── Fruit.cpp/h        
│       └── Wall.cpp/h         
│
├── Representation/             (SFML-dependent rendering)
│   ├── Game.cpp/h             (Main game setup and loop)
│   ├── Camera.cpp/h           (Coordinate transformation)
│   ├── ConcreteFactory.cpp/h  (Creates entities with views attached)
│   ├── StateManager/          
│   │   ├── StateManager.cpp/h (State machine controller)
│   │   └── States/
│   │       └── State.cpp/h    (Menu, Level, Pause, GameOver states)
│   └── views/                 
│       ├── EntityView.cpp/h   (Base class for all views)
│       ├── PacmanView.cpp/h   (Handles Pac-Man rendering and animation)
│       ├── GhostView.cpp/h    (Handles ghost rendering and animation)
│       ├── CoinView.cpp/h     
│       ├── FruitView.cpp/h    
│       └── WallView.cpp/h     
│
├── assets/                     
│   ├── spritesheet.png        (Game sprites)
│   ├── Map1                   (Level layout file)
│   └── DejaVuSans.ttf        (Font for UI text)
│
├── main.cpp                   (Entry point)
├── CMakeLists.txt            (Build configuration)
└── .circleci/
    └── config.yml            (CI/CD configuration)
```

## Building and Running

### On Linux (Ubuntu)

First, install the required dependencies:
```bash
sudo apt update
sudo apt install build-essential cmake libsfml-dev
```

Then clone and build the project:
```bash
git clone [YOUR-GITHUB-REPO-URL]
cd Pacman_Retry
mkdir build
cd build
cmake ..
make
```

Run the game:
```bash
./Pacman_Retry
```

### On Windows

Download SFML 2.6.1 from the official website and extract it somewhere (like C:/SFML-2.6.1).

Edit CMakeLists.txt line 40 to point to your SFML installation:
```cmake
set(SFML_DIR "C:/Path/To/Your/SFML-2.6.1/lib/cmake/SFML")
```

Build the project:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

You'll need to copy the SFML DLLs to the build directory before running the executable.

### Reference Platform

The project is tested on the university's reference platform:
- Ubuntu 24.04.3 LTS
- SFML 2.6.1
- CMake 3.28.3
- G++ 13.2.0 or Clang 18.0.0

## Continuous Integration

The project uses CircleCI for automated builds. Every commit is automatically tested to make sure:
- The code compiles successfully on Ubuntu 24.04.3
- All dependencies link correctly
- The executable is generated properly
- No build warnings or errors occur

The build status badge at the top of this README shows whether the latest commit built successfully.

## Contact

If you have questions about the implementation or find any issues, feel free to reach out:

Email: Etienne.ElHachem@student.uantwerpen.be

---
