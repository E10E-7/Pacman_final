// Representation/StateManager/States/State.cpp
#include "State.h"
#include <memory>
#include <iostream>

#include "Logic/World.h"
#include "Logic/Score.h"
#include "Representation/Camera.h"
#include "Representation/ConcreteFactory.h"
#include "Representation/StateManager/StateManager.h"

// ============================================
// MENU STATE
// ============================================

MenuState::MenuState(sf::RenderWindow& window) {
    windowSize = window.getSize();

    if (!font.loadFromFile("../assets/DejaVuSans.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }

    // Setup Title
    titleText.setFont(font);
    titleText.setString("PAC-MAN");
    titleText.setCharacterSize(60);
    titleText.setFillColor(sf::Color::Yellow);

    // Setup Leaderboard Title
    leaderboardTitleText.setFont(font);
    leaderboardTitleText.setString("TOP 5 SCORES");
    leaderboardTitleText.setCharacterSize(30);
    leaderboardTitleText.setFillColor(sf::Color::White);

    // Setup Play Button
    playButton.setSize(sf::Vector2f(200, 60));
    playButton.setFillColor(sf::Color(0, 150, 0));
    playButton.setOutlineThickness(3);
    playButton.setOutlineColor(sf::Color::White);

    playButtonText.setFont(font);
    playButtonText.setString("PLAY");
    playButtonText.setCharacterSize(30);
    playButtonText.setFillColor(sf::Color::White);

    // Load scores and position UI
    loadScores();
    repositionUI();
}

void MenuState::repositionUI() {
    float centerX = windowSize.x / 2.0f;

    // Position Title
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.width / 2.0f, 0);
    titleText.setPosition(centerX, 50);

    // Position Leaderboard Title
    sf::FloatRect lbBounds = leaderboardTitleText.getLocalBounds();
    leaderboardTitleText.setOrigin(lbBounds.width / 2.0f, 0);
    leaderboardTitleText.setPosition(centerX, 120);

    // Position score texts
    for (size_t i = 0; i < scoreTexts.size(); ++i) {
        sf::FloatRect bounds = scoreTexts[i].getLocalBounds();
        scoreTexts[i].setOrigin(bounds.width / 2.0f, 0);
        scoreTexts[i].setPosition(centerX, 170 + i * 30);
    }

    // Position Play Button
    playButton.setOrigin(100, 30);
    playButton.setPosition(centerX, windowSize.y - 100);

    sf::FloatRect btnTextBounds = playButtonText.getLocalBounds();
    playButtonText.setOrigin(btnTextBounds.width / 2.0f, btnTextBounds.height / 2.0f + 5);
    playButtonText.setPosition(centerX, windowSize.y - 100);
}

void MenuState::loadScores() {
    scoreTexts.clear();

    Logic::ScoreManager scoreManager;
    auto topScores = scoreManager.getTopScores(5);

    for (size_t i = 0; i < 5; ++i) {
        sf::Text scoreText;
        scoreText.setFont(font);

        if (i < topScores.size()) {
            scoreText.setString(std::to_string(i + 1) + ". " +
                              topScores[i].name + " - " +
                              std::to_string(topScores[i].score));
        } else {
            scoreText.setString(std::to_string(i + 1) + ". ---");
        }

        scoreText.setCharacterSize(20);
        scoreText.setFillColor(sf::Color::White);
        scoreTexts.push_back(scoreText);
    }
}

void MenuState::handleResize(unsigned int width, unsigned int height) {
    windowSize.x = width;
    windowSize.y = height;
    repositionUI();
}

void MenuState::update(sf::RenderWindow& window, StateManager& stateManager) {
    loadScores();
    repositionUI();
}

void MenuState::draw(sf::RenderWindow& window) {
    window.draw(titleText);
    window.draw(leaderboardTitleText);

    for (const auto& text : scoreTexts) {
        window.draw(text);
    }

    window.draw(playButton);
    window.draw(playButtonText);
}

void MenuState::processEvents(const sf::Event& event, sf::RenderWindow& window, StateManager& stateManager) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left &&
        playButton.getGlobalBounds().contains(worldPos)) {
        stateManager.push(std::make_unique<LevelState>(window));
        return;
    }

    if (playButton.getGlobalBounds().contains(worldPos)) {
        playButton.setFillColor(sf::Color(0, 200, 0));
    } else {
        playButton.setFillColor(sf::Color(0, 150, 0));
    }
}

// ============================================
// LEVEL STATE
// ============================================

LevelState::LevelState(sf::RenderWindow &window) {
    camera = std::make_shared<Render::Camera>(window.getSize().x, window.getSize().y);
    factory = std::make_shared<Render::ConcreteFactory>(window, camera);
    world = std::make_unique<Logic::World>(factory);

    if (!font.loadFromFile("../assets/DejaVuSans.ttf")) {
        std::cerr << "Failed to load font for UI!" << std::endl;
    }

    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    livesText.setFont(font);
    livesText.setCharacterSize(24);
    livesText.setFillColor(sf::Color::Yellow);
    livesText.setPosition(10, 40);

    levelText.setFont(font);
    levelText.setCharacterSize(24);
    levelText.setFillColor(sf::Color::Cyan);
    levelText.setPosition(10, 70);
}

