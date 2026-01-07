//
// Created by etienne on 12/7/25.
//

#include "Entity.h"

namespace Logic {

    Entity::Entity(const float x, const float y, const float width, const float height)
        : x(x), y(y), width(width), height(height) {}

    void Entity::setPosition(const float newX, const float newY) {
        x = newX;
        y = newY;
    }

    void Entity::setSize(const float w, const float h) {
        width = w;
        height = h;
    }

    bool Entity::intersects(const Entity& other) const {
        constexpr float epsilon = 0.008f; // Small tolerance

        const float left = x - width / 2.0f;
        const float right = x + width / 2.0f;
        const float top = y - height / 2.0f;
        const float bottom = y + height / 2.0f;

        const float otherLeft = other.x - other.width / 2.0f;
        const float otherRight = other.x + other.width / 2.0f;
        const float otherTop = other.y - other.height / 2.0f;
        const float otherBottom = other.y + other.height / 2.0f;

        return !(right <= otherLeft + epsilon || left >= otherRight - epsilon ||
                 bottom <= otherTop + epsilon || top >= otherBottom - epsilon);
    }

} // namespace Logic