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

#include "introspection.hpp"

/* A value view is a functor that takes a function and applies it to a set of values.
For example, a value view of a node array would be a function that given a function f calls f on all
the values of the elements of the array.
It's a simple wrapper around a lambda. It statically checks that the lambda can accept a function as
parameter. */
template <class ItFunc>
struct ValueView {
    ItFunc itfunc;

    template <class F>
    void operator()(F&& f) {
        itfunc(std::forward<F>(f));
    }

    static_assert(is_itfunc<ItFunc>::value,
                  "BAYES_TOOLBOX ERROR: Trying to build a view with a function that cannot accept "
                  "functions as arguments.");
};

namespace overloads {

    template <class ItFunc>
    auto make_valueview_impl(unknown_tag, ItFunc itf) {  // assuming unknown = lambda
        return ValueView<ItFunc>{std::forward<ItFunc>(itf)};
    }

    template <class Node>
    auto make_valueview_impl(lone_node_tag, Node& node) {
        return make_valueview_impl(unknown_tag{}, [&ref = get<value>(node)](auto&& f) { f(ref); });
    }

    template <class Node>
    auto make_valueview_impl(node_array_tag, Node& node) {
        return make_valueview_impl(unknown_tag{}, [&ref = get<value>(node)](auto&& f) {
            for (auto& e : ref) { f(e); }
        });
    }

    template <class Node>
    auto make_valueview_impl(node_matrix_tag, Node& node) {
        return make_valueview_impl(unknown_tag{}, [&ref = get<value>(node)](auto&& f) {
            for (auto& row : ref) {
                for (auto& e : row) { f(e); }
            }
        });
    }

}  // namespace overloads

template <class T>
auto make_valueview(T&& x) {
    return overloads::make_valueview_impl(type_tag(x), std::forward<T>(x));
}

template <class... Args>
auto make_valueview_collection(Args&&... args) {
    return make_valueview([col = std::make_tuple(make_valueview(args)...)](auto&& f) {
        // function that takes an element of the collection (an itfunc) and passes f to it
        auto g = [f](auto&& itf) mutable { itf(f); };
        apply_to_tuple_helper(std::move(g), std::move(col),
                              std::make_index_sequence<sizeof...(Args)>());
    });
}

// associated type trait
template <class T>
struct is_valueview : std::false_type {};

template <class ItFunc>
struct is_valueview<ValueView<ItFunc>> : std::true_type {};