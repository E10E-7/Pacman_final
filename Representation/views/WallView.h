//
// Created by etienne on 12/11/25.
//

#ifndef PACMAN_RETRY_WALLVIEW_H
#define PACMAN_RETRY_WALLVIEW_H
#include "EntityView.h"

namespace Render {
    class WallView final : public Render::EntityView {
    public:
        WallView(const std::shared_ptr<Logic::Entity>& entity, const std::shared_ptr<Camera>& cam, sf::RenderWindow& win) : EntityView(entity, cam, win) {};
        ~WallView() override = default;
        void draw() override;
    };
}


#endif //PACMAN_RETRY_WALLVIEW_H