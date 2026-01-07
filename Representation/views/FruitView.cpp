//
// Created by etienne on 12/11/25.
//

#include "FruitView.h"
#include <stdexcept>

namespace Render {
    FruitView::FruitView(const std::shared_ptr<Logic::Entity>& entity, const std::shared_ptr<Camera>& cam, sf::RenderWindow& win) : EntityView(entity, cam, win)
    {
        try {
            if (!texture.loadFromFile("../assets/spritesheet.png")) {
                throw std::runtime_error("Failed to load spritesheet.png for Fruit");
            }

            sprite.setTexture(texture);

            // Set initial frame
            setFrame(6 * 16, 1 * 16, 16, 16); // Adjust based on your spritesheet
        } catch (const std::exception& e) {
            std::cerr << "Exception in FruitView constructor: " << e.what() << std::endl;
            throw;
        }
    }

    void FruitView::setFrame(const int frameX, const int frameY, const int frameWidth, const int frameHeight) {
        sprite.setTextureRect(sf::IntRect(frameX, frameY, frameWidth, frameHeight));
    }
}