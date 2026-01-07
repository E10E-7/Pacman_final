//
// Created by etienne on 12/11/25.
//

#ifndef PACMAN_RETRY_FRUITVIEW_H
#define PACMAN_RETRY_FRUITVIEW_H
#include "EntityView.h"

namespace Render {
    class FruitView final : public EntityView {
        public:
        FruitView(const std::shared_ptr<Logic::Entity>& entity, const std::shared_ptr<Camera>& cam, sf::RenderWindow& win);
        void setFrame(int frameX, int frameY, int frameWidth, int frameHeight);
    };
}


#endif //PACMAN_RETRY_FRUITVIEW_H