//
// Created by etienne on 12/8/25.
//

#include "EntityView.h"
#include "Logic/Entities/Entity.h"
#include <stdexcept>

namespace Render {
    void EntityView::draw() {
        if (const auto entity = model.lock()) {
            const sf::Vector2f screenPos = camera->worldToScreen(entity->getX(), entity->getY());

            // Scale the sprite based on entity's world size
            const float screenWidth = camera->worldToScreenSize(entity->getWidth());
            const float screenHeight = camera->worldToScreenSize(entity->getHeight());

            const sf::IntRect textureRect = sprite.getTextureRect();
            const float scaleX = screenWidth / textureRect.width;
            const float scaleY = screenHeight / textureRect.height;

            sprite.setScale(scaleX, scaleY);

            // Set origin to center of the texture rect
            sprite.setOrigin(textureRect.width / 2.0f, textureRect.height / 2.0f);

            sprite.setPosition(screenPos);
            window.draw(sprite);
        }
    }

    void EntityView::updateSpriteScale(const float worldWidth, const float worldHeight) {
        const float screenWidth = camera->worldToScreenSize(worldWidth);
        const float screenHeight = camera->worldToScreenSize(worldHeight);

        const sf::IntRect textureRect = sprite.getTextureRect();
        const float scaleX = screenWidth / static_cast<float>(textureRect.width);
        const float scaleY = screenHeight / static_cast<float>(textureRect.height);

        sprite.setScale(scaleX, scaleY);
    }

    void EntityView::onNotify() {
        // Handle notifications from the model if needed
        draw();
    }

    bool EntityView::loadTexture(const std::string &filename) {
        try {
            if (!texture.loadFromFile(filename)) {
                throw std::runtime_error("Failed to load texture file: " + filename);
            }
            sprite.setTexture(texture);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Exception in loadTexture: " << e.what() << std::endl;
            throw; // Re-throw to let caller handle it
        }
    }
} // namespace Render