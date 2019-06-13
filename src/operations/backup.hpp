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

#include "raw_value.hpp"

namespace overloads {
    template <class T>  // @todo: add complex_node_tag or something to simplify here
    auto backup(node_matrix_tag, T& node, NoIndex = NoIndex()) {
        return get<value>(node);
    }

    template <class T>
    auto backup(node_matrix_tag, T& node, ArrayIndex index) {
        assert(index.i >= 0 && index.i < get<value>(node).size());
        return get<value>(node)[i];
    }

    template <class T>
    auto backup(node_matrix_tag, T& node, MatrixIndex index) {
        return raw_value(node, index);
    }

    template <class T>
    auto backup(node_array_tag, T& node, NoIndex = NoIndex()) {
        return get<value>(node);
    }

    template <class T>
    auto backup(node_array_tag, T& node, ArrayIndex index) {
        return raw_value(node, index);
    }

    template <class T, class Index>
    auto backup(lone_node_tag, T& node, NoIndex = NoIndex()) {
        return raw_value(node);
    }

};  // namespace overloads

template <class T, class... IndexArgs>
auto backup(T& x, IndexArgs... args) {
    return overloads::backup(type_tag(x), x, make_index(args...));
}