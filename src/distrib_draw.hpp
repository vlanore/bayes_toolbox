/*Copyright or © or Copr. CNRS (2019). Contributors:
- Vincent Lanore. vincent.lanore@gmail.com

This software is a computer program whose purpose is to provide a set of C++ data structures and
functions to perform Bayesian inference with MCMC algorithms.

This software is governed by the CeCILL-C license under French law and abiding by the rules of
distribution of free software. You can use, modify and/ or redistribute the software under the terms
of the CeCILL-C license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and rights to copy, modify and redistribute
granted by the license, users are provided only with a limited warranty and the software's author,
the holder of the economic rights, and the successive licensors have only limited liability.

In this respect, the user's attention is drawn to the risks associated with loading, using,
modifying and/or developing or reproducing the software by the user in light of its specific status
of free software, that may mean that it is complicated to manipulate, and that also therefore means
that it is reserved for developers and experienced professionals having in-depth computer knowledge.
Users are therefore encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or data to be ensured and,
more generally, to use and operate it in the same conditions as regards security.

The fact that you are presently reading this means that you have had knowledge of the CeCILL-C
license and that you accept its terms.*/

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