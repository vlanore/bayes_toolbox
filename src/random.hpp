#pragma once

#include <random>

std::mt19937 make_generator(int seed) { return std::mt19937(seed); }

std::mt19937 make_generator() {
    std::random_device rd;
    return make_generator(rd());
}