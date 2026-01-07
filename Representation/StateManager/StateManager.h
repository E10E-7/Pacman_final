// Representation/StateManager/StateManager.h
#ifndef PACMAN_RETRY_STATEMANAGER_H
#define PACMAN_RETRY_STATEMANAGER_H
#include <memory>
#include <stack>
#include <SFML/Graphics.hpp>

class State;

class StateManager {
    std::stack<std::unique_ptr<State>> stateStack;
public:
    void push(std::unique_ptr<State> state);
    void pop();

    // Split the 'run' loop
    void processEvents(sf::RenderWindow& window, const sf::Event &event);
    void update(sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);

    // Handle window resize
    void handleResize(unsigned int width, unsigned int height);
};
#endif