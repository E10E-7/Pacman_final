//
// PacmanView.h - With death animation
//

#ifndef PACMAN_RETRY_PACMANVIEW_H
#define PACMAN_RETRY_PACMANVIEW_H

#include "EntityView.h"

namespace Render {
    class PacmanView : public EntityView {
    private:
        int currentFrame;
        float animationSpeed; // Time between frames in seconds
        float animationAccumulator; // Accumulated time since last frame change

        void updateAnimation();
        void updateSpriteForDirection(char direction);
        void updateDeathSprite(); // Death animation

    public:
        PacmanView(const std::shared_ptr<Logic::Entity>& entity,
                   const std::shared_ptr<Camera>& cam,
                   sf::RenderWindow& win);

        void setFrame(int frameX, int frameY, int frameWidth, int frameHeight);
        void draw() override;
        void onNotify() override;
    };
}

#endif // PACMAN_RETRY_PACMANVIEW_H