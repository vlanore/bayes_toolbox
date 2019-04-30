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
#include "tags.hpp"

/*==================================================================================================
~~ Raw drawing functions ~~
==================================================================================================*/
template <typename Gen>
double draw_uniform(Gen& gen) {
    std::uniform_real_distribution<double> distrib(0, 1);
    return distrib(gen);
}

/*==================================================================================================
~~ Overloads that unpack parameters ~~
==================================================================================================*/
template <typename Distrib, typename Param, typename Gen, size_t... Is>
auto draw_helper(Param& param, Gen& gen, std::index_sequence<Is...>) {
    return Distrib::draw(std::get<Is>(param.data)()..., gen);
}

template <typename Value, typename Param, typename Gen>
void draw(Value& node, Param& param, Gen& gen) {
    auto is = std::make_index_sequence<Param::tag_map::size()>();
    using distrib = typename std::remove_reference<decltype(node)>::type::template type_of<distrib>;
    node.template get<raw_value>() = draw_helper<distrib>(param, gen, is);
}

/*==================================================================================================
~~ Generic version that unpacks probnode objects ~~
==================================================================================================*/
template <class ProbNode, typename Gen>
void draw(ProbNode& node, Gen& gen) {
    draw(node.template get<value>(), node.template get<params>(), gen);
}