//
// Created by etienne on 12/8/25.
//

#ifndef PACMAN_RETRY_ENTITYVIEW_H
#define PACMAN_RETRY_ENTITYVIEW_H
#include <memory>
#include <SFML/Graphics.hpp>
#include "Logic/Observer.h"
#include "Representation/Camera.h"
#include "Logic/Entities/Entity.h"
#include <iostream>

namespace Logic {
    class Entity;
}

namespace Render {

    class EntityView : public Observer {
    protected:
        sf::Sprite sprite;
        sf::Texture texture;
        std::weak_ptr<Logic::Entity> model;
        std::shared_ptr<Camera> camera;
        sf::RenderWindow& window;

    public:
        EntityView(const std::shared_ptr<Logic::Entity>& entity, const std::shared_ptr<Camera>& cam, sf::RenderWindow& win)
            : model(entity), camera(cam), window(win) {}
        ~EntityView() override = default;

        virtual void draw();
        void onNotify() override;

    protected:
        bool loadTexture(const std::string& filename);
        void updateSpriteScale(float worldWidth, float worldHeight);
    };
}


#endif //PACMAN_RETRY_ENTITYVIEW_H