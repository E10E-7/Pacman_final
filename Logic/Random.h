// Logic/Random.h
#ifndef PACMAN_RETRY_RANDOM_H
#define PACMAN_RETRY_RANDOM_H

#include <random>

class Random {
private:
    std::mt19937 mt;
    Random() {
        std::random_device rd;
        mt.seed(rd());
    }

public:
    static Random& getInstance() {
        static Random instance;
        return instance;
    }

    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

    // Returns true with probability p (0.0 to 1.0)
    bool nextBool(float probability = 0.5f) {
        std::bernoulli_distribution dist(probability);
        return dist(mt);
    }

    int nextInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(mt);
    }
};

#endif // PACMAN_RETRY_RANDOM_H