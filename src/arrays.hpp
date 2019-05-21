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

#include <functional>
#include <vector>
#include "tagged_tuple/src/tagged_tuple.hpp"

// reference types
template <class T>
struct ArrayParamFactory {
    static auto make(std::function<T(int)> f) { return f; }
};

// declaration struct and functions
namespace helper {
    template <int index, class ParamDecl>
    auto make_array_param_impl() {
        return make_tagged_tuple();
    }

    template <int index, class ParamDecl, class First, class... Rest>
    auto make_array_param_impl(First&& first, Rest&&... rest) {
        auto recursive_call =
            make_array_param_impl<index + 1, ParamDecl>(std::forward<Rest>(rest)...);

        using field = list_element_t<ParamDecl, index>;
        using field_tag = first_t<field>;
        using field_type = second_t<field>;

        auto param = ArrayParamFactory<field_type>::make(std::forward<First>(first));
        return push_front<field_tag>(recursive_call, std::move(param));
    }
};  // namespace helper

template <class Distrib, class... Args>
auto make_array_params(Args&&... args) {
    using param_decl = typename Distrib::param_decl;
    return helper::make_array_param_impl<0, param_decl>(std::forward<Args>(args)...);
}
