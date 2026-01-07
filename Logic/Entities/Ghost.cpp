// Logic/Entities/Ghost.cpp
#include "Ghost.h"
#include "Pacman.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include "../Stopwatch.h"

namespace Logic {

    Ghost::Ghost(float x, float y, float width, float height, float waitTime, int id)
        : Entity(x, y, width, height), state(GhostState::WAITING), startX(x), startY(y),
          spawnTimer(waitTime), originalSpawnTime(waitTime), spriteId(id) {}

    char Ghost::getOppositeDirection() const {
        switch(direction) {
            case 'U': return 'D';
            case 'D': return 'U';
            case 'L': return 'R';
            case 'R': return 'L';
            default: return ' ';
        }
    }

    void Ghost::setFeared(float duration) {
        // Only switch if we are alive and active
        if (state != GhostState::DEAD && state != GhostState::WAITING) {
            state = GhostState::FEARED;
            fearedTimer = duration;
            speed = baseSpeed * 0.5f; // Slower

            // Requirement: "Reverses their direction" immediately
            direction = getOppositeDirection();
        }
    }

    void Ghost::die() {
        state = GhostState::DEAD;
        speed = baseSpeed * 2.5f; // Move fast to respawn
        // Visuals usually hide the body here, leaving only eyes
    }

    void Ghost::despawn() {
        // Called immediately when Pacman dies - makes ghost invisible
        state = GhostState::DESPAWNED;
        direction = ' ';
    }

    void Ghost::resetForPacmanDeath() {
        // Called after death animation completes - reset ghost to spawn with wait timer
        std::cout << "Ghost resetting - position (" << startX << ", " << startY << "), state: WAITING" << std::endl;
        state = GhostState::WAITING;
        speed = baseSpeed;
        setPosition(startX, startY);
        direction = ' ';
        spawnTimer = originalSpawnTime; // Restore original wait time
        fearedTimer = 0.0f; // Clear any feared state
    }


    // Helper functie (zorg dat deze beschikbaar is)
    float getGridCenter(float pos, float size) {
        float shifted = pos + 1.0f;
        int tileIndex = static_cast<int>(shifted / size);
        return (tileIndex * size) + (size / 2.0f) - 1.0f;
    }

    // De standaard update loop die ALLE ghosts gebruiken
    void Ghost::update(const std::vector<char> &valid, const std::shared_ptr<Pacman> &pacman) {
        float dt = Stopwatch::getInstance().getDeltaTime();

        // 1. STATE HANDLING
        if (state == GhostState::DESPAWNED) {
            // Ghost is hidden - do nothing
            notify(); // Still notify so view can handle invisibility
            return;
        }

        if (state == GhostState::WAITING) {
            spawnTimer -= dt;
            if (spawnTimer <= 0) state = GhostState::CHASING;
            else return;
        } else if (state == GhostState::FEARED) {
            fearedTimer -= dt;
            if (fearedTimer <= 0) {
                state = GhostState::CHASING;
                speed = baseSpeed;
            }
        } else if (state == GhostState::DEAD) {
            // Dead logic - ghost eaten by Pacman, returning to spawn
            float dx = startX - getX();
            float dy = startY - getY();
            float dist = std::sqrt(dx*dx + dy*dy);
            if (dist < 0.1f) {
                // Reached spawn - go straight to CHASING (no wait!)
                state = GhostState::CHASING;
                speed = baseSpeed;
                setPosition(startX, startY);
                direction = ' ';
                // NO waiting when eaten by Pacman
            } else {
                float move = speed * 2.5f * dt; // Fast return to spawn
                setPosition(getX() + (dx/dist)*move, getY() + (dy/dist)*move);
            }
            notify();
            return;
        }

        // 2. KIES EERSTE RICHTING
        if (direction == ' ') {
             // Als we bang zijn, kies random, anders vraag het brein
             if (state == GhostState::FEARED) direction = pickRandomDirection(valid);
             else direction = decideDirection(valid, pacman);
        }

        // 3. POSITIE BEREKENINGEN
        float tileW = getWidth();
        float tileH = getHeight();
        float centerX = getGridCenter(getX(), tileW);
        float centerY = getGridCenter(getY(), tileH);
        float moveDist = speed * dt;

        bool approachingCenter = false;
        float distRemaining = 0.0f;

        switch (direction) {
            case 'U': approachingCenter = (getY() > centerY); distRemaining = std::abs(getY() - centerY); break;
            case 'D': approachingCenter = (getY() < centerY); distRemaining = std::abs(getY() - centerY); break;
            case 'L': approachingCenter = (getX() > centerX); distRemaining = std::abs(getX() - centerX); break;
            case 'R': approachingCenter = (getX() < centerX); distRemaining = std::abs(getX() - centerX); break;
        }

        bool isBlocked = std::find(valid.begin(), valid.end(), direction) == valid.end();

        // 4. KRUISPUNT OF BLOKKADE LOGICA
        if (isBlocked && !approachingCenter) {
            setPosition(centerX, centerY);

            // HIER IS HET VERSCHIL: We roepen de virtuele functie aan
            if (state == GhostState::FEARED) direction = pickRandomDirection(valid);
            else direction = decideDirection(valid, pacman);

            notify();
            return;
        }

        if (approachingCenter && moveDist >= distRemaining) {
            setPosition(centerX, centerY);
            float overshoot = moveDist - distRemaining;

            // OOK HIER: Vraag het brein om advies
            if (state == GhostState::FEARED) direction = pickRandomDirection(valid);
            else direction = decideDirection(valid, pacman);

            switch (direction) {
                case 'U': setPosition(getX(), getY() - overshoot); break;
                case 'D': setPosition(getX(), getY() + overshoot); break;
                case 'L': setPosition(getX() - overshoot, getY()); break;
                case 'R': setPosition(getX() + overshoot, getY()); break;
            }
        } else {
            // Gewoon doorlopen
            float nextX = getX();
            float nextY = getY();
            switch (direction) {
                case 'U': nextY -= moveDist; break;
                case 'D': nextY += moveDist; break;
                case 'L': nextX -= moveDist; break;
                case 'R': nextX += moveDist; break;
            }
            setPosition(nextX, nextY);
        }

        // Axis locking
        if (direction == 'U' || direction == 'D') setPosition(centerX, getY());
        else if (direction == 'L' || direction == 'R') setPosition(getX(), centerY);

        notify();
    }

    char Ghost::pickRandomDirection(const std::vector<char>& valid) {
        if (valid.empty()) return ' ';
        return valid[Random::getInstance().nextInt(0, valid.size() - 1)];
    }
}