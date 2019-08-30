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

#include <tuple>
#include "ValueView.hpp"
#include "type_tag.hpp"

/*==================================================================================================
~~ Node views ~~
==================================================================================================*/
template <template <class> class Trait, class ItF>  // ItF: iteration function
struct TraitView {
    ItF itf;

    template <class... Args>
    void operator()(Args&&... args) {
        itf(std::forward<Args>(args)...);
    }

    // checks that itf actually iterates over nodes
    using check_itf = typename check_itfunc_type<Trait, ItF>::type;
};

template <template <class> class Trait, class ItF>
auto make_trait_view(ItF&& itf) {
    return TraitView<Trait, ItF>{std::forward<ItF>(itf)};
}

template <class F, class... Ts, size_t... Is>
void apply_to_tuple_helper(F f, std::tuple<Ts...> t, std::index_sequence<Is...>) {
    // @todo: check if unpacking as function args would be more performant
    std::vector<int> ignore = {(f(get<Is>(t)), 0)...};
}

template <class... Nodes>
auto node_collection(Nodes&... nodes) {
    // have to name param pack to avoid g++5 bug
    return make_trait_view<is_node>([col = std::tuple<Nodes&...>(nodes...)](auto&& f) {
        apply_to_tuple_helper(std::forward<decltype(f)>(f), col,
                              std::make_index_sequence<sizeof...(Nodes)>());
    });
}

/*==================================================================================================
~~ ith/jth itfunc generator generators ~~
==================================================================================================*/

template <class Node>  // for multi-variable view collections
auto ith_element(Node& node) {
    static_assert(is_node_array<Node>::value, "Expects a node array");
    return [&node](size_t i, size_t = 0) { return element_itfunc(raw_value(node, i)); };
}

template <class Node>  // for multi-variable view collections
auto jth_element(Node& node) {
    static_assert(is_node_array<Node>::value, "Expects a node array");
    return [&node](size_t, size_t j) { return element_itfunc(raw_value(node, j)); };
}

template <class Node>  // for multi-variable view collections
auto ijth_element(Node& node) {
    static_assert(is_node_matrix<Node>::value, "Expects a node matrix");
    return [&node](size_t i, size_t j) { return element_itfunc(raw_value(node, i, j)); };
}

/*==================================================================================================
~~ itfunc collections ~~
==================================================================================================*/

// @todo: allow direct node references and value views
template <class... ItFs>
auto make_valueview_collection_i(ItFs&&... itfs) {
    // @todo: check all args are itfuncs
    return [col = std::make_tuple(itfs...)](size_t i) {
        return make_valueview([col, i](auto&& f) {
            // function that takes an element of the collection (an itfunc) and passes f to it
            auto g = [i, f](auto&& itf) mutable { itf(i)(f); };
            apply_to_tuple_helper(std::move(g), std::move(col),
                                  std::make_index_sequence<sizeof...(ItFs)>());
        });
    };
}

/*==================================================================================================
~~ Value-index views ~~
==================================================================================================*/