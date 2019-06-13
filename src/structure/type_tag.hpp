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

#include "node.hpp"
#include "reference.hpp"
#include "suffstat_utils.hpp"
#include "tags.hpp"
#include "view.hpp"

// @todo: have subfolders in repo

template <class T>
auto type_tag(const T&) {
    return unknown_tag();
}

template <class MD, class... Fields>
auto type_tag(const tagged_tuple<MD, Fields...>&) {
    using std::conditional_t;
    using T = tagged_tuple<MD, Fields...>;
    // clang-format off
    return conditional_t<is_node<T>::value,
        conditional_t<is_node_array<T>::value,
            node_array_tag,
            conditional_t<is_node_matrix<T>::value,
                node_matrix_tag,
                lone_node_tag
            >
        >,
        conditional_t<is_model<T>::value,
            model_tag,
            conditional_t<is_suffstat<T>::value,
                suffstat_tag,
                unknown_tag>
            >
        >();
    // clang-format off
}

template <class... Refs>
auto type_tag(const view<Refs...>&) {
    return view_tag();
}

template <class Node, class Index>
auto type_tag(const ref<Node, Index>&) {
    return ref_tag();
}