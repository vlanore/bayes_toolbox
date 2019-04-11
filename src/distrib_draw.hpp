#pragma once

#include "distrib_types.hpp"
#include "random.hpp"

template <typename Gen>
void draw_exponential(double& node, double rate, Gen& gen) {
    std::exponential_distribution<double> distrib(rate);
    node = distrib(gen);
}

template <typename Gen>
void draw(distrib::exponential_t& node, double rate, Gen& gen) {
    draw_exponential(node.value, rate, gen);
}