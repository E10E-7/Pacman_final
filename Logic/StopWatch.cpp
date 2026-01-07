//
// Stopwatch.cpp
//

#include "Stopwatch.h"

namespace Logic {

    Stopwatch::Stopwatch()
        : lastTime(Clock::now()),
          deltaTime(0.0f),
          paused(false) {}

    Stopwatch& Stopwatch::getInstance() {
        static Stopwatch instance;
        return instance;
    }

    void Stopwatch::update() {
        if (paused) {
            // When paused, don't accumulate time but update lastTime
            // so when we resume we don't get a huge delta
            lastTime = Clock::now();
            deltaTime = 0.0f;
            return;
        }

        TimePoint currentTime = Clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        deltaTime = elapsed.count();
        lastTime = currentTime;
    }

    void Stopwatch::reset() {
        lastTime = Clock::now();
        deltaTime = 0.0f;
        paused = false;
    }

    void Stopwatch::pause() {
        paused = true;
    }

    void Stopwatch::resume() {
        if (paused) {
            paused = false;
            lastTime = Clock::now(); // Reset time to avoid huge delta
        }
    }

} // namespace Logic