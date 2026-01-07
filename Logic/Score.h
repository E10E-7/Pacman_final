//
// Score.h - Score calculation with time-based multipliers and high score management
//

#ifndef SCORE_H
#define SCORE_H

#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>
#include "Observer.h"
#include "Stopwatch.h"

/**
 * @file Score.h
 * @brief Score calculation system with time-based multipliers and persistent high score storage
 *
 * This file contains two main components:
 * 1. ScoreManager - Handles persistent storage of top 5 high scores to file
 * 2. Score - Calculates current game score with dynamic, skill-based modifiers
 *
 * The Score class implements time-based coin collection multipliers to reward
 * fast-paced gameplay, creating a risk/reward dynamic that encourages aggressive play.
 */

namespace Logic {
    // ============================================
    // SCORE ENTRY (for high scores storage)
    // ============================================

    /**
     * @struct ScoreEntry
     * @brief Represents a single high score entry with player name and score
     *
     * Used by ScoreManager to store and retrieve high score data from file.
     * The file format is plain text: "name score" pairs, one per line.
     */
    struct ScoreEntry {
        std::string name;  ///< Player name (max 10 characters in UI)
        int score;         ///< Final score achieved

        /**
         * @brief Default constructor initializing empty entry
         */
        ScoreEntry() : name(""), score(0) {}

        /**
         * @brief Construct entry with name and score
         * @param n Player name
         * @param s Score value
         */
        ScoreEntry(std::string n, int s) : name(std::move(n)), score(s) {}
    };

    // ============================================
    // SCORE MANAGER (high scores file management)
    // ============================================

    /**
     * @class ScoreManager
     * @brief Manages persistent storage of top 5 high scores
     *
     * ScoreManager handles reading and writing high scores to a plain text file.
     * It maintains a sorted list of the top 5 scores, automatically trimming
     * lower scores when new ones are added.
     *
     * File Format:
     * - Location: ../scores.txt
     * - Format: "name score" pairs, one per line
     * - Example: "Alice 5000\nBob 4500\n"
     *
     * Thread Safety:
     * - NOT thread-safe - should only be accessed from main game thread
     *
     * Usage Pattern:
     * @code
     * ScoreManager manager;
     * if (manager.isHighScore(playerScore)) {
     *     manager.addScore(playerName, playerScore);
     * }
     * auto topScores = manager.getTopScores(5);
     * @endcode
     */
    class ScoreManager {
    private:
        static constexpr const char* SCORE_FILE = "../scores.txt";  ///< Path to high scores file
        static constexpr int MAX_SCORES = 5;                         ///< Maximum number of scores to store
        std::vector<ScoreEntry> scores;                              ///< In-memory cache of high scores

        /**
         * @brief Loads scores from file into memory
         *
         * Reads the scores.txt file and populates the scores vector.
         * If the file doesn't exist, the vector remains empty (no scores yet).
         * Silent failure if file cannot be opened (new installation scenario).
         */
        void loadScores() {
            scores.clear();
            std::ifstream file(SCORE_FILE);
            if (file.is_open()) {
                std::string name;
                int score;
                while (file >> name >> score) {
                    scores.emplace_back(name, score);
                }
                file.close();
            }
        }

        /**
         * @brief Saves current scores from memory to file
         *
         * Writes the scores vector to scores.txt, overwriting previous content.
         * Each entry is written as "name score\n".
         * Silent failure if file cannot be opened (permission issue).
         */
        void saveScores() {
            std::ofstream file(SCORE_FILE);
            if (file.is_open()) {
                for (const auto& entry : scores) {
                    file << entry.name << " " << entry.score << "\n";
                }
                file.close();
            }
        }

    public:
        /**
         * @brief Constructs ScoreManager and loads existing scores
         *
         * Automatically calls loadScores() to populate the in-memory cache.
         */
        ScoreManager() {
            loadScores();
        }

        /**
         * @brief Adds a new score and saves to file
         * @param name Player name
         * @param score Score value
         *
         * This method:
         * 1. Adds the new score to the in-memory vector
         * 2. Sorts all scores in descending order (highest first)
         * 3. Trims to top 5 if more than 5 entries exist
         * 4. Saves the updated list to file
         *
         * Note: Does not check if score qualifies for top 5 before adding.
         * Call isHighScore() first if you want to check eligibility.
         */
        void addScore(const std::string& name, int score) {
            scores.emplace_back(name, score);
            std::sort(scores.begin(), scores.end(),
                [](const ScoreEntry& a, const ScoreEntry& b) {
                    return a.score > b.score;
                });
            if (scores.size() > MAX_SCORES) {
                scores.resize(MAX_SCORES);
            }
            saveScores();
        }

