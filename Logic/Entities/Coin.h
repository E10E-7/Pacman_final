//
// Created by etienne on 12/7/25.
//

#ifndef PACMAN_RETRY_COIN_H
#define PACMAN_RETRY_COIN_H
#include "Entity.h"

namespace Logic {
    class Coin final : public Entity {
    private:
        bool collected = false;

    public:
        Coin(const float x, const float y, const float width, const float height) : Entity(x, y, width, height) {}
        ~Coin() override = default;

        [[nodiscard]] bool isCollected() const { return collected; }
        void collect() { collected = true; }
        void reset() { collected = false; }
        void update() const;
    };
}


#endif //PACMAN_RETRY_COIN_H