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

#include "structure/distrib_utils.hpp"
#include "structure/type_tag.hpp"

//==================================================================================================
// Parameter unpacking

template <typename Distrib, class T = typename Distrib::raw_value, typename Param,
          class... ParamKeys, class... Indexes>
auto logprob_helper(const T& value, const Param& param, std::tuple<ParamKeys...>,
                    Indexes... indexes) {
    return Distrib::logprob(value, get<ParamKeys>(param)(indexes...)...);
}

// template <class SS, class Param>
// double logprob(const SS& ss, const Param& param) {
//     using distrib = typename SS::distrib;
//     using keys = map_key_list_t<typename distrib::param_decl>;
//     // TODO check that params are identical?
//     return logprob_helper<SS, SS>(ss, param, keys(), 0);
// }

//==================================================================================================
// Overloads

namespace overloads {
    template <class SS>
    double logprob(suffstat_tag, SS& ss) {
        using ss_t = metadata_get_property<suffstat_type, metadata_t<SS>>;
        return logprob<ss_t>(get<suffstat>(ss), get<params>(ss));
    }

    template <class Node>
    double logprob(lone_node_tag, Node& node, NoIndex = NoIndex{}) {
        using distrib = node_distrib_t<Node>;
        using keys = map_key_list_t<typename distrib::param_decl>;
        return logprob_helper<distrib>(raw_value(node), get<params>(node), keys());
    }

    template <class Node>
    double logprob(node_array_tag, Node& node, ArrayIndex index) {
        using distrib = node_distrib_t<Node>;
        using keys = map_key_list_t<typename distrib::param_decl>;
        return logprob_helper<distrib>(raw_value(node, index), get<params>(node), keys(), index.i);
    }

    template <class Node>
    double logprob(node_array_tag, Node& node, NoIndex = NoIndex{}) {
        using distrib = node_distrib_t<Node>;
        using keys = map_key_list_t<typename distrib::param_decl>;
        double result = 0;
        auto& v = get<value>(node);
        for (size_t i = 0; i < v.size(); i++) {
            result += logprob_helper<distrib>(raw_value(node, i), get<params>(node), keys(), i);
        }
        return result;
    }

    template <class Node>
    double logprob(node_matrix_tag, Node& node, NoIndex = NoIndex{}) {
        using distrib = node_distrib_t<Node>;
        using keys = map_key_list_t<typename distrib::param_decl>;
        double result = 0;
        auto& v = get<value>(node);
        for (size_t i = 0; i < v.size(); i++) {
            for (size_t j = 0; j < v[i].size(); j++) {
                result +=
                    logprob_helper<distrib>(raw_value(node, i, j), get<params>(node), keys(), i, j);
            }
        }
        return result;
    }

    // @todo: add 3 matrix overloads (with NoIndex, ArrayIndex and MatrixIndex)

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