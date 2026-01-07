//
// Created by etienne on 12/4/25.
//

#ifndef PACMAN_RETRY_CONCRETEFACTORY_H
#define PACMAN_RETRY_CONCRETEFACTORY_H

#include <SFML/Graphics/RenderWindow.hpp>
#include "Logic/AbstractFactory.h"

namespace Render {
    class Camera;

    class ConcreteFactory : public Logic::AbstractFactory {
    private:
        std::shared_ptr<Camera> camera;
        sf::RenderWindow& window;

    public:
        ConcreteFactory(sf::RenderWindow& window, const std::shared_ptr<Camera>& camera);
        ~ConcreteFactory() override = default;
        void setCamera(std::shared_ptr<Camera> cam);

        std::shared_ptr<Logic::Pacman> createPacMan(float x, float y, float w, float h) override;
        std::shared_ptr<Logic::Ghost> createGhost(float x, float y, float w, float h, char type) override;
        std::shared_ptr<Logic::Coin> createCoin(float x, float y, float w, float h) override;
        std::shared_ptr<Logic::Fruit> createFruit(float x, float y, float w, float h) override;
        std::shared_ptr<Logic::Wall> createWall(float x, float y, float w, float h) override;
    };
}
#endif //PACMAN_RETRY_CONCRETEFACTORY_H