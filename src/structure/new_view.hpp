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
#include "introspection.hpp"
#include "operations/raw_value.hpp"

template <class F, class KeyList>
struct UseNodeContext {
    F f;
};

template <class F, class... Keys, class Node, class... Indices>
auto apply(UseNodeContext<F, type_list<Keys...>> cf, Node& node, Indices... is) {
    cf.f(node_distrib_t<Node>{}, raw_value(node, is...), get<params, Keys>(node)(is...)...);
}

template <class F, class Node, class... Indices>
auto apply(F f, Node& node, Indices... is) {
    f(raw_value(node, is...));
}

/*==================================================================================================
~~ Node subset ~~
==================================================================================================*/
template <class Node, class Subset>
class NodeSubset {
    Node& node;
    Subset subset;

  public:
    NodeSubset(Node& node, Subset&& subset) : node(node), subset(std::forward<Subset>(subset)) {}

    template <class F>
    void across_values(F f) {
        subset(node, f);
    }

    template <class F>
    void across_nodes(F f) {
        subset(node, UseNodeContext<F, param_keys_t<node_distrib_t<Node>>>{f});
    }
};

template <class Node, class Subset>
auto make_subset(Node& node, Subset&& subset) {
    return NodeSubset<Node, Subset>(node, std::forward<Subset>(subset));
}

/*==================================================================================================
~~ Pre-made subset lambdas ~~
==================================================================================================*/
struct subsets {
    template <class Node>
    static auto element(Node& node, size_t i) {
        return make_subset(node, [i](auto& node, auto f) {
            static_assert(is_node_array<std::decay_t<decltype(node)>>::value,
                          "Expects a node array");
            apply(f, node, i);
        });
    }

    template <class Node>
    static auto element(Node& node, size_t i, size_t j) {
        return make_subset(node, [i, j](auto& node, auto f) {
            static_assert(is_node_matrix<std::decay_t<decltype(node)>>::value,
                          "Expects a node matrix");
            apply(f, node, i, j);
        });
    }

    template <class Node>
    static auto column(Node& node, size_t j) {
        return make_subset(node, [j](auto& node, auto f) {
            static_assert(is_node_matrix<std::decay_t<decltype(node)>>::value,
                          "Expects a node matrix");
            for (size_t i = 0; i < get<value>(node).size(); i++) { apply(f, node, i, j); }
        });
    }

    template <class Node>
    static auto row(Node& node, size_t i) {
        return make_subset(node, [i](auto& node, auto f) {
            static_assert(is_node_matrix<std::decay_t<decltype(node)>>::value,
                          "Expects a node matrix");
            for (size_t j = 0; j < get<value>(node)[i].size(); j++) { apply(f, node, i, j); }
        });
    }
};

/*==================================================================================================
~~ Set collection ~~
==================================================================================================*/
template <class... Sets>
class SetCollection {
    std::tuple<Sets...> sets;

    template <class F, size_t... is>
    void across_elements_helper(F f, std::index_sequence<is...>) {
        std::vector<int> ignore = {(f(get<is>(sets)), 0)...};
    }

    template <class F, size_t... is>
    auto gather_across_elements_helper(F f, std::index_sequence<is...>) {
        return std::make_tuple(f(get<is>(sets))...);
    }

    template <class F, class... Elements, size_t... is>
    auto joint_across_elements_helper(F f, std::tuple<Elements...>& other,
                                      std::index_sequence<is...>) {
        std::vector<int> ignore = {(f(get<is>(sets), get<is>(other)), 0)...};
    }

  public:
    SetCollection(Sets&&... sets) : sets(std::forward<Sets>(sets)...) {}

    template <class F>
    void across_elements(F f) {
        across_elements_helper(f, std::index_sequence_for<Sets...>{});
    }

    template <class F, class... Elements>
    void joint_across_elements(F f, std::tuple<Elements...>& other) {
        static_assert(sizeof...(Sets) == sizeof...(Elements), "Wrong other size");
        joint_across_elements_helper(f, other, std::index_sequence_for<Sets...>{});
    }

    template <class F>
    auto gather_across_elements(F f) {
        return gather_across_elements_helper(f, std::index_sequence_for<Sets...>{});
    }
};

template <class... Sets>
auto make_collection(Sets&&... sets) {
    return SetCollection<Sets...>(std::forward<Sets>(sets)...);
}
