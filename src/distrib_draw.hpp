#pragma once

#include <assert.h>
#include "distrib_types.hpp"
#include "param_types.hpp"
#include "random.hpp"

template <typename Gen>
void draw_exponential(double& node, double rate, Gen& gen) {
    assert(rate > 0);
    std::exponential_distribution<double> distrib(rate);
    node = distrib(gen);
}

template <typename Gen>
void draw(distrib::exponential_t& node, distrib::exponential_param_t param, Gen& gen) {
    draw_exponential(node.value, param.rate, gen);
}

template <typename Gen>
void draw_gamma(double& node, double shape, double scale, Gen& gen) {
    assert(shape > 0);
    assert(scale > 0);
    std::gamma_distribution<double> distrib(shape, scale);
    node = distrib(gen);
}

template <typename Gen>
void draw(distrib::gamma_t& node, distrib::gamma_param_t param, Gen& gen) {
    draw_gamma(node.value, param.shape, param.scale, gen);
}