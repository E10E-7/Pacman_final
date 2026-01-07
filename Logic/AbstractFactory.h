// Logic/AbstractFactory.h
#ifndef PACMAN_RETRY_ABSTRACTFACTORY_H
#define PACMAN_RETRY_ABSTRACTFACTORY_H
#include <memory>

namespace Logic {
    class Pacman;
    class Fruit;
    class Coin;
    class Wall;
    class Ghost;

    class AbstractFactory {
    public:
        virtual ~AbstractFactory() = default;

        virtual std::shared_ptr<Pacman> createPacMan(float x, float y, float w, float h) = 0;
        virtual std::shared_ptr<Ghost> createGhost(float x, float y, float w, float h, char type) = 0;
        virtual std::shared_ptr<Coin> createCoin(float x, float y, float w, float h) = 0;
        virtual std::shared_ptr<Fruit> createFruit(float x, float y, float w, float h) = 0;
        virtual std::shared_ptr<Wall> createWall(float x, float y, float w, float h) = 0;
    };
}
#endif //PACMAN_RETRY_ABSTRACTFACTORY_H