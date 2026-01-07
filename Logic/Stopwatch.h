//
// Stopwatch.h - Time management (Singleton pattern)
//

#ifndef PACMAN_STOPWATCH_H
#define PACMAN_STOPWATCH_H

#include <chrono>

namespace Logic {

    class Stopwatch {
    private:
        using Clock = std::chrono::high_resolution_clock;
        using TimePoint = std::chrono::time_point<Clock>;

        TimePoint lastTime;
        float deltaTime;
        bool paused;

        // Private constructor
        Stopwatch();

    public:
        // Delete copy constructor and assignment operator
        Stopwatch(const Stopwatch&) = delete;
        Stopwatch& operator=(const Stopwatch&) = delete;

        // Get singleton instance
        static Stopwatch& getInstance();

        // Update the stopwatch (call once per frame)
        void update();

        // Get time since last update in seconds
        [[nodiscard]] float getDeltaTime() const { return paused ? 0.0f : deltaTime; }

        // Reset the stopwatch
        void reset();

        // Pause/Resume
        void pause();
        void resume();
        [[nodiscard]] bool isPaused() const { return paused; }
    };

} // namespace Logic

#endif //PACMAN_STOPWATCH_H