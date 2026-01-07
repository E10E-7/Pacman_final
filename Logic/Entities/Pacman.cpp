// Logic/Entities/Pacman.cpp
#include "Pacman.h"
#include <iostream>

namespace Logic {

    void Pacman::setDirection(const char dir) {
        if (dying) return; // Can't change direction while dying
        if (dir == 'U' || dir == 'D' || dir == 'L' || dir == 'R' || dir == ' ') {
            direction = dir;
        }
    }

    void Pacman::queueDirection(const char dir) {
        if (dying) return; // Can't queue direction while dying
        if (dir == 'U' || dir == 'D' || dir == 'L' || dir == 'R') {
            directionBuffer = dir;
        }
    }

    void Pacman::die() {
        if (dying) return; // Already dying

        dying = true;
        deathTimer = 0.0f;
        direction = ' ';
        moving = false;
        lives--;

        std::cout << "Pacman died! Lives remaining: " << lives << std::endl;
    }

    void Pacman::updateDeath(float deltaTime) {
        if (!dying) return;

        deathTimer += deltaTime;

        // Don't auto-respawn here - let World handle it after checking
        // World will call resetPositions() which calls respawn()
    }

    bool Pacman::isDeathAnimationComplete() const {
        return deathTimer >= deathAnimationDuration;
    }

    void Pacman::respawn() {
        dying = false;
        deathTimer = 0.0f;
        direction = ' ';
        directionBuffer = ' ';
        moving = false;

        // Return to spawn position
        setPosition(spawnX, spawnY);

        std::cout << "Pacman respawned at (" << spawnX << ", " << spawnY << ")" << std::endl;
    }

    void Pacman::update(const float deltaTime) {
        // If dying, only update death animation
        if (dying) {
            updateDeath(deltaTime);
            notify();
            return;
        }

        // If we have no direction, do nothing
        if (direction == ' ') return;

        const float distance = speed * deltaTime;

        // Calculate new position based on CURRENT valid direction
        float nextX = getX();
        float nextY = getY();

        switch (direction) {
            case 'U': nextY -= distance; break;
            case 'D': nextY += distance; break;
            case 'L': nextX -= distance; break;
            case 'R': nextX += distance; break;
            default: break;
        }

        // Move unconditionally (World will revert if we hit a wall)
        setPosition(nextX, nextY);
        moving = true;
        notify();
    }
}