void LevelState::handleResize(unsigned int width, unsigned int height) {
    if (camera) {
        camera->setWindowSize(width, height);
    }
}

void LevelState::processEvents(const sf::Event& event, sf::RenderWindow& window, StateManager& stateManager) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        stateManager.push(std::make_unique<PauseState>(window));
        return;
    }
}

void LevelState::update(sf::RenderWindow& window, StateManager& stateManager) {
    if (world->isGameOver()) {
        int finalScore = world->getScore();
        std::cout << "Game Over! Final Score: " << finalScore << std::endl;
        stateManager.pop();
        stateManager.push(std::make_unique<GameOverState>(window, finalScore));
        return;
    }

    if (world->isLevelComplete()) {
        world->nextLevel();
    }

    char input = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    input = 'U';
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  input = 'D';
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  input = 'L';
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) input = 'R';

    world->update(input);

    scoreText.setString("Score: " + std::to_string(world->getScore()));
    livesText.setString("Lives: " + std::to_string(world->getLives()));
    levelText.setString("Level: " + std::to_string(world->getCurrentLevel()));
}

void LevelState::draw(sf::RenderWindow& window) {
    window.draw(scoreText);
    window.draw(livesText);
    window.draw(levelText);
}

// ============================================
// PAUSE STATE
// ============================================

PauseState::PauseState(sf::RenderWindow& window) {
    Logic::Stopwatch::getInstance().pause();

    sf::Vector2u windowSize = window.getSize();

    if (!font.loadFromFile("../assets/DejaVuSans.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }

    overlay.setSize(sf::Vector2f(windowSize.x, windowSize.y));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));

    titleText.setFont(font);
    titleText.setString("PAUSED");
    titleText.setCharacterSize(60);
    titleText.setFillColor(sf::Color::Yellow);

    continueButton.setSize(sf::Vector2f(250, 60));
    continueButton.setFillColor(sf::Color(0, 150, 0));
    continueButton.setOutlineThickness(3);
    continueButton.setOutlineColor(sf::Color::White);

    continueButtonText.setFont(font);
    continueButtonText.setString("CONTINUE");
    continueButtonText.setCharacterSize(30);
    continueButtonText.setFillColor(sf::Color::White);

    menuButton.setSize(sf::Vector2f(250, 60));
    menuButton.setFillColor(sf::Color(150, 0, 0));
    menuButton.setOutlineThickness(3);
    menuButton.setOutlineColor(sf::Color::White);

    menuButtonText.setFont(font);
    menuButtonText.setString("MAIN MENU");
    menuButtonText.setCharacterSize(30);
    menuButtonText.setFillColor(sf::Color::White);

    repositionUI(windowSize.x, windowSize.y);
}

void PauseState::repositionUI(unsigned int width, unsigned int height) {
    float centerX = width / 2.0f;
    float centerY = height / 2.0f;

    overlay.setSize(sf::Vector2f(width, height));

    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
    titleText.setPosition(centerX, centerY - 100);

    continueButton.setOrigin(125, 30);
    continueButton.setPosition(centerX, centerY);

    sf::FloatRect contBounds = continueButtonText.getLocalBounds();
    continueButtonText.setOrigin(contBounds.width / 2.0f, contBounds.height / 2.0f + 5);
    continueButtonText.setPosition(centerX, centerY);

    menuButton.setOrigin(125, 30);
    menuButton.setPosition(centerX, centerY + 80);

    sf::FloatRect menuBounds = menuButtonText.getLocalBounds();
    menuButtonText.setOrigin(menuBounds.width / 2.0f, menuBounds.height / 2.0f + 5);
    menuButtonText.setPosition(centerX, centerY + 80);
}

void PauseState::handleResize(unsigned int width, unsigned int height) {
    repositionUI(width, height);
}

void PauseState::draw(sf::RenderWindow& window) {
    window.draw(overlay);
    window.draw(titleText);
    window.draw(continueButton);
    window.draw(continueButtonText);
    window.draw(menuButton);
    window.draw(menuButtonText);
}

void PauseState::processEvents(const sf::Event& event, sf::RenderWindow& window, StateManager& stateManager) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        Logic::Stopwatch::getInstance().resume();
        stateManager.pop();
        return;
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (continueButton.getGlobalBounds().contains(worldPos)) {
            Logic::Stopwatch::getInstance().resume();
            stateManager.pop();
            return;
        }

        if (menuButton.getGlobalBounds().contains(worldPos)) {
            Logic::Stopwatch::getInstance().resume();
            stateManager.pop();
            stateManager.pop();
            return;
        }
    }

    if (continueButton.getGlobalBounds().contains(worldPos)) {
        continueButton.setFillColor(sf::Color(0, 200, 0));
    } else {
        continueButton.setFillColor(sf::Color(0, 150, 0));
    }

    if (menuButton.getGlobalBounds().contains(worldPos)) {
        menuButton.setFillColor(sf::Color(200, 0, 0));
    } else {
        menuButton.setFillColor(sf::Color(150, 0, 0));
    }
}

