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

#include <vector>
#include "tagged_tuple/src/tagged_tuple.hpp"
#include "tags.hpp"
#include "utils/random.hpp"

//==================================================================================================
// helper

template <class T, class Tag>
struct has_meta_tag : std::false_type {};

template <class Tag, class MD, class... Fields>
struct has_meta_tag<tagged_tuple<MD, Fields...>, Tag> : metadata_has_tag<Tag, MD> {};

//==================================================================================================
// node traits

template <class T>
using is_node = has_meta_tag<T, node_tag>;

template <class T>
struct is_vector : std::false_type {};

template <class T>
struct is_vector<std::vector<T>> : std::true_type {};

template <class T>
using is_lone_node = has_meta_tag<T, lone_node_tag>;

template <class T>
using is_node_array = has_meta_tag<T, node_array_tag>;

template <class T>
using is_node_matrix = has_meta_tag<T, node_matrix_tag>;

//==================================================================================================
// distrib traits (***)

template <class... Ts>
using to_void = void;

template <class T, class = void>
struct has_array_logprob : std::false_type {};

template <class T>
struct has_array_logprob<T, to_void<decltype(T::array_logprob)>> : std::true_type {};

template <class T, class = void>
struct has_array_draw : std::false_type {};

template <class T>
struct has_array_draw<T, to_void<decltype(T::template array_draw<decltype(make_generator())>)>>
    : std::true_type {};

//==================================================================================================
// node introspection

template <class Node>
using node_distrib_t = metadata_get_property<struct distrib, metadata_t<Node>>;

template <class Distrib>
using param_keys_t = map_key_list_t<typename Distrib::param_decl>;

//==================================================================================================
// model traits

template <class T>
struct is_model : std::false_type {};

template <class MD, class... Fields>
struct is_model<tagged_tuple<MD, Fields...>> : metadata_has_tag<model_tag, MD> {};

template <class M>
using model_nodes = map_key_list_t<field_map_t<M>>;

//==================================================================================================
// view traits

template <class...>
struct View;  // forward-decl to avoid silly include

template <class T>
struct is_view : std::false_type {};

template <class... Refs>
struct is_view<View<Refs...>> : std::true_type {};

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
struct is_itfunc<T, to_void<decltype(std::declval<T>()(helper::generic_test_functor{}))>>
    : std::true_type {};

// Checks that ItF
template <template <class> class Trait, class ItF>
struct check_itfunc_type {
    static_assert(is_itfunc<ItF>::value,
                  "BAYES_TOOLBOX ERROR: Trying to build a view with a function that cannot accept "
                  "functions as arguments.");
    using type = decltype(std::declval<ItF>()(helper::trait_checker_functor<Trait>{}));
};