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
#include "structure/type_tag.hpp"

/*==================================================================================================
~~ Static itfunc checking ~~
==================================================================================================*/
namespace helper {
    // --
    template <template <class> class Trait>
    struct trait_checker_functor {
        template <class T>
        void operator()(T&&) {
            static_assert(Trait<std::decay_t<T>>::value,
                          "BAYES_TOOLBOX ERROR: Trying to build a view with a function that calls "
                          "its passed function on the wrong type. For example, trying to build a "
                          "node_view with a function that iterates over values.");
        }
    };

    struct generic_test_functor {
        template <class T>
        void operator()(T&&) {}
    };
}  // namespace helper

template <class T, class = void>
struct is_itfunc : std::false_type {};

template <class T>
struct is_itfunc<T, to_void<decltype(std::declval<T>()(helper::generic_test_functor{})),
                            decltype(std::declval<T>()(helper::generic_test_functor{}, 0, 1))>>
    : std::true_type {};

// Checks that ItF
template <template <class> class Trait, class ItF>
struct check_itfunc_type {
    static_assert(is_itfunc<ItF>::value,
                  "BAYES_TOOLBOX ERROR: Trying to build a view with a function that cannot accept "
                  "functions as arguments.");
    using type = decltype(std::declval<ItF>()(helper::trait_checker_functor<Trait>{}));
};

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
    return make_trait_view<is_node>([col = std::tuple<Nodes&...>(nodes...)](auto&& f, auto&&...) {
        apply_to_tuple_helper(std::forward<decltype(f)>(f), col,
                              std::make_index_sequence<sizeof...(Nodes)>());
    });
}

/*==================================================================================================
~~ Value views ~~
==================================================================================================*/
template <class ItF>  // ItF: iteration function
struct ValueView {
    ItF itf;

    template <class... Args>
    void operator()(Args&&... args) {
        itf(std::forward<Args>(args)...);
    }

    // @todo check itf
};

template <class ItF>
auto make_value_view(ItF&& itf) {
    return ValueView<ItF>{std::forward<ItF>(itf)};
}

/*================================================================================================*/
template <class T>
auto element_itfunc(T& ref) {
    return [&ref](auto&& f) { f(ref); };
}

template <class T>
auto vector_itfunc(std::vector<T>& v) {
    return [&v](auto&& f) {
        for (auto e : v) { f(e); };
    };
}

template <class Node, class... Indexes>
auto element(Node& node, Indexes... is) {
    // @todo: check that sizeof...(Indexes) == dim(node)
    return element_itfunc(raw_value(node, is...));
}

template <class Node, class... Indexes>
auto row(Node& node, size_t row_nb) {
    static_assert(is_node_matrix<Node>::value, "Expects a node matrix");
    return vector_itfunc(get<value>(node).at(row_nb));
}
/*================================================================================================*/

template <class Node>  // for multi-variable view collections
auto ith_element(Node& node) {
    static_assert(is_node_array<Node>::value, "Expects a node array");
    return make_value_view([&v = get<value>(node)](auto&& f, size_t i, auto...) { f(v[i]); });
}

template <class Node>  // for multi-variable view collections
auto jth_element(Node& node) {
    static_assert(is_node_array<Node>::value, "Expects a node array");
    return make_value_view([&v = get<value>(node)](auto&& f, auto, size_t j) { f(v[j]); });
}

// /*==================================================================================================
// ~~ Value-index views ~~
// ==================================================================================================*/