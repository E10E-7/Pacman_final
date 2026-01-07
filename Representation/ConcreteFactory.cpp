//
// Created by etienne on 12/4/25.
//

#include "ConcreteFactory.h"

#include <utility>
#include "Camera.h"
#include "Logic/Entities/Coin.h"
#include "Logic/Entities/Fruit.h"
#include "Logic/Entities/Pacman.h"
#include "Logic/Entities/Wall.h"
#include "Logic/Entities/GhostsTypes.h"
#include "views/CoinView.h"
#include "views/WallView.h"
#include "views/PacmanView.h"
#include "views/FruitView.h"
#include "views/GhostView.h"

namespace Render {
    void ConcreteFactory::setCamera(std::shared_ptr<Camera> cam) {
        this->camera = std::move(cam);
    }

    ConcreteFactory::ConcreteFactory(sf::RenderWindow& window, const std::shared_ptr<Camera>& camera) : window(window) {
        setCamera(camera);
    }


    std::shared_ptr<Logic::Pacman> ConcreteFactory::createPacMan(float x, float y, float w, float h) {
        auto pacman = std::make_shared<Logic::Pacman>(x, y, w, h);
        const auto pacmanView = std::make_shared<PacmanView>(pacman, camera, window);
        pacman->attach(pacmanView);
        return pacman;
    }

    std::shared_ptr<Logic::Coin> ConcreteFactory::createCoin(float x, float y, float w, float h) {
        auto coin = std::make_shared<Logic::Coin>(x, y, w, h);
        const auto coinView = std::make_shared<CoinView>(coin, camera, window);
        coin->attach(coinView);
        return coin;
    }

    std::shared_ptr<Logic::Ghost> ConcreteFactory::createGhost(float x, float y, float w, float h, char type) {
        std::shared_ptr<Logic::Ghost> ghost;
        int spriteId = 0; // 0=Red, 1=Pink, 2=Blue, 3=Orange

        switch (type) {
            case 'R': // Red
                ghost = std::make_shared<Logic::RedGhost>(x, y, w, h);
                spriteId = 0;
                break;
            case 'I': // Pink (Inky/Pinky mapping per your request)
                 ghost = std::make_shared<Logic::PinkGhost>(x, y, w, h);
                 spriteId = 1;
                 break;
            case 'B': // Blue
                 ghost = std::make_shared<Logic::BlueGhost>(x, y, w, h);
                 spriteId = 2;
                 break;
            case 'O': // Orange
                 ghost = std::make_shared<Logic::OrangeGhost>(x, y, w, h);
                 spriteId = 3;
                 break;
            default: // Fallback
                ghost = std::make_shared<Logic::RedGhost>(x, y, w, h);
                spriteId = 0;
                break;
        }

        // Create the view, passing the spriteId so it knows which color to draw
        auto ghostView = std::make_shared<GhostView>(ghost, camera, window, spriteId);
        ghost->attach(ghostView);

        return ghost;
    }

    std::shared_ptr<Logic::Wall> ConcreteFactory::createWall(float x, float y, float w, float h) {
        auto wall = std::make_shared<Logic::Wall>(x, y, w, h);
        const auto wallView = std::make_shared<WallView>(wall, camera, window);
        wall->attach(wallView);
        return wall;
    }

    std::shared_ptr<Logic::Fruit> ConcreteFactory::createFruit(float x, float y, float w, float h) {
        auto fruit = std::make_shared<Logic::Fruit>(x, y, w, h);
        const auto fruitView = std::make_shared<FruitView>(fruit, camera, window);
        fruit->attach(fruitView);
        return fruit;
    }
}
