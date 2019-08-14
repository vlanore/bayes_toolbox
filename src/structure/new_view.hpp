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
~~ Node views ~~
==================================================================================================*/
namespace helper {
    // --
    template <template <class> class Trait>
    struct trait_checker {
        template <class T>
        void operator()(T&&) {
            static_assert(Trait<std::decay_t<T>>::value,
                          "BAYES_TOOLBOX ERROR: Trying to build a view with a function that calls "
                          "its passed function on the wrong type. For example, trying to build a "
                          "node_view with a function that iterates over values.");
        }
    };

    template <template <class> class Trait, class ItF>
    struct check_iterator_type {
        using type = decltype(std::declval<ItF>()(trait_checker<Trait>{}));
    };
}  // namespace helper

template <class ItF>  // ItF: iteration function
struct node_view {
    ItF itf;

    template <class F>
    void operator()(F&& f) {
        itf(std::forward<F>(f));
    }

    // checks that itf actually iterates over nodes
    using check = typename helper::check_iterator_type<is_node, ItF>::type;
};

template <class ItF>
auto make_node_view(ItF&& itf) {
    return node_view<ItF>{std::forward<ItF>(itf)};
}

template <class F, class... Ts, size_t... Is>
void apply_to_tuple_helper(F f, std::tuple<Ts...> t, std::index_sequence<Is...>) {
    // @todo: check if unpacking as function args would be more performant
    std::vector<int> ignore = {(f(get<Is>(t)), 0)...};
}

template <class... Nodes>
auto node_collection(Nodes&... nodes) {
    return make_node_view([col = std::tuple<Nodes&...>(nodes...)](auto&& f) {
        apply_to_tuple_helper(std::forward<decltype(f)>(f), col,
                              std::make_index_sequence<sizeof...(Nodes)>());
    });
}

/*==================================================================================================
~~ Value views ~~
==================================================================================================*/
template <class ItF>  // ItF: iteration function
struct value_view {
    ItF itf;

    template <class F>
    void operator()(F&& f) {
        itf(std::forward<F>(f));
    }
};

/*==================================================================================================
~~ Value-index views ~~
==================================================================================================*/
template <class ItF>  // ItF: iteration function
struct value_i_view {
    ItF itf;

    template <class F>
    void operator()(F&& f) {
        itf(std::forward<F>(f));
    }
};

template <class T>
auto get_apply_single(T& x) {
    return [&x](auto&& f) { return f(raw_value(x)); };
}

template <class T>
auto get_apply_array(T& x) {
    static_assert(is_node_array<T>::value, "Expects node array");
    return [&v = get<value>(x)](auto&& f) {
        for (auto& e : v) { f(e); }
    };
}
