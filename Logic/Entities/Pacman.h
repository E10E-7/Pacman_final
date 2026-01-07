// Logic/Entities/Pacman.h
#ifndef PACMAN_RETRY_PACMAN_H
#define PACMAN_RETRY_PACMAN_H

#include "Entity.h"

namespace Logic {
    class Pacman final : public Entity {
    private:
        char direction = ' ';
        char directionBuffer = ' '; // Stores the next intended move
        float speed = 0.5f;
        bool moving = false;

        // Lives and death
        int lives = 3;
        bool dying = false;
        float deathTimer = 0.0f;
        const float deathAnimationDuration = 1.0f; // Total death animation time

        // Spawn position
        float spawnX, spawnY;

    public:
        Pacman(const float x, const float y, const float width, const float height)
            : Entity(x, y, width, height), spawnX(x), spawnY(y) {}

        [[nodiscard]] char getDirection() const override { return direction; }
        [[nodiscard]] bool isMoving() const override { return moving; }
        void setMoving(const bool move) { moving = move; }
        [[nodiscard]] char getQueuedDirection() const { return directionBuffer; }
        [[nodiscard]] float getSpeed() const { return speed; }

        // Death and lives
        [[nodiscard]] bool isDying() const override { return dying; }
        [[nodiscard]] int getLives() const override { return lives; }
        [[nodiscard]] float getDeathTimer() const { return deathTimer; }
        void die();
        void respawn();
        void updateDeath(float deltaTime);
        [[nodiscard]] bool isDeathAnimationComplete() const;

        // Sets the ACTUAL direction (called only when valid)
        void setDirection(char dir);

        // Sets the BUFFERED direction (called on key press)
        void queueDirection(char dir);

        void update(float deltaTime);
    };
}

#endif //PACMAN_RETRY_PACMAN_H