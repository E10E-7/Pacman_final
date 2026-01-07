//
// PacmanView.cpp - With death animation and exception handling
//

#include "PacmanView.h"
#include "Logic/Stopwatch.h"
#include <stdexcept>

namespace Render {
    PacmanView::PacmanView(const std::shared_ptr<Logic::Entity>& entity,
                           const std::shared_ptr<Camera>& cam,
                           sf::RenderWindow& win)
        : EntityView(entity, cam, win), currentFrame(0), animationSpeed(0.15f),
          animationAccumulator(0.0f) {

        try {
            if (!texture.loadFromFile("../assets/spritesheet.png")) {
                throw std::runtime_error("Failed to load spritesheet.png for Pacman");
            }

            sprite.setTexture(texture);

            // Initialize with first frame facing right (full circle)
            setFrame(2*16, 0, 16, 16);
        } catch (const std::exception& e) {
            std::cerr << "Exception in PacmanView constructor: " << e.what() << std::endl;
            throw;
        }
    }

    void PacmanView::updateAnimation() {
        // Get the entity - no casting needed!
        auto entity = model.lock();
        if (!entity) return;

        // Check if dying
        bool dying = entity->isDying();

        if (dying) {
            // Death animation - 12 frames
            animationAccumulator += Logic::Stopwatch::getInstance().getDeltaTime();

            // Each frame lasts ~0.083 seconds (1.0s total / 12 frames)
            const float deathFrameDuration = 1.0f / 12.0f;

            if (animationAccumulator >= deathFrameDuration) {
                currentFrame++;
                if (currentFrame > 11) currentFrame = 11; // Stay on last frame
                animationAccumulator -= deathFrameDuration;
            }

            updateDeathSprite();
            return;
        }

        // Reset death animation when not dying
        if (currentFrame > 2) {
            currentFrame = 0;
            animationAccumulator = 0.0f;
        }

        // Normal animation
        bool moving = entity->isMoving();
        char direction = entity->getDirection();

        // Only animate if Pacman is moving
        if (!moving) {
            // Reset to closed mouth (full circle) when not moving
            currentFrame = 0;
            animationAccumulator = 0.0f;
            updateSpriteForDirection(direction);
            return;
        }

        // Accumulate time using Stopwatch singleton
        animationAccumulator += Logic::Stopwatch::getInstance().getDeltaTime();

        // Update animation frame when enough time has passed
        if (animationAccumulator >= animationSpeed) {
            currentFrame = (currentFrame + 1) % 3; // 3 frames per direction
            animationAccumulator -= animationSpeed; // Keep remainder
        }

        updateSpriteForDirection(direction);
    }

    void PacmanView::updateDeathSprite() {
        // Death animation coordinates:
        // 1 = (2*16,0), 2 = (3*16,0), 3 = (4*16,0), 4 = (5*16,0),
        // 5 = (6*16,0), 6 = (7*16,0), 7 = (8*16,0), 8 = (9*16,0),
        // 9 = (10*16,0), 10 = (11*16,0), 11 = (12*16,0), 12 = (13*16,0)

        int frameX = (2 + currentFrame) * 16; // Start at column 2, add currentFrame (0-11)
        int frameY = 0;

        setFrame(frameX, frameY, 16, 16);
    }

    void PacmanView::updateSpriteForDirection(char direction) {
        int frameX = 0;
        int frameY = 0;

        switch(direction) {
            case 'R': // Moving right
                if (currentFrame == 0) {
                    frameX = 2*16; frameY = 0;        // (32, 0)
                } else if (currentFrame == 1) {
                    frameX = 1*16; frameY = 0;        // (16, 0)
                } else {
                    frameX = 0; frameY = 0;           // (0, 0)
                }
                break;

            case 'L': // Moving left
                if (currentFrame == 0) {
                    frameX = 2*16; frameY = 0;        // (32, 0)
                } else if (currentFrame == 1) {
                    frameX = 1*16; frameY = 1*16;     // (16, 16)
                } else {
                    frameX = 0; frameY = 1*16;        // (0, 16)
                }
                break;

            case 'U': // Moving up
                if (currentFrame == 0) {
                    frameX = 2*16; frameY = 0;        // (32, 0)
                } else if (currentFrame == 1) {
                    frameX = 3*16; frameY = 1*16;     // (48, 16)
                } else {
                    frameX = 2*16; frameY = 1*16;     // (32, 16)
                }
                break;

            case 'D': // Moving down
                if (currentFrame == 0) {
                    frameX = 2*16; frameY = 0;        // (32, 0)
                } else if (currentFrame == 1) {
                    frameX = 5*16; frameY = 1*16;     // (80, 16)
                } else {
                    frameX = 4*16; frameY = 1*16;     // (64, 16)
                }
                break;

            default: // No direction, full circle
                frameX = 2*16;
                frameY = 0;
                currentFrame = 0;
                break;
        }

        setFrame(frameX, frameY, 16, 16);
    }

    void PacmanView::setFrame(const int frameX, const int frameY,
                              const int frameWidth, const int frameHeight) {
        sprite.setTextureRect(sf::IntRect(frameX, frameY, frameWidth, frameHeight));
    }

    void PacmanView::draw() {
        // Update animation before drawing
        updateAnimation();

        // Call the parent draw method to handle positioning and scaling
        EntityView::draw();
    }

    void PacmanView::onNotify() {
        // When notified by the model, update animation and draw
        draw();
    }
}