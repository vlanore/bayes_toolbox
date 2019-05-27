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

template <class Distrib, class... ParamArgs>
auto make_node(ParamArgs&&... args) {
    auto v = typename Distrib::T();
    auto params = make_params<Distrib>(std::forward<ParamArgs>(args)...);
    return make_tagged_tuple(unique_ptr_field<struct value>(std::move(v)),
                             value_field<struct params>(params), property<distrib, Distrib>(),
                             tag<node_tag>());
}

template <class Distrib, class... ParamArgs>
auto make_node_array(size_t size, ParamArgs&&... args) {
    std::vector<typename Distrib::T> values(size);
    auto params = make_array_params<Distrib>(std::forward<ParamArgs>(args)...);
    return make_tagged_tuple(value_field<struct value>(std::move(values)),
                             value_field<struct params>(params), property<distrib, Distrib>(),
                             tag<node_tag>());
}

template <class T>
using is_node = ttuple_has_tag<T, node_tag>;

template <class Value>
using value_distrib_t = typename Value::distrib;

template <class Node>
using node_distrib_t = get_property<Node, distrib>;

template <class Node>
using node_value_t = std::remove_reference_t<decltype(get<value>(std::declval<Node>()))>;

template <class NodeArray>
using node_array_value_t =
    typename std::remove_reference_t<decltype(get<value>(std::declval<NodeArray>()))>::T;

template <class Node>
auto& get_raw_value(Node& node) {
    return get<value>(node).value;
}

template <class Node>
const auto& get_raw_value(const Node& node) {
    return get<value>(node).value;
}

template <class Node>
auto& get_array_raw_value(Node& node, size_t i) {
    return get<value>(node)[i].value;
}

template <class Node>
const auto& get_array_raw_value(const Node& node, size_t i) {
    return get<value>(node)[i].value;
}

template <class Distrib, class... ParamArgs>
auto make_backuped_node(ParamArgs&&... args) {
    auto node = make_node<Distrib>(std::forward<ParamArgs>(args)...);
    return push_front<backup_value, typename Distrib::T>(node, {});
}

template <class Distrib, class... ParamArgs>
auto make_backuped_node_array(size_t size, ParamArgs&&... args) {
    auto node = make_node_array<Distrib>(size, std::forward<ParamArgs>(args)...);
    using vec_t = std::vector<typename Distrib::T>;
    return push_front<backup_value, vec_t>(node, vec_t(size));
}