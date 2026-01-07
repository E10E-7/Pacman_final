//
// Entity.h - Updated with death state methods
//

#ifndef PACMAN_RETRY_ENTITY_H
#define PACMAN_RETRY_ENTITY_H
#include "Logic/Observer.h"

namespace Logic {
    class Entity : public Subject {
    private:
        float x, y;
        float width, height;
    public:
        Entity(float x, float y, float width, float height);

        // Getters
        [[nodiscard]] float getX() const { return x; }
        [[nodiscard]] float getY() const { return y; }
        [[nodiscard]] float getWidth() const { return width; }
        [[nodiscard]] float getHeight() const { return height; }

        // Setters
        void setPosition(float newX, float newY);
        virtual void setSize(float w, float h);

        // Virtual methods for animation data - can be overridden by derived classes
        [[nodiscard]] virtual char getDirection() const { return ' '; }
        [[nodiscard]] virtual bool isMoving() const { return false; }

        // For ghost-specific state (returns -1 for non-ghosts)
        // 0=WAITING, 1=CHASING, 2=FEARED, 3=DEAD
        [[nodiscard]] virtual int getStateInt() const { return -1; }
        [[nodiscard]] virtual float getStateTimer() const { return 0.0f; }

        // For sprite identification (returns -1 for non-ghosts)
        [[nodiscard]] virtual int getSpriteId() const { return -1; }

        // For death animation (Pacman-specific)
        [[nodiscard]] virtual bool isDying() const { return false; }
        [[nodiscard]] virtual int getLives() const { return 0; }

        // Collision detection using basic rectangle intersection
        [[nodiscard]] bool intersects(const Entity& other) const;
    };
}

#endif //PACMAN_RETRY_ENTITY_H