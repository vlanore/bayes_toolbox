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
#include "params.hpp"

template <class Tag, class Distrib>
using node_metadata = metadata<type_list<node_tag, Tag>, type_map<property<distrib, Distrib>>>;

template <class Distrib, class... ParamArgs>
auto make_node(ParamArgs&&... args) {
    auto v = typename Distrib::T();
    auto params = make_params<Distrib>(std::forward<ParamArgs>(args)...);
    return make_tagged_tuple<node_metadata<lone_node_tag, Distrib>>(
        unique_ptr_field<struct value>(std::move(v)), value_field<struct params>(params));
}

template <class Distrib, class... ParamArgs>
auto make_node_array(size_t size, ParamArgs&&... args) {
    std::vector<typename Distrib::T> values(size);
    auto params = make_array_params<Distrib>(std::forward<ParamArgs>(args)...);
    return make_tagged_tuple<node_metadata<node_array_tag, Distrib>>(
        unique_ptr_field<struct value>(std::move(values)), value_field<struct params>(params));
}

template <class T>
struct is_node : std::false_type {};

template <class MD, class... Fields>
struct is_node<tagged_tuple<MD, Fields...>> : metadata_has_tag<node_tag, MD> {};

template <class T>
struct is_vector : std::false_type {};

template <class T>
struct is_vector<std::vector<T>> : std::true_type {};

template <class...>  // @todo: remove or move elsewhere
using void_type = void;

template <class T, class = void>
struct is_array : std::false_type {};

template <class MD, class... Fields>
struct is_array<tagged_tuple<MD, Fields...>> : metadata_has_tag<node_array_tag, MD> {};

template <class Value>
using value_distrib_t = typename Value::distrib;

template <class Node>
using node_distrib_t = metadata_get_property<struct distrib, metadata_t<Node>>;

template <class Node>
using node_value_t = std::remove_reference_t<decltype(get<value>(std::declval<Node>()))>;

template <class NodeArray>
using node_array_value_t =
    typename std::remove_reference_t<decltype(get<value>(std::declval<NodeArray>()))>::T;