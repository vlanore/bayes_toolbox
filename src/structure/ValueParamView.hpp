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

namespace helper {

    template <class Param>
    auto param_from_packed_index(Param& param, NoIndex) {
        return param();
    }

    template <class Param>
    auto param_from_packed_index(Param& param, ArrayIndex index) {
        return param(index.i);
    }

    template <class Param>
    auto param_from_packed_index(Param& param, MatrixIndex index) {
        return param(index.i, index.j);
    }

    template <class F, class Value, class Index, class Params, class... Keys>
    void apply_to_value_param_tuple(F&& f, Value& x, Index index, Params& params,
                                    type_list<Keys...>) {
        f(x, param_from_packed_index(get<Keys>(params), index)...);
    }

}  // namespace helper

template <class ItFunc>
struct ValueParamView {
    ItFunc itfunc;

    template <class F>
    void operator()(F&& f) {
        itfunc(std::forward<F>(f));
    }

    // static_assert(is_itfunc<ItFunc>::value,
    //               "BAYES_TOOLBOX ERROR: Trying to build a view with a function that cannot accept
    //               "
    //               "functions as arguments.");  //@fixme: not sure it works for this case
};

// make function to auto-deduce lambda type
template <class ItFunc>
auto make_valueparamview(ItFunc&& itf) {
    return ValueParamView<ItFunc>{std::forward<ItFunc>(itf)};
}

// associated type trait
template <class T>
struct is_valueparamview : std::false_type {};

template <class ItFunc>
struct is_valueparamview<ValueParamView<ItFunc>> : std::true_type {};
