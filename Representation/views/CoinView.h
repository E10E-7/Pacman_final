//
// Created by etienne on 12/11/25.
//

#ifndef PACMAN_RETRY_COINVIEW_H
#define PACMAN_RETRY_COINVIEW_H
#include "EntityView.h"

namespace Render {
    class CoinView final : public EntityView {
    public:
        CoinView(const std::shared_ptr<Logic::Entity>& entity, const std::shared_ptr<Camera>& cam, sf::RenderWindow& win);
        void setFrame(int frameX, int frameY, int frameWidth, int frameHeight);

    };
}

#endif //PACMAN_RETRY_COINVIEW_H