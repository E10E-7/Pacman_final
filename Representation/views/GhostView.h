//
// GhostView.h - Using Stopwatch singleton
//

#ifndef PACMAN_RETRY_GHOSTVIEW_H
#define PACMAN_RETRY_GHOSTVIEW_H

#include "EntityView.h"

namespace Render {
    class GhostView : public EntityView {
    private:
        int spriteId; // 0=Red, 1=Pink, 2=Blue, 3=Orange
        int currentFrame;
        float animationSpeed; // Time between frames in seconds
        float animationAccumulator; // Accumulated time since last frame change
        float blinkAccumulator; // Accumulated time for blinking
        bool isWhiteFeared; // For blinking between blue and white when feared timer is low

        void updateAnimation();
        void updateSpriteForState();
        void checkFearedBlinking();
        int getGhostRow() const; // Helper to map spriteId to row in spritesheet

    public:
        GhostView(const std::shared_ptr<Logic::Entity>& entity,
                  const std::shared_ptr<Camera>& cam,
                  sf::RenderWindow& win,
                  int id);

        void draw() override;
        void onNotify() override;
    };
}

#endif // PACMAN_RETRY_GHOSTVIEW_H