//
// Created by etienne on 12/7/25.
//

#ifndef PACMAN_RETRY_FRUIT_H
#define PACMAN_RETRY_FRUIT_H
#include "Entity.h"

namespace Logic {
    class Fruit final : public Entity {
    public:
        Fruit(const float x, const float y, const float width, const float height) : Entity(x, y, width, height) {}
        void update() const;
    };
}
#endif //PACMAN_RETRY_FRUIT_H