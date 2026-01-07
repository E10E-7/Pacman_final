// Logic/World.h
#ifndef PACMAN_RETRY_WORLD_H
#define PACMAN_RETRY_WORLD_H
#include <memory>
#include <vector>
#include "Logic/Stopwatch.h"
#include "Logic/Score.h"

/**
 * @file World.h
 * @brief Core game logic controller with tile-based movement system
 *
 * This version implements TRUE arcade-style Pac-Man movement:
 * - Discrete tile-based logic positions
 * - Smooth visual interpolation between tiles
 * - Direction changes only at tile centers
 * - Input buffering for responsive controls
 * - Consistent behavior across all systems (Linux/Windows/Fast/Slow PCs)
 */

namespace Logic {
    class AbstractFactory;
    class Pacman;
    class Ghost;
    class Coin;
    class Fruit;
    class Wall;
    class Entity;

    /**
     * @class World
     * @brief Main game logic controller with arcade-accurate tile-based movement
     */
    class World {
    private:
        int difficulty;
        int currentLevel = 1;
        std::shared_ptr<AbstractFactory> factory{};
        std::shared_ptr<Pacman> pacman{};
        std::vector<std::shared_ptr<Ghost>> ghosts{};
        std::vector<std::shared_ptr<Coin>> coins{};
        std::vector<std::shared_ptr<Fruit>> fruits{};
        std::vector<std::shared_ptr<Wall>> walls{};
        bool levelCleared;
        float B_width = 0.0f;   ///< Width of one tile in normalized coordinates
        float B_height = 0.0f;  ///< Height of one tile in normalized coordinates

        std::unique_ptr<Score> scoreSystem;
        bool inFruitMode = false;

        bool waitingToRestart = false;
        bool gameStarted = false;

        // ============================================
        // TILE-BASED MOVEMENT STATE (Arcade Style)
        // ============================================

        /**
         * @brief Progress from current tile to next tile (0.0 to 1.0)
         *
         * 0.0 = At current tile center (can change direction)
         * 0.5 = Halfway to next tile
         * 1.0 = Reached next tile (becomes new current tile, reset to 0.0)
         */
        float pacmanTileProgress = 0.0f;

        /**
         * @brief Current tile X index (column) in the grid
         *
         * Represents Pacman's LOGICAL position, not visual position.
         * Visual position is interpolated between current and next tile.
         */
        int pacmanTileX = 0;

        /**
         * @brief Current tile Y index (row) in the grid
         *
         * Represents Pacman's LOGICAL position, not visual position.
         */
        int pacmanTileY = 0;

        /**
         * @brief Actual movement direction ('U', 'D', 'L', 'R', or ' ' for stopped)
         *
         * This is the direction Pacman is currently moving in.
         * Changes only at tile centers when buffered input is valid.
         */
        char pacmanMoveDir = ' ';

        /**
         * @brief Buffered player input waiting to be applied
         *
         * Player input is stored here and applied at the next tile center
         * if the direction is valid (no wall). This allows "early turning"
         * where you press a direction key before reaching an intersection.
         */
        char pacmanBufferedInput = ' ';

    public:
        explicit World(std::shared_ptr<AbstractFactory> factory);
        ~World() = default;

        /**
         * @brief Main update method called once per frame
         * @param input Character representing player input
         */
        void update(char input);

        /**
         * @brief Initializes level from map file and sets up tile-based state
         */
        void initializeLevel();

        /**
         * @brief Resets Pacman and ghosts to spawn positions
         */
        void resetPositions();

        // Collision detection methods
        bool wouldCollide(const std::shared_ptr<Entity>& ent, char dir, float distance) const;
        bool checkGhostWallCollision(const std::shared_ptr<Ghost>& ghost, char dir) const;
        bool intersectsPickup(const Entity& entity, const Entity& pickup) const;

        // Getters
        [[nodiscard]] int getScore() const { return scoreSystem->getCurrentScore(); }
        [[nodiscard]] int getLives() const;
        [[nodiscard]] int getCurrentLevel() const { return currentLevel; }
        [[nodiscard]] bool isLevelComplete() const { return coins.empty(); }
        [[nodiscard]] bool isGameOver() const;

        void nextLevel();

    private:
        // ============================================
        // TILE-BASED HELPER METHODS
        // ============================================

        /**
         * @brief Converts normalized position to tile indices
         * @param x X position in normalized coordinates [-1, 1]
         * @param y Y position in normalized coordinates [-1, 1]
         * @param outTileX Output tile X index (column)
         * @param outTileY Output tile Y index (row)
         *
         * Example: Position (-0.5, 0.3) might map to tile (3, 7)
         */
        void getTileFromPosition(float x, float y, int& outTileX, int& outTileY) const;

        /**
         * @brief Converts tile indices to normalized position (tile center)
         * @param tileX Tile X index (column)
         * @param tileY Tile Y index (row)
         * @param outX Output X position in normalized coordinates
         * @param outY Output Y position in normalized coordinates
         *
         * Returns the CENTER position of the tile for smooth rendering.
         */
        void getPositionFromTile(int tileX, int tileY, float& outX, float& outY) const;

        /**
         * @brief Checks if a tile contains a wall
         * @param tileX Tile X index to check
         * @param tileY Tile Y index to check
         * @return true if the tile has a wall, false if it's empty/walkable
         *
         * This is the ONLY collision check needed for tile-based movement.
         * No sub-pixel or floating-point collision detection required!
         */
        bool isTileWall(int tileX, int tileY) const;

        /**
         * @brief Updates Pacman using tile-based movement logic
         * @param deltaTime Time elapsed since last frame (seconds)
         * @param input Player input this frame
         *
         * This implements the TRUE arcade Pac-Man movement algorithm:
         *
         * 1. Buffer player input
         * 2. At tile centers (progress = 0.0):
         *    - Try to apply buffered input if valid
         *    - If invalid, continue in current direction
         *    - If blocked, stop moving
         * 3. Between tiles:
         *    - Increment progress based on speed and deltaTime
         *    - When progress >= 1.0, move to next tile
         * 4. Update visual position:
         *    - Interpolate between current and next tile
         *    - Smooth movement regardless of frame rate
         *
         * This system is completely deterministic and works identically
         * on all systems regardless of CPU speed or frame rate.
         */
        void updatePacmanTileBased(float deltaTime, char input);
    };
}
#endif //PACMAN_RETRY_WORLD_H