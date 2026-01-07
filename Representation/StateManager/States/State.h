// Representation/StateManager/States/State.h
#ifndef PACMAN_RETRY_STATE_H
#define PACMAN_RETRY_STATE_H

#include <memory>
#include <SFML/Graphics.hpp>
#include "Logic/Stopwatch.h"

namespace Render {
    class ConcreteFactory;
    class Camera;
}

namespace Logic {
    class World;
    class ScoreManager;
}

class StateManager;

class State {
public:
    State() = default;
    virtual ~State() = default;

    virtual void processEvents(const sf::Event& event, sf::RenderWindow& window, StateManager& stateManager) = 0;
    virtual void update(sf::RenderWindow& window, StateManager& stateManager) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void handleResize(unsigned int width, unsigned int height) {}
};

// ============ MENU STATE ============
class MenuState final : public State {
private:
    sf::Font font;
    sf::Text titleText;
    sf::Text leaderboardTitleText;
    std::vector<sf::Text> scoreTexts;
    sf::RectangleShape playButton;
    sf::Text playButtonText;
    sf::Vector2u windowSize;

    void loadScores();
    void repositionUI();

public:
    explicit MenuState(sf::RenderWindow& window);

    void processEvents(const sf::Event& event, sf::RenderWindow& window, StateManager& stateManager) override;
    void update(sf::RenderWindow& window, StateManager& stateManager) override;
    void draw(sf::RenderWindow& window) override;
    void handleResize(unsigned int width, unsigned int height) override;
};

// ============ LEVEL STATE ============
class LevelState final : public State {
private:
    std::unique_ptr<Logic::World> world;
    std::shared_ptr<Render::ConcreteFactory> factory;
    std::shared_ptr<Render::Camera> camera;

    // UI elements
    sf::Font font;
    sf::Text scoreText;
    sf::Text livesText;
    sf::Text levelText;

public:
    explicit LevelState(sf::RenderWindow &window);

    void processEvents(const sf::Event& event, sf::RenderWindow& window, StateManager& stateManager) override;
    void update(sf::RenderWindow& window, StateManager& stateManager) override;
    void draw(sf::RenderWindow& window) override;
    void handleResize(unsigned int width, unsigned int height) override;
};

// ============ PAUSE STATE ============
class PauseState final : public State {
private:
    sf::Font font;
    sf::Text titleText;
    sf::RectangleShape continueButton;
    sf::Text continueButtonText;
    sf::RectangleShape menuButton;
    sf::Text menuButtonText;
    sf::RectangleShape overlay;

    void repositionUI(unsigned int width, unsigned int height);

public:
    explicit PauseState(sf::RenderWindow& window);

    void processEvents(const sf::Event& event, sf::RenderWindow& window, StateManager& stateManager) override;
    void update(sf::RenderWindow& window, StateManager& stateManager) override {}
    void draw(sf::RenderWindow& window) override;
    void handleResize(unsigned int width, unsigned int height) override;
};

// ============ GAME OVER STATE ============
class GameOverState final : public State {
private:
    sf::Font font;
    sf::Text titleText;
    sf::Text scoreText;
    sf::Text promptText;
    sf::Text nameInputText;
    sf::RectangleShape inputBox;
    sf::RectangleShape submitButton;
    sf::Text submitButtonText;

    std::string playerName;
    int finalScore;
    bool nameSubmitted = false;

    void repositionUI(unsigned int width, unsigned int height);

public:
    explicit GameOverState(sf::RenderWindow& window, int score);

    void processEvents(const sf::Event& event, sf::RenderWindow& window, StateManager& stateManager) override;
    void update(sf::RenderWindow& window, StateManager& stateManager) override {}
    void draw(sf::RenderWindow& window) override;
    void handleResize(unsigned int width, unsigned int height) override;
};

#endif //PACMAN_RETRY_STATE_H