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

#include "operations/across_values_params.hpp"
#include "structure/distrib_utils.hpp"
#include "structure/type_tag.hpp"

namespace overloads {
    template <class Node, class Index>
    double select_logprob(std::false_type /* no array lprob */, node_tag, Node& node, Index index) {
        double result = 0;
        auto f = [&result](const auto& x, const auto&... params) {
            result += node_distrib_t<Node>::logprob(x, params...);
        };
        across_values_params(node, f, index);
        return result;
    }

    template <class Node, class... Keys>
    double compute_array_logprob(Node& node, std::tuple<Keys...>) {
        return node_distrib_t<Node>::array_logprob(get<value>(node),
                                                   get<Keys...>(get<params>(node))());
    }

    template <class Node>
    double select_logprob(std::true_type /* array logprob */, node_array_tag, Node& node, NoIndex) {
        return compute_array_logprob(node, param_keys_t<node_distrib_t<Node>>());
    }

    template <class Node, class Index>
    double logprob(node_tag, Node& node, Index index) {
        return select_logprob(has_array_logprob<node_distrib_t<Node>>(), type_tag(node), node,
                              index);
    }  // namespace overloads

    template <class View>
    double logprob(view_tag, View& view, NoIndex = NoIndex()) {
        double result = 0;
        forall_in_view(view, [&result](auto& node, auto index) { result += logprob(node, index); });
        return result;
    }
};  // namespace overloads

template <class T, class... Rest>
double logprob(T& x, Rest... rest) {
    return overloads::logprob(type_tag(x), x, make_index(rest...));
}