        /**
         * @brief Retrieves the top N scores
         * @param count Number of scores to retrieve (default 5)
         * @return Vector of ScoreEntry objects, sorted highest to lowest
         *
         * Always reloads from file before returning to ensure fresh data.
         * If fewer than count scores exist, returns all available scores.
         */
        std::vector<ScoreEntry> getTopScores(int count = MAX_SCORES) {
            loadScores();
            int numScores = std::min(count, static_cast<int>(scores.size()));
            return std::vector<ScoreEntry>(scores.begin(), scores.begin() + numScores);
        }

        /**
         * @brief Checks if a score qualifies for the top 5
         * @param score Score to check
         * @return true if score would make the top 5 list
         *
         * Returns true if:
         * - Less than 5 scores currently exist (always qualifies), OR
         * - Score is higher than the current 5th place score
         */
        bool isHighScore(int score) {
            if (scores.size() < MAX_SCORES) return true;
            return score > scores.back().score;
        }
    };

    // ============================================
    // SCORE (current game score with Observer)
    // ============================================

    /**
     * @enum ScoreEvent
     * @brief Types of events that affect score calculation
     *
     * These events are triggered by the World class when specific
     * game actions occur. The Score class responds to each event
     * with appropriate point calculations.
     */
    enum class ScoreEvent {
        CoinCollected,   ///< Pacman collected a coin (time-based multiplier applies)
        FruitCollected,  ///< Pacman collected a fruit (enables fear mode)
        GhostEaten,      ///< Pacman ate a ghost during fear mode (combo multiplier)
        PacmanDied,      ///< Pacman lost a life (resets timers and combos)
        LevelCleared     ///< All coins collected (bonus points awarded)
    };

    /**
     * @class Score
     * @brief Current game score calculation with time-based multipliers
     *
     * The Score class implements a dynamic scoring system that rewards skilled,
     * aggressive gameplay through time-based multipliers and combo systems.
     *
     * Scoring Rules:
     *
     * Coins (time-based multiplier):
     * - Base: 10 points
     * - If collected < 1 second after last coin: 3x multiplier (30 points)
     * - If collected < 3 seconds after last coin: 2x multiplier (20 points)
     * - If collected > 3 seconds after last coin: 1x multiplier (10 points)
     *
     * Fruits:
     * - Flat 50 points
     * - Resets ghost combo multiplier
     *
     * Ghosts (combo multiplier):
     * - 1st ghost: 200 points
     * - 2nd ghost: 400 points (2x)
     * - 3rd ghost: 800 points (4x)
     * - 4th ghost: 1600 points (8x)
     * - Combo resets when fear mode ends or Pacman dies
     *
     * Level Clear:
     * - Flat 500 bonus points
     *
     * Design Rationale:
     * This system creates multiple risk/reward decisions:
     * - Rush for coins quickly (higher multiplier) vs. play safe (lower multiplier)
     * - Hunt all 4 ghosts during fear mode (max 3000 points) vs. avoid them
     * - Balance speed with survival to maximize score across multiple levels
     *
     * @see Stopwatch For delta time used in timing calculations
     * @see Observer Base class for event notification pattern
     */
    class Score : public Observer {
    private:
        int currentScore;           ///< Running total of points earned
        float timeSinceLastCoin;    ///< Seconds elapsed since last coin collection
        int consecutiveGhosts;      ///< Number of ghosts eaten in current fear mode (combo counter)

        // Score constants
        static constexpr int BASE_COIN_SCORE = 10;        ///< Base points per coin before multiplier
        static constexpr int FRUIT_SCORE = 50;            ///< Fixed points for fruit collection
        static constexpr int BASE_GHOST_SCORE = 200;      ///< Points for first ghost (doubles with each)
        static constexpr int LEVEL_CLEAR_BONUS = 500;     ///< Bonus for completing a level

        // Time-based multiplier thresholds
        static constexpr float FAST_COLLECT_TIME = 1.0f;   ///< < 1 sec = 3x multiplier
        static constexpr float MEDIUM_COLLECT_TIME = 3.0f; ///< < 3 sec = 2x multiplier
        // > 3 seconds = 1x multiplier (base score)

    public:
        /**
         * @brief Constructs a Score object with initial values
         *
         * Initializes score to 0, timers to 0, and combo counter to 0.
         */
        Score() : currentScore(0), timeSinceLastCoin(0.0f), consecutiveGhosts(0) {}

