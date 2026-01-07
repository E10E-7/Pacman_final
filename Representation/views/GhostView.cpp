//
// GhostView.cpp - Using Stopwatch singleton for timing
//

#include "GhostView.h"
#include "Logic/Stopwatch.h"
#include <iostream>

namespace Render {
    GhostView::GhostView(const std::shared_ptr<Logic::Entity>& entity,
                         const std::shared_ptr<Camera>& cam,
                         sf::RenderWindow& win,
                         int id)
        : EntityView(entity, cam, win), spriteId(id), currentFrame(0),
          animationSpeed(0.2f), animationAccumulator(0.0f),
          blinkAccumulator(0.0f), isWhiteFeared(false) {

        if (!texture.loadFromFile("../assets/spritesheet.png")) {
            std::cerr << "Failed to load ghost texture!" << std::endl;
        }
        sprite.setTexture(texture);

        // Set initial frame
        updateSpriteForState();
    }

    void GhostView::updateAnimation() {
        // Accumulate time using Stopwatch singleton
        float deltaTime = Logic::Stopwatch::getInstance().getDeltaTime();
        animationAccumulator += deltaTime;

        // Update animation frame when enough time has passed
        if (animationAccumulator >= animationSpeed) {
            currentFrame = (currentFrame + 1) % 2; // 2 frames for walking animation
            animationAccumulator -= animationSpeed; // Keep remainder
        }

        updateSpriteForState();
    }

    void GhostView::updateSpriteForState() {
        // Get entity using polymorphic interface - NO CASTING!
        auto entity = model.lock();
        if (!entity) return;

        // Use polymorphic methods from Entity base class
        int stateInt = entity->getStateInt();
        char direction = entity->getDirection();

        // Convert state int back to enum for readability
        // 0=WAITING, 1=CHASING, 2=FEARED, 3=DEAD

        int srcX = 0;
        int srcY = 0;

        // IMPORTANT: FEARED and DEAD states use the SAME sprites for ALL ghost colors!
        // Only the CHASING and WAITING states use color-specific rows (2, 3, 4, 5)

        if (stateInt == 3) { // DEAD
            // Show only eyes when dead
            // ALL ghosts use the same eye sprites at row 3 (Y=48)
            srcY = 3 * 16; // Always row 3 (48) for all ghost colors
            switch(direction) {
                case 'R': srcX = 8*16; break;   // (128, 48)
                case 'L': srcX = 9*16; break;   // (144, 48)
                case 'U': srcX = 10*16; break;  // (160, 48)
                case 'D': srcX = 11*16; break;  // (176, 48)
                default: srcX = 8*16; break;    // Default to right
            }
        }
        else if (stateInt == 2) { // FEARED
            // ALL ghosts use the same feared sprites at row 2 (Y=32)
            srcY = 2 * 16; // Always row 2 (32) for all ghost colors when feared

            if (isWhiteFeared) {
                srcX = (10 + currentFrame) * 16;  // (160, 32) or (176, 32)
            } else {
                srcX = (8 + currentFrame) * 16;   // (128, 32) or (144, 32)
            }
        }
        else if (stateInt == 0) { // WAITING
            // Static sprite, no animation - use frame 0, facing right
            srcX = 0;
            srcY = getGhostRow() * 16;
        }
        else { // CHASING (stateInt == 1)
            // Normal ghost with walking animation
            int baseColumn = 0;
            switch(direction) {
                case 'R': baseColumn = 0; break;  // Columns 0-1
                case 'L': baseColumn = 2; break;  // Columns 2-3
                case 'U': baseColumn = 4; break;  // Columns 4-5
                case 'D': baseColumn = 6; break;  // Columns 6-7
                default: baseColumn = 0; break;
            }

            srcX = (baseColumn + currentFrame) * 16;
            srcY = getGhostRow() * 16;
        }

        sprite.setTextureRect(sf::IntRect(srcX, srcY, 16, 16));
    }

    int GhostView::getGhostRow() const {
        // Map spriteId to actual row in YOUR spritesheet
        switch(spriteId) {
            case 0: return 2; // Red ghost - row 2
            case 1: return 3; // Pink ghost - row 3
            case 2: return 4; // Blue ghost - row 4
            case 3: return 5; // Orange ghost - row 5
            default: return 2; // Default to red
        }
    }

    void GhostView::checkFearedBlinking() {
        // Get entity using polymorphic interface - NO CASTING!
        auto entity = model.lock();
        if (!entity) return;

        int stateInt = entity->getStateInt();
        float stateTimer = entity->getStateTimer();

        // Only blink when feared (state 2) and timer is less than 2 seconds
        if (stateInt == 2 && stateTimer < 2.0f) {
            // Accumulate time using Stopwatch singleton
            float deltaTime = Logic::Stopwatch::getInstance().getDeltaTime();
            blinkAccumulator += deltaTime;

            // Blink between blue and white every 0.2 seconds
            if (blinkAccumulator >= 0.2f) {
                isWhiteFeared = !isWhiteFeared;
                blinkAccumulator -= 0.2f; // Keep remainder
            }
        } else {
            // Not blinking, use normal blue feared sprites
            isWhiteFeared = false;
            blinkAccumulator = 0.0f; // Reset accumulator
        }
    }

    void GhostView::draw() {
        // Get entity to check if despawned
        auto entity = model.lock();
        if (!entity) return;

        int stateInt = entity->getStateInt();

        // Debug output
        static int frameCount = 0;
        if (frameCount++ % 60 == 0) { // Print every 60 frames
            std::cout << "Ghost " << spriteId << " drawing - state: " << stateInt << std::endl;
        }

        // Don't draw if ghost is despawned (state 4)
        // 0=WAITING, 1=CHASING, 2=FEARED, 3=DEAD, 4=DESPAWNED
        if (stateInt == 4) {
            return; // Ghost is invisible
        }

        // Check if we should blink in feared state
        checkFearedBlinking();

        // Update animation before drawing
        updateAnimation();

        // Call the parent draw method to handle positioning and scaling
        EntityView::draw();
    }

    void GhostView::onNotify() {
        // When notified by the model, update animation and draw
        draw();
    }
}