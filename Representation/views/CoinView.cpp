//
// Created by etienne on 12/11/25.
//

#include "CoinView.h"

namespace Render {
    CoinView::CoinView(const std::shared_ptr<Logic::Entity> &entity, const std::shared_ptr<Camera> &cam, sf::RenderWindow &win) : EntityView(entity, cam, win)
    {
        if (!texture.loadFromFile("../assets/spritesheet.png")) {
            // Handle error
            std::cerr << "Failed to load texture" << std::endl;
        }

        sprite.setTexture(texture);

        // Set initial frame
        setFrame(14 * 16, 1 * 16, 16, 16); // Adjust based on your spritesheet
    }

    void CoinView::setFrame(const int frameX, const int frameY, const int frameWidth, const int frameHeight) {
        sprite.setTextureRect(sf::IntRect(frameX, frameY, frameWidth, frameHeight));
    }
}
