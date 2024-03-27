#pragma once

#include <cstddef>
#include <random>

namespace BJTUGE {

class Random {

    static size_t random() {
        static std::random_device                    rd;
        static std::mt19937_64                       gen(rd());
        static std::uniform_int_distribution<size_t> dist;
        return dist(gen);
    }

    static size_t random(size_t min, size_t max) { return min + (random() % (max - min + 1)); };

    static size_t generateID() { return random(); }
};

} // namespace BJTUGE