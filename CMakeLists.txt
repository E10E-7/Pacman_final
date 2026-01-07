cmake_minimum_required(VERSION 3.30)
project(Pacman_Retry)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================
# LOGIC LIBRARY (STANDALONE - NO SFML!)
# ============================================
# This library contains ONLY game logic
# It should compile without SFML installed

add_library(PacmanLogic STATIC
        # Core Logic Files
        Logic/World.cpp
        Logic/World.h
        Logic/AbstractFactory.h
        Logic/Observer.cpp
        Logic/Observer.h
        Logic/StopWatch.cpp
        Logic/Stopwatch.h
        Logic/Score.h
        Logic/Random.h

        # Entity Files
        Logic/Entities/Entity.cpp
        Logic/Entities/Entity.h
        Logic/Entities/Pacman.cpp
        Logic/Entities/Pacman.h
        Logic/Entities/Ghost.cpp
        Logic/Entities/Ghost.h
        Logic/Entities/GhostsTypes.cpp
        Logic/Entities/GhostsTypes.h
        Logic/Entities/Coin.cpp
        Logic/Entities/Coin.h
        Logic/Entities/Fruit.cpp
        Logic/Entities/Fruit.h
        Logic/Entities/Wall.cpp
        Logic/Entities/Wall.h
)

# Set include directories for Logic library
target_include_directories(PacmanLogic PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ============================================
# SFML CONFIGURATION (Only for Representation)
# ============================================
set(SFML_DIR "C:/Users/etien/CLionProjects/Pacman/SFML-2.6.1/lib/cmake/SFML")
find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

# Linux path (if needed)
include_directories(/home/etienne/Documents/SFML-2.6.1/)

# ============================================
# MAIN EXECUTABLE (Uses both Logic + SFML)
# ============================================
add_executable(Pacman_Retry
        main.cpp

        # Representation Files (SFML-dependent)
        Representation/Game.cpp
        Representation/Game.h
        Representation/ConcreteFactory.cpp
        Representation/ConcreteFactory.h
        Representation/Camera.h
        Representation/Camera.cpp

        # State Management
        Representation/StateManager/StateManager.cpp
        Representation/StateManager/StateManager.h
        Representation/StateManager/States/State.cpp
        Representation/StateManager/States/State.h

        # View Classes
        Representation/views/EntityView.cpp
        Representation/views/EntityView.h
        Representation/views/PacmanView.cpp
        Representation/views/PacmanView.h
        Representation/views/GhostView.cpp
        Representation/views/GhostView.h
        Representation/views/CoinView.cpp
        Representation/views/CoinView.h
        Representation/views/FruitView.cpp
        Representation/views/FruitView.h
        Representation/views/WallView.cpp
        Representation/views/WallView.h
)

# Include directories for main executable
target_include_directories(Pacman_Retry PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link Logic library + SFML to executable
target_link_libraries(Pacman_Retry PRIVATE
        PacmanLogic
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
)