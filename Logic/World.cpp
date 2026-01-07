// Logic/World.cpp - Fixed collision detection
#include "World.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include "AbstractFactory.h"
#include "Entities/Coin.h"
#include "Entities/Fruit.h"
#include "Entities/Pacman.h"
#include "Entities/Wall.h"
#include "Entities/Ghost.h"

int countLines(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open map file: " + fileName);
    }
    int lineCount = 0;
    std::string line;
    while (std::getline(file, line)) lineCount++;
    return lineCount;
}

int getWidth(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open map file: " + fileName);
    }
    if (std::string line; std::getline(file, line)) return static_cast<int>(line.length());
    return 0;
}

float getCenteredPosition(float currentPos, float size) {
    float shifted = currentPos + 1.0f;
    int tileIndex = static_cast<int>(shifted / size);
    float center = (tileIndex * size) + (size / 2.0f);
    return center - 1.0f;
}

namespace Logic {
    World::World(std::shared_ptr<AbstractFactory> factory)
        : difficulty(1), factory(std::move(factory)), levelCleared(false), gameStarted(false) {

        scoreSystem = std::make_unique<Score>();

        try {
            initializeLevel();
        } catch (const std::exception& e) {
            std::cerr << "Error initializing level: " << e.what() << std::endl;
            throw;
        }
    }

    void World::initializeLevel() {
        std::string filename = "../assets/Map1";

        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open map file: " + filename);
            }

            auto W_width = static_cast<float>(getWidth(filename));
            auto W_height = static_cast<float>(countLines(filename));
            B_width = 2.0f / W_width;
            B_height = 2.0f / W_height;

            float y = -1.0f + B_height/2.0f;
            std::string line;
            while (std::getline(file, line)) {
                float x = -1.0f + B_width/2.0f;
                for (char& ch : line) {
                    switch (ch) {
                        case '#': walls.push_back(factory->createWall(x, y, B_width, B_height)); break;
                        case '0': coins.push_back(factory->createCoin(x, y, B_width, B_height)); break;
                        case 'P':
                            pacman = factory->createPacMan(x, y, B_width, B_height);
                            // Initialize Pacman's tile position
                            getTileFromPosition(x, y, pacmanTileX, pacmanTileY);
                            pacmanTileProgress = 0.0f;
                            pacmanMoveDir = ' ';
                            pacmanBufferedInput = ' ';
                            break;
                        case 'f': fruits.push_back(factory->createFruit(x, y, B_width, B_height)); break;
                        case 'R': case 'I': case 'B': case 'O':
                            ghosts.push_back(factory->createGhost(x, y, B_width, B_height, ch));
                            break;
                        case ' ': break;
                        default: if(ch != '\r' && ch != '\n') std::cerr << "Unknown: " << ch << std::endl;
                    }
                    x += B_width;
                }
                y += B_height;
            }
            std::cout << "Level initialized with " << ghosts.size() << " ghosts." << std::endl;

