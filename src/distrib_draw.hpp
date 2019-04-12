#pragma once

#include <assert.h>
#include "node_types.hpp"
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
void draw(distrib::exponential::value_t& node, distrib::exponential::Param<Rate> param, Gen& gen) {
    draw_exponential(node.value, param.rate(), gen);
}

template <typename Shape, typename Scale, typename Gen>
void draw(distrib::gamma::value_t& node, distrib::gamma::Param<Shape, Scale> param, Gen& gen) {
    draw_gamma(node.value, param.shape(), param.scale(), gen);
}

/*==================================================================================================
~~ Generic version that unpacks probnode objects ~~
==================================================================================================*/
template <typename Value, typename Params, typename Gen>
void draw(ProbNodeRef<Value, Params> noderef, Gen& gen) {
    draw(noderef.value, noderef.params, gen);
}

template <typename Value, typename Params, typename Gen>
void draw(ProbNode<Value, Params>& node, Gen& gen) {
    draw(node.value, node.params, gen);
}