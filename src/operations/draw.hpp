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

#include "across_values_params.hpp"
#include "structure/distrib_utils.hpp"
#include "structure/type_tag.hpp"

/*==================================================================================================
~~ Raw drawing functions ~~
==================================================================================================*/
template <typename Gen>
double draw_uniform(Gen& gen) {  // @todo: move elsewhere
    std::uniform_real_distribution<double> distrib(0, 1);
    return distrib(gen);
}

/*==================================================================================================
~~ Generic version that unpacks probnode objects ~~
==================================================================================================*/
namespace overloads {
    template <class Node, class Index, typename Gen>
    void select_draw(std::false_type /*no array draw*/, node_tag, Node& node, Index index,
                     Gen& gen) {
        auto f = [&gen](auto& x, const auto&... params) {
            x = node_distrib_t<Node>::draw(params..., gen);
        };
        across_values_params(node, f, index);
    }

    template <class Node, typename Gen, class... Keys>
    void perform_draw_array(Node& node, Gen& gen, std::tuple<Keys...>) {
        node_distrib_t<Node>::array_draw(get<value>(node), get<Keys>(get<params>(node))()..., gen);
    }

    template <class Node, typename Gen>
    void select_draw(std::true_type /*array draw*/, node_array_tag, Node& node, NoIndex, Gen& gen) {
        perform_draw_array(node, gen, param_keys_t<node_distrib_t<Node>>());
    }

    template <class Node, class Index, typename Gen>
    void draw(node_tag, Node& node, Index index, Gen& gen) {
        select_draw(has_array_draw<node_distrib_t<Node>>(), type_tag(node), node, index, gen);
    }

    template <class View, typename Gen>
    void draw(view_tag, View& view, NoIndex, Gen& gen) {
        // @todo: transmit index
        forall_in_view(view, [&gen](auto& node, NoIndex) { draw(node, gen); });
    }
}  // namespace overloads

template <class T, class... IndexArgs, class Gen>
void draw(T& x, Gen& gen, IndexArgs... args) {
    overloads::draw(type_tag(x), x, make_index(args...), gen);
}