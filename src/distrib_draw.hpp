#pragma once

#include <assert.h>
#include "distrib_types.hpp"
#include "param_types.hpp"
#include "random.hpp"

/*==================================================================================================
~~ Raw drawing functions (with direct access to data) ~~
==================================================================================================*/
template <typename Gen>
void draw_exponential(double& node, double rate, Gen& gen) {
    assert(rate > 0);
    std::exponential_distribution<double> distrib(rate);
    node = distrib(gen);
}

template <typename Gen>
void draw_gamma(double& node, double shape, double scale, Gen& gen) {
    assert(shape > 0);
    assert(scale > 0);
    std::gamma_distribution<double> distrib(shape, scale);
    node = distrib(gen);
}

/*==================================================================================================
~~ Overloads that distringuish based on typing ~~
==================================================================================================*/

template <typename Rate, typename Gen>
void draw(distrib::exponential_t& node, Rate& rate, Gen& gen) {
    draw_exponential(node.value, rate(), gen);
}

template <typename Shape, typename Scale, typename Gen>
void draw(distrib::gamma_t& node, Shape shape, Scale scale, Gen& gen) {
    draw_gamma(node.value, shape(), scale(), gen);
}