#pragma once

#include <assert.h>
#include "node_types.hpp"
#include "random.hpp"

double positive_real(double input) {
    assert(input >= 0);
    if (input == 0) {
        return DBL_MIN;
    } else {
        return input;
    }
}

/*==================================================================================================
~~ Raw drawing functions (with direct access to data) ~~
==================================================================================================*/
template <typename Gen>
void draw_exponential(double& node, double rate, Gen& gen) {
    std::exponential_distribution<double> distrib(positive_real(rate));
    node = distrib(gen);
    // printf("drawn %f from param %f\n", node, rate);
}

template <typename Gen>
void draw_gamma(double& node, double shape, double scale, Gen& gen) {
    std::gamma_distribution<double> distrib(positive_real(shape), positive_real(scale));
    node = distrib(gen);
}

template <typename Gen>
void draw_poisson(int& node, double rate, Gen& gen) {
    std::poisson_distribution<int> distrib(positive_real(rate));
    node = distrib(gen);
}

/*==================================================================================================
~~ Overloads that distinguish based on typing ~~
==================================================================================================*/
template <typename Rate, typename Gen>
void draw(distrib::exponential::value_t& node, distrib::exponential::Param<Rate>& param, Gen& gen) {
    draw_exponential(node.value, param.rate(), gen);
}

template <typename Shape, typename Scale, typename Gen>
void draw(distrib::gamma::value_t& node, distrib::gamma::Param<Shape, Scale>& param, Gen& gen) {
    draw_gamma(node.value, param.shape(), param.scale(), gen);
}

template <typename Rate, typename Gen>
void draw(distrib::poisson::value_t& node, distrib::poisson::Param<Rate>& param, Gen& gen) {
    draw_poisson(node.value, param.rate(), gen);
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