            gameStarted = false;

        } catch (const std::exception& e) {
            std::cerr << "Exception in initializeLevel: " << e.what() << std::endl;
            throw;
        }
    }

    void World::resetPositions() {
        std::cout << "=== RESET POSITIONS CALLED ===" << std::endl;

        // Reset Pacman to spawn position
        pacman->respawn();

        // Reset tile-based movement state
        getTileFromPosition(pacman->getX(), pacman->getY(), pacmanTileX, pacmanTileY);
        pacmanTileProgress = 0.0f;
        pacmanMoveDir = ' ';
        pacmanBufferedInput = ' ';

        // Reset all ghosts to spawn with their wait timers
        for (auto& ghost : ghosts) {
            ghost->resetForPacmanDeath();
            ghost->notify();
        }

        waitingToRestart = true;
        gameStarted = false;
        std::cout << "Positions reset. Waiting for player input to restart..." << std::endl;
    }

    int World::getLives() const {
        return pacman ? pacman->getLives() : 0;
    }

    bool World::isGameOver() const {
        return pacman && pacman->getLives() <= 0 && pacman->isDying() && pacman->isDeathAnimationComplete();
    }

    void World::nextLevel() {
        std::cout << "=== LEVEL COMPLETE! ===" << std::endl;
        std::cout << "Moving to level " << (currentLevel + 1) << std::endl;

        scoreSystem->onLevelCleared();

        currentLevel++;

        float speedMultiplier = 1.0f + (currentLevel - 1) * 0.15f;
        float newGhostSpeed = 0.30f * speedMultiplier;

        std::cout << "Ghost speed: " << newGhostSpeed << " (multiplier: " << speedMultiplier << "x)" << std::endl;

        coins.clear();
        fruits.clear();
        ghosts.clear();

        try {
            initializeLevel();
        } catch (const std::exception& e) {
            std::cerr << "Error loading next level: " << e.what() << std::endl;
            throw;
        }

        for (auto& ghost : ghosts) {
            ghost->setBaseSpeed(newGhostSpeed);
        }

        pacman->respawn();
        waitingToRestart = true;
        gameStarted = false;

        std::cout << "Level " << currentLevel << " ready! Score: " << scoreSystem->getCurrentScore() << std::endl;
    }

    bool World::wouldCollide(const std::shared_ptr<Entity>& ent, const char dir, const float distance) const {
        // Calculate test position
        float testX = ent->getX();
        float testY = ent->getY();

        switch (dir) {
            case 'U': testY -= distance; break;
            case 'D': testY += distance; break;
            case 'L': testX -= distance; break;
            case 'R': testX += distance; break;
            default: return false;
        }

        // Use standard collision box with small tolerance
        float w = ent->getWidth();
        float h = ent->getHeight();
        float tolerance = 0.08f; // Slightly bigger tolerance than before

        float left = testX - w/2.0f + tolerance;
        float right = testX + w/2.0f - tolerance;
        float top = testY - h/2.0f + tolerance;
        float bottom = testY + h/2.0f - tolerance;

        // Check collision with all walls
        for (const auto& wall : walls) {
             float wLeft = wall->getX() - wall->getWidth()/2.0f;
             float wRight = wall->getX() + wall->getWidth()/2.0f;
             float wTop = wall->getY() - wall->getHeight()/2.0f;
             float wBottom = wall->getY() + wall->getHeight()/2.0f;

             // AABB collision test
             if (right > wLeft && left < wRight && bottom > wTop && top < wBottom) {
                 return true;
             }
        }
        return false;
    }

    bool World::checkGhostWallCollision(const std::shared_ptr<Ghost>& ghost, char dir) const {
        float cx = getCenteredPosition(ghost->getX(), B_width);
        float cy = getCenteredPosition(ghost->getY(), B_height);

        float nx = cx;
        float ny = cy;
        switch (dir) {
            case 'U': ny -= B_height; break;
            case 'D': ny += B_height; break;
            case 'L': nx -= B_width; break;
            case 'R': nx += B_width; break;
            default: return false;
        }

        float epsilon = B_width * 0.25f;
        for (const auto& wall : walls) {
            if (std::abs(wall->getX() - nx) < epsilon &&
                std::abs(wall->getY() - ny) < epsilon) {
                return true;
            }
        }
        return false;
    }

    bool World::intersectsPickup(const Entity& entity, const Entity& pickup) const {
        float shrinkFactor = 0.5f;

        float w = entity.getWidth() * shrinkFactor;
        float h = entity.getHeight() * shrinkFactor;

        float left = entity.getX() - w / 2.0f;
        float right = entity.getX() + w / 2.0f;
        float top = entity.getY() - h / 2.0f;
        float bottom = entity.getY() + h / 2.0f;

        float pickupLeft = pickup.getX() - pickup.getWidth() / 2.0f;
        float pickupRight = pickup.getX() + pickup.getWidth() / 2.0f;
        float pickupTop = pickup.getY() - pickup.getHeight() / 2.0f;
        float pickupBottom = pickup.getY() + pickup.getHeight() / 2.0f;

        return !(right <= pickupLeft || left >= pickupRight ||
                 bottom <= pickupTop || top >= pickupBottom);
    }

    void World::update(const char input) {
        Stopwatch& stopwatch = Stopwatch::getInstance();
        stopwatch.update();
        const float deltaTime = stopwatch.getDeltaTime();

        scoreSystem->update(deltaTime);

        // If Pacman is dying, update death animation but still draw everything
        if (pacman->isDying()) {
            pacman->update(deltaTime);

            if (pacman->isDeathAnimationComplete()) {
                if (pacman->getLives() > 0) {
                    scoreSystem->onPacmanDied();
                    resetPositions();
                } else {
                    std::cout << "Game Over! Final Score: " << scoreSystem->getCurrentScore() << std::endl;
                }
            }

            pacman->notify();
            for (const auto& wall : walls) wall->notify();
            for (const auto& coin : coins) coin->notify();
            for (const auto& fruit : fruits) fruit->notify();
            for (const auto& ghost : ghosts) ghost->notify();

            return;
        }

        // If waiting to restart, only start when player presses a direction key
        if (waitingToRestart) {
            if (input != 0 && input != ' ') {
                waitingToRestart = false;
                gameStarted = true;
                std::cout << "Game resumed!" << std::endl;
            } else {
                pacman->notify();
                for (const auto& wall : walls) wall->notify();
                for (const auto& coin : coins) coin->notify();
                for (const auto& fruit : fruits) fruit->notify();
                for (const auto& ghost : ghosts) ghost->notify();
                return;
            }
        }

        // Check if game has started
        if (!gameStarted) {
            if (input != 0 && input != ' ') {
                gameStarted = true;
                std::cout << "Game started! Ghosts will now move." << std::endl;
            } else {
                pacman->notify();
                for (const auto& wall : walls) wall->notify();
                for (const auto& coin : coins) coin->notify();
                for (const auto& fruit : fruits) fruit->notify();
                for (const auto& ghost : ghosts) ghost->notify();
                return;
            }
        }

        // --- TILE-BASED PACMAN MOVEMENT (True Arcade Style) ---
        updatePacmanTileBased(deltaTime, input);

        // --- GHOST UPDATE ---
        if (gameStarted) {
            for (const auto& ghost : ghosts) {
                if (intersectsPickup(*pacman, *ghost)) {
                    if (ghost->getState() == GhostState::FEARED) {
                        scoreSystem->onGhostEaten();
                        ghost->die();
                    } else if (ghost->getState() == GhostState::CHASING) {
                        pacman->die();

                        for (auto& g : ghosts) {
                            g->despawn();
                        }

                        inFruitMode = false;
                        return;
                    }
                }

                std::vector<char> valid = {'U', 'D', 'L', 'R'};
                std::erase_if(valid, [&](char dir) {
                    return checkGhostWallCollision(ghost, dir);
                });

                if (valid.size() > 1) {
                    char opposite = ghost->getOppositeDirection();
                    std::erase_if(valid, [&](char dir) { return dir == opposite; });
                }

                ghost->update(valid, pacman);
            }
        }

        // --- COIN COLLECTION ---
        std::erase_if(coins, [&](const std::shared_ptr<Coin>& coin) {
             if (intersectsPickup(*pacman, *coin)) {
                 // Don't notify - coin will be destroyed immediately
                 scoreSystem->onCoinCollected();
                 return true;  // Remove from vector
             }
             return false;
        });

        // --- FRUIT COLLECTION ---
        std::erase_if(fruits, [&](const std::shared_ptr<Fruit>& fruit) {
             if (intersectsPickup(*pacman, *fruit)) {
                 // Don't notify - fruit will be destroyed immediately
                 scoreSystem->onFruitCollected();

                 inFruitMode = true;

                 for(auto& g : ghosts) g->setFeared(5.0f);
                 return true;  // Remove from vector
             }
             return false;
        });

        // Check if we've exited fruit mode
        if (inFruitMode) {
            bool anyFeared = false;
            for (const auto& g : ghosts) {
                if (g->getState() == GhostState::FEARED) {
                    anyFeared = true;
                    break;
                }
            }
            if (!anyFeared) {
                inFruitMode = false;
            }
        }

        // Notify remaining entities
        for (const auto& wall : walls) wall->notify();
        for (const auto& coin : coins) coin->notify();
        for (const auto& fruit : fruits) fruit->notify();
        for (const auto& ghost : ghosts) ghost->notify();
    }

    // ============================================
    // TILE-BASED HELPER METHODS
    // ============================================

    void World::getTileFromPosition(float x, float y, int& outTileX, int& outTileY) const {
        // Convert normalized position [-1, 1] to tile indices
        float shiftedX = x + 1.0f;  // [0, 2]
        float shiftedY = y + 1.0f;  // [0, 2]

        outTileX = static_cast<int>(shiftedX / B_width);
        outTileY = static_cast<int>(shiftedY / B_height);
    }

    void World::getPositionFromTile(int tileX, int tileY, float& outX, float& outY) const {
        // Convert tile indices to normalized position (tile center)
        outX = -1.0f + (tileX * B_width) + (B_width / 2.0f);
        outY = -1.0f + (tileY * B_height) + (B_height / 2.0f);
    }

    bool World::isTileWall(int tileX, int tileY) const {
        // Get position of this tile
        float x, y;
        getPositionFromTile(tileX, tileY, x, y);

        // Check if any wall occupies this tile
        float epsilon = B_width * 0.3f;
        for (const auto& wall : walls) {
            if (std::abs(wall->getX() - x) < epsilon &&
                std::abs(wall->getY() - y) < epsilon) {
                return true;
            }
        }
        return false;
    }

    void World::updatePacmanTileBased(float deltaTime, char input) {
        // Step 1: Buffer player input
        if (input != 0 && input != ' ') {
            pacmanBufferedInput = input;
        }

        // Step 2: Calculate movement speed (in tiles per second)
        const float tilesPerSecond = 5.0f;  // Adjust this for speed
        float tileProgressDelta = tilesPerSecond * deltaTime;

        // Step 3: Check for INSTANT REVERSAL (opposite direction)
        bool instantReversal = false;
        if (pacmanBufferedInput != ' ' && pacmanMoveDir != ' ' && pacmanTileProgress > 0.0f) {
            // Check if buffered input is opposite to current direction
            if ((pacmanMoveDir == 'U' && pacmanBufferedInput == 'D') ||
                (pacmanMoveDir == 'D' && pacmanBufferedInput == 'U') ||
                (pacmanMoveDir == 'L' && pacmanBufferedInput == 'R') ||
                (pacmanMoveDir == 'R' && pacmanBufferedInput == 'L')) {

                // INSTANT REVERSAL!
                // Visual position MUST stay the same!
                //
                // Before: currentTile + (nextTile - currentTile) * progress
                // After:  nextTile + (currentTile - nextTile) * (1 - progress)
                //
                // Example: 30% from A(5.0) to B(6.0) = 5.0 + 1.0*0.3 = 5.3
                // After:   70% from B(6.0) to A(5.0) = 6.0 + (-1.0)*0.7 = 5.3 ✓

                // Move to next tile
                switch (pacmanMoveDir) {
                    case 'U': pacmanTileY--; break;
                    case 'D': pacmanTileY++; break;
                    case 'L': pacmanTileX--; break;
                    case 'R': pacmanTileX++; break;
                }

                // Reverse direction
                pacmanMoveDir = pacmanBufferedInput;
                pacmanBufferedInput = ' ';

                // Flip progress: 30% becomes 70%
                pacmanTileProgress = 1.0f - pacmanTileProgress;

                instantReversal = true;
                pacman->setDirection(pacmanMoveDir);
            }
        }

        // Step 4: Check if we're at a tile center (can change direction normally)
        if (!instantReversal && pacmanTileProgress == 0.0f) {
            // We're at a tile center - try to apply buffered input
            if (pacmanBufferedInput != ' ' && pacmanBufferedInput != pacmanMoveDir) {
                // Calculate target tile for buffered direction
                int targetTileX = pacmanTileX;
                int targetTileY = pacmanTileY;

                switch (pacmanBufferedInput) {
                    case 'U': targetTileY--; break;
                    case 'D': targetTileY++; break;
                    case 'L': targetTileX--; break;
                    case 'R': targetTileX++; break;
                }

                // Check if target tile is valid (not a wall)
                if (!isTileWall(targetTileX, targetTileY)) {
                    // Direction change is valid!
                    pacmanMoveDir = pacmanBufferedInput;
                    pacmanBufferedInput = ' ';  // Clear buffer
                    pacman->setDirection(pacmanMoveDir);
                }
            }

            // If still no direction, try current direction
            if (pacmanMoveDir != ' ') {
                int targetTileX = pacmanTileX;
                int targetTileY = pacmanTileY;

                switch (pacmanMoveDir) {
                    case 'U': targetTileY--; break;
                    case 'D': targetTileY++; break;
                    case 'L': targetTileX--; break;
                    case 'R': targetTileX++; break;
                }

                // Check if we can continue in current direction
                if (isTileWall(targetTileX, targetTileY)) {
                    // Hit a wall - stop moving
                    pacmanMoveDir = ' ';
                    pacman->setDirection(' ');
                    pacman->setMoving(false);
                }
            }
        }

        // Step 5: Move along current direction
        if (pacmanMoveDir != ' ') {
            pacmanTileProgress += tileProgressDelta;
            pacman->setMoving(true);

            // Check if we've completed moving to next tile
            if (pacmanTileProgress >= 1.0f) {
                // Move to next tile
                switch (pacmanMoveDir) {
                    case 'U': pacmanTileY--; break;
                    case 'D': pacmanTileY++; break;
                    case 'L': pacmanTileX--; break;
                    case 'R': pacmanTileX++; break;
                }

                pacmanTileProgress = 0.0f;  // Reset progress
            }
        } else {
            pacman->setMoving(false);
        }

        // Step 6: Calculate visual position (interpolate between tiles)
        float currentX, currentY;
        getPositionFromTile(pacmanTileX, pacmanTileY, currentX, currentY);

        if (pacmanMoveDir != ' ' && pacmanTileProgress > 0.0f) {
            // Calculate next tile position
            int nextTileX = pacmanTileX;
            int nextTileY = pacmanTileY;

            switch (pacmanMoveDir) {
                case 'U': nextTileY--; break;
                case 'D': nextTileY++; break;
                case 'L': nextTileX--; break;
                case 'R': nextTileX++; break;
            }

            float nextX, nextY;
            getPositionFromTile(nextTileX, nextTileY, nextX, nextY);

            // Linear interpolation
            currentX = currentX + (nextX - currentX) * pacmanTileProgress;
            currentY = currentY + (nextY - currentY) * pacmanTileProgress;
        }

        // Update Pacman's visual position
        pacman->setPosition(currentX, currentY);
        pacman->notify();
    }
}