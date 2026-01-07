// Representation/StateManager/StateManager.cpp
#include "StateManager.h"
#include "States/State.h"

void StateManager::push(std::unique_ptr<State> state) {
    stateStack.push(std::move(state));
}

void StateManager::pop() {
    if (!stateStack.empty()) stateStack.pop();
}

void StateManager::processEvents(sf::RenderWindow& window, const sf::Event &event) {
    if (!stateStack.empty()) stateStack.top()->processEvents(event, window, *this);
}

void StateManager::update(sf::RenderWindow& window) {
    if (!stateStack.empty()) stateStack.top()->update(window, *this);
}

void StateManager::draw(sf::RenderWindow& window) {
    if (!stateStack.empty()) stateStack.top()->draw(window);
}

void StateManager::handleResize(unsigned int width, unsigned int height) {
    if (!stateStack.empty()) stateStack.top()->handleResize(width, height);
}