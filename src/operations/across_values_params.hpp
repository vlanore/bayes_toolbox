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
#include "structure/node.hpp"
#include "structure/view.hpp"

namespace overloads {
    template <class T, class F, class Params, class... Keys, class... Indexes>
    void unpack_params(T& x, const F& f, const Params& params, std::tuple<Keys...>, Indexes... is) {
        f(x, get<Keys>(params)(is...)...);
    }

    template <class Node, class F>
    void across_values_params(lone_node_tag, Node& n, const F& f, NoIndex) {
        using keys = param_keys_t<node_distrib_t<Node>>;
        unpack_params(raw_value(n), f, get<params>(n), keys());
    }

    template <class Array, class F>
    void across_values_params(node_array_tag, Array& a, const F& f, NoIndex) {
        using keys = param_keys_t<node_distrib_t<Array>>;
        for (size_t i = 0; i < get<value>(a).size(); i++) {
            unpack_params(raw_value(a, i), f, get<params>(a), keys(), i);
        }
    }

    template <class Array, class F>
    void across_values_params(node_array_tag, Array& a, const F& f, ArrayIndex index) {
        using keys = param_keys_t<node_distrib_t<Array>>;
        unpack_params(raw_value(a, index.i), f, get<params>(a), keys(), index.i);
    }

    template <class Matrix, class F>
    void across_values_params(node_matrix_tag, Matrix& m, const F& f, NoIndex) {
        using keys = param_keys_t<node_distrib_t<Matrix>>;
        for (size_t i = 0; i < get<value>(m).size(); i++) {
            for (size_t j = 0; j < get<value>(m)[i].size(); j++) {
                unpack_params(raw_value(m, i, j), f, get<params>(m), keys(), i, j);
            }
        }
    }

    template <class Matrix, class F>
    void across_values_params(node_matrix_tag, Matrix& m, const F& f, ArrayIndex index) {
        using keys = param_keys_t<node_distrib_t<Matrix>>;
        for (size_t j = 0; j < get<value>(m)[index.i].size(); j++) {
            unpack_params(raw_value(m, index.i, j), f, get<params>(m), keys(), index.i, j);
        }
    }

    template <class Matrix, class F>
    void across_values_params(node_matrix_tag, Matrix& m, const F& f, MatrixIndex index) {
        using keys = param_keys_t<node_distrib_t<Matrix>>;
        unpack_params(raw_value(m, index.i, index.j), f, get<params>(m), keys(), index.i, index.j);
    }
};  // namespace overloads

template <class T, class F, class... IndexArgs>
void across_values_params(T& x, F&& f, IndexArgs... args) {
    overloads::across_values_params(type_tag(x), x, std::forward<F>(f), make_index(args...));
}