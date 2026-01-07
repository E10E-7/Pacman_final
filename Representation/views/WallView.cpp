//
// Created by etienne on 12/11/25.
//

#include "WallView.h"

namespace Render {
    void WallView::draw() {
        if (const auto entity = model.lock()) {
            const sf::Vector2f screenPos = camera->worldToScreen(entity->getX(), entity->getY());

            const float screenWidth = camera->worldToScreenSize(entity->getWidth());
            const float screenHeight = camera->worldToScreenSize(entity->getHeight());

            // Create a blue rectangle
            sf::RectangleShape rectangle(sf::Vector2f(screenWidth, screenHeight));
            rectangle.setFillColor(sf::Color::Blue);

            // Set origin to center so it aligns with entity position
            rectangle.setOrigin(screenWidth / 2.0f, screenHeight / 2.0f);
            rectangle.setPosition(screenPos);

            window.draw(rectangle);
        }
    }
}
