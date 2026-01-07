// Game.cpp
#include "Game.h"
#include "Representation/StateManager/StateManager.h"
#include "Representation/StateManager/States/State.h"

void Game::run() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Pacman");
    window.setFramerateLimit(60);

    StateManager stateManager;
    stateManager.push(std::make_unique<MenuState>(window));

    while (window.isOpen()) {
        sf::Event event{};

        // 1. INPUT
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle window resize
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));

                // Notify the state manager about the resize
                stateManager.handleResize(event.size.width, event.size.height);
            }

            stateManager.processEvents(window, event);
        }

        // 2. CLEAR (Must happen BEFORE update, because update triggers drawing)
        window.clear();

        // 3. UPDATE (Logic runs -> Notifies Entities -> Views Draw themselves)
        stateManager.update(window);

        // 4. DRAW (Draw UI, Menus, or things not covered by Observers)
        stateManager.draw(window);

        // 5. DISPLAY
        window.display();
    }
}