        /**
         * @brief Generic notification method (required by Observer interface)
         *
         * This method is called by Subject::notify(), but the actual event handling
         * is done through specific methods (onCoinCollected, etc.) called directly
         * by the World class.
         *
         * Design Note: While this is technically an Observer, we use direct method
         * calls for clarity and type safety rather than encoding event types in
         * a generic notification system.
         */
        void onNotify() override {
            // Event handling done via specific methods below
        }

        /**
         * @brief Handles coin collection with time-based multiplier
         *
         * Calculates points based on time since last coin:
         * - < 1 second: 30 points (3x)
         * - < 3 seconds: 20 points (2x)
         * - > 3 seconds: 10 points (1x)
         *
         * Logs the result to console showing multiplier if applicable.
         * Resets the timer for the next coin.
         *
         * @note Called by World when Pacman collects a coin
         */
        void onCoinCollected() {
            Stopwatch& stopwatch = Stopwatch::getInstance();
            timeSinceLastCoin += stopwatch.getDeltaTime();

            int multiplier = 1;
            if (timeSinceLastCoin < FAST_COLLECT_TIME) {
                multiplier = 3;
            } else if (timeSinceLastCoin < MEDIUM_COLLECT_TIME) {
                multiplier = 2;
            }

            int points = BASE_COIN_SCORE * multiplier;
            currentScore += points;

            std::cout << "Coin collected! +" << points << " points";
            if (multiplier > 1) {
                std::cout << " (x" << multiplier << " speed bonus!)";
            }
            std::cout << " Total: " << currentScore << std::endl;

            timeSinceLastCoin = 0.0f; // Reset timer
        }

        /**
         * @brief Handles fruit collection
         *
         * Awards flat 50 points and resets the ghost combo counter
         * (since fruit starts a new fear mode).
         *
         * @note Called by World when Pacman collects a fruit
         */
        void onFruitCollected() {
            currentScore += FRUIT_SCORE;
            consecutiveGhosts = 0; // Reset ghost combo
            std::cout << "Fruit collected! +" << FRUIT_SCORE << " points. Total: " << currentScore << std::endl;
        }

        /**
         * @brief Handles ghost consumption during fear mode
         *
         * Increments combo counter and awards points with exponential multiplier:
         * - 1st ghost: 200 × 1 = 200
         * - 2nd ghost: 200 × 2 = 400
         * - 3rd ghost: 200 × 4 = 800
         * - 4th ghost: 200 × 8 = 1600
         *
         * Logs result showing combo multiplier.
         *
         * @note Called by World when Pacman eats a feared ghost
         * @note Combo continues across multiple ghosts in same fear mode
         */
        void onGhostEaten() {
            consecutiveGhosts++;
            int points = BASE_GHOST_SCORE * consecutiveGhosts;
            currentScore += points;
            std::cout << "Ghost eaten! +" << points << " points (x" << consecutiveGhosts << " combo). Total: " << currentScore << std::endl;
        }

        /**
         * @brief Handles Pacman death
         *
         * Resets combo counter and coin timer but preserves score.
         * This ensures progress isn't lost on death while preventing
         * exploitation of death for timer resets.
         *
         * @note Called by World when Pacman loses a life
         */
        void onPacmanDied() {
            consecutiveGhosts = 0; // Reset combo on death
            timeSinceLastCoin = 0.0f; // Reset coin timer
            std::cout << "Pacman died! Score preserved: " << currentScore << std::endl;
        }

        /**
         * @brief Handles level completion
         *
         * Awards flat 500 bonus points and resets timers/combos
         * for the next level.
         *
         * @note Called by World::nextLevel() when all coins are collected
         */
        void onLevelCleared() {
            currentScore += LEVEL_CLEAR_BONUS;
            consecutiveGhosts = 0;
            timeSinceLastCoin = 0.0f;
            std::cout << "Level cleared! +" << LEVEL_CLEAR_BONUS << " bonus points! Total: " << currentScore << std::endl;
        }

        /**
         * @brief Updates time-based tracking
         * @param deltaTime Time elapsed since last update (seconds)
         *
         * Increments the coin collection timer used for multiplier calculation.
         * Should be called once per frame by World::update().
         */
        void update(float deltaTime) {
            timeSinceLastCoin += deltaTime;
        }

        /**
         * @brief Gets the current score
         * @return Current accumulated score
         */
        [[nodiscard]] int getCurrentScore() const { return currentScore; }

        /**
         * @brief Resets score to initial state
         *
         * Sets score, timers, and combos back to zero.
         * Typically called when starting a new game.
         */
        void reset() {
            currentScore = 0;
            timeSinceLastCoin = 0.0f;
            consecutiveGhosts = 0;
        }
    };
}

#endif // SCORE_H