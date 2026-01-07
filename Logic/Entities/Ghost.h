// Logic/Entities/Ghost.h
#ifndef PACMAN_RETRY_GHOST_H
#define PACMAN_RETRY_GHOST_H

#include "Entity.h"
#include <vector>
#include <memory>
#include <cmath>
#include "../Random.h"

namespace Logic {
    class Pacman;
    class Wall;

    enum class GhostState {
        WAITING,
        CHASING,
        FEARED,
        DEAD,
        DESPAWNED  // Hidden when Pacman dies
    };

    class Ghost : public Entity {
    protected:
        GhostState state;
        char direction = ' ';
        float speed = 0.30f;
        float baseSpeed = 0.30f;
        int spriteId; // 0=Red, 1=Pink, 2=Blue, 3=Orange

        float startX, startY;
        float spawnTimer;
        float originalSpawnTime; // Store original wait time for respawning
        float fearedTimer = 0.0f;

        // Helper voor random keuze (handig voor FEARED state)
        static char pickRandomDirection(const std::vector<char>& valid) ;

        // DIT MOET ELKE GHOST ZELF INVULLEN (Het brein)
        virtual char decideDirection(const std::vector<char>& valid, std::shared_ptr<Pacman> pacman) = 0;

    public:
        [[nodiscard]] char getOppositeDirection() const;
        Ghost(float x, float y, float width, float height, float waitTime, int spriteId);
        ~Ghost() override = default;
        virtual void update(const std::vector<char> &valid, const std::shared_ptr<Pacman> &pacman);
        void setFeared(float duration);
        void die();

        // Override Entity's virtual methods for polymorphic access
        [[nodiscard]] char getDirection() const override { return direction; }
        [[nodiscard]] bool isMoving() const override { return direction != ' '; }
        [[nodiscard]] int getStateInt() const override { return static_cast<int>(state); }
        [[nodiscard]] float getStateTimer() const override { return fearedTimer; }
        [[nodiscard]] int getSpriteId() const override { return spriteId; }

        // Ghost-specific getters (for logic layer use)
        [[nodiscard]] GhostState getState() const { return state; }
        [[nodiscard]] float getSpeed() const { return speed; }
        [[nodiscard]] float getFearedTimer() const { return fearedTimer; }

        // Despawn ghost when Pacman dies (makes invisible)
        void despawn();

        // Reset ghost when Pacman dies (restore wait timers)
        void resetForPacmanDeath();

        // Set base speed (for difficulty scaling)
        void setBaseSpeed(float newSpeed) { baseSpeed = newSpeed; speed = newSpeed; }
    };
}
#endif //PACMAN_RETRY_GHOST_H