// ============================================
// GAME OVER STATE
// ============================================

GameOverState::GameOverState(sf::RenderWindow& window, int score) : finalScore(score) {
    Logic::Stopwatch::getInstance().pause();

    sf::Vector2u windowSize = window.getSize();

    if (!font.loadFromFile("../assets/DejaVuSans.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }

    titleText.setFont(font);
    titleText.setString("GAME OVER");
    titleText.setCharacterSize(60);
    titleText.setFillColor(sf::Color::Red);

    scoreText.setFont(font);
    scoreText.setString("Final Score: " + std::to_string(finalScore));
    scoreText.setCharacterSize(40);
    scoreText.setFillColor(sf::Color::White);

    promptText.setFont(font);
    promptText.setString("Enter your name:");
    promptText.setCharacterSize(30);
    promptText.setFillColor(sf::Color::White);

    inputBox.setSize(sf::Vector2f(300, 50));
    inputBox.setFillColor(sf::Color(50, 50, 50));
    inputBox.setOutlineThickness(3);
    inputBox.setOutlineColor(sf::Color::White);

    nameInputText.setFont(font);
    nameInputText.setCharacterSize(30);
    nameInputText.setFillColor(sf::Color::Yellow);
    nameInputText.setString("_");

    submitButton.setSize(sf::Vector2f(200, 60));
    submitButton.setFillColor(sf::Color(0, 150, 0));
    submitButton.setOutlineThickness(3);
    submitButton.setOutlineColor(sf::Color::White);

    submitButtonText.setFont(font);
    submitButtonText.setString("SUBMIT");
    submitButtonText.setCharacterSize(30);
    submitButtonText.setFillColor(sf::Color::White);

    repositionUI(windowSize.x, windowSize.y);
}

void GameOverState::repositionUI(unsigned int width, unsigned int height) {
    float centerX = width / 2.0f;

    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
    titleText.setPosition(centerX, 100);

    sf::FloatRect scoreBounds = scoreText.getLocalBounds();
    scoreText.setOrigin(scoreBounds.width / 2.0f, scoreBounds.height / 2.0f);
    scoreText.setPosition(centerX, 200);

    sf::FloatRect promptBounds = promptText.getLocalBounds();
    promptText.setOrigin(promptBounds.width / 2.0f, promptBounds.height / 2.0f);
    promptText.setPosition(centerX, 280);

    inputBox.setOrigin(150, 25);
    inputBox.setPosition(centerX, 350);

    sf::FloatRect nameTextBounds = nameInputText.getLocalBounds();
    nameInputText.setOrigin(nameTextBounds.width / 2.0f, nameTextBounds.height / 2.0f + 5);
    nameInputText.setPosition(centerX, 350);

    submitButton.setOrigin(100, 30);
    submitButton.setPosition(centerX, 450);

    sf::FloatRect btnBounds = submitButtonText.getLocalBounds();
    submitButtonText.setOrigin(btnBounds.width / 2.0f, btnBounds.height / 2.0f + 5);
    submitButtonText.setPosition(centerX, 450);
}

void GameOverState::handleResize(unsigned int width, unsigned int height) {
    repositionUI(width, height);
}

void GameOverState::draw(sf::RenderWindow& window) {
    window.draw(titleText);
    window.draw(scoreText);
    window.draw(promptText);
    window.draw(inputBox);
    window.draw(nameInputText);
    window.draw(submitButton);
    window.draw(submitButtonText);
}

void GameOverState::processEvents(const sf::Event& event, sf::RenderWindow& window, StateManager& stateManager) {
    if (nameSubmitted) return;

    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode == 8) {
            if (!playerName.empty()) {
                playerName.pop_back();
            }
        } else if (event.text.unicode == 13) {
            if (!playerName.empty()) {
                Logic::ScoreManager scoreManager;
                scoreManager.addScore(playerName, finalScore);
                nameSubmitted = true;

                Logic::Stopwatch::getInstance().resume();
                stateManager.pop();
                return;
            }
        } else if (event.text.unicode < 128 && event.text.unicode >= 32 && playerName.length() < 10) {
            playerName += static_cast<char>(event.text.unicode);
        }

        nameInputText.setString(playerName + "_");
        sf::FloatRect nameTextBounds = nameInputText.getLocalBounds();
        nameInputText.setOrigin(nameTextBounds.width / 2.0f, nameTextBounds.height / 2.0f + 5);
    }

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

    if (submitButton.getGlobalBounds().contains(worldPos)) {
        submitButton.setFillColor(sf::Color(0, 200, 0));
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            if (!playerName.empty()) {
                Logic::ScoreManager scoreManager;
                scoreManager.addScore(playerName, finalScore);
                nameSubmitted = true;

                Logic::Stopwatch::getInstance().resume();
                stateManager.pop();
                return;
            }
        }
    } else {
        submitButton.setFillColor(sf::Color(0, 150, 0));
    }
}