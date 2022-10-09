#pragma once
#include <random>

// https://stackoverflow.com/questions/7114043/random-number-generation-in-c11-how-to-generate-how-does-it-work
class Random {
private:
    std::random_device dev;
    std::mt19937 rng;

public:
    Random() { rng = std::mt19937(dev()); }

    int gen(int min, int max) {
        std::uniform_int_distribution<std::mt19937::result_type> rand(min, max);
        return rand(rng);
    }
};
