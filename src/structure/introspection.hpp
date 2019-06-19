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
// node traits

template <class T>
struct is_node : std::false_type {};

template <class MD, class... Fields>
struct is_node<tagged_tuple<MD, Fields...>> : metadata_has_tag<node_tag, MD> {};

template <class T>
struct is_vector : std::false_type {};

template <class T>
struct is_vector<std::vector<T>> : std::true_type {};

template <class T, class = void>
struct is_node_array : std::false_type {};

template <class MD, class... Fields>
struct is_node_array<tagged_tuple<MD, Fields...>> : metadata_has_tag<node_array_tag, MD> {};

template <class T, class = void>
struct is_node_matrix : std::false_type {};

template <class MD, class... Fields>
struct is_node_matrix<tagged_tuple<MD, Fields...>> : metadata_has_tag<node_matrix_tag, MD> {};

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

template <class Node>
using node_value_t = std::remove_reference_t<decltype(get<value>(std::declval<Node>()))>;

template <class NodeArray>  // @todo: remove?
using node_array_value_t =
    typename std::remove_reference_t<decltype(get<value>(std::declval<NodeArray>()))>::T;