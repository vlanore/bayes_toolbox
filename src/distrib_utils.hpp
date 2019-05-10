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

#include <cmath>
#include <type_traits>
#include "math_utils.hpp"
#include "param_types.hpp"
#include "random.hpp"
#include "tagged_tuple/src/tagged_tuple.hpp"
#include "tags.hpp"

template <class... Pairs>
using param_decl = type_map::Map<Pairs...>;

template <class ParamTag, class ParamRawValue>
using param = utils::Pair<ParamTag, ParamRawValue>;

namespace helper {
    template <class RawParamType, class Value>
    auto param_builder(std::true_type /* is a node */, Value& v) {
        return ParamFactory<RawParamType>::make(get<value, raw_value>(v));
    }

    template <class RawParamType, class Value>
    auto param_builder(std::false_type /* is not a node */, Value&& value) {
        return ParamFactory<RawParamType>::make(std::forward<Value>(value));
    }

    template <class ParamDecl, int index>
    auto make_params_helper() {
        return tagged_tuple<>();
    }

    template <class ParamDecl, int index, class First, class... Rest>
    auto make_params_helper(First&& first, Rest&&... rest) {
        using field_tag = typename ParamDecl::template get_tag<index>;
        using field_type = typename ParamDecl::template get<field_tag>;

        constexpr bool param_is_tuple =
            is_tagged_tuple<typename std::remove_reference<First>::type>;  // assuming tuple means
                                                                           // it's a node
        auto param = param_builder<field_type>(std::integral_constant<bool, param_is_tuple>(),
                                               std::forward<First>(first));

        auto recursive_call = make_params_helper<ParamDecl, index + 1>(std::forward<Rest>(rest)...);
        return push_front<field_tag>(recursive_call, std::move(param));
        // return recursive_call.template push_front<field_tag>(std::move(param));
    }
};  // namespace helper

template <class Distrib, class... ParamArgs>
auto make_params(ParamArgs&&... args) {
    using param_decl = typename Distrib::param_decl;
    static_assert(sizeof...(ParamArgs) == param_decl::size(),
                  "Number of args does not match expected number");
    return helper::make_params_helper<param_decl, 0>(std::forward<ParamArgs>(args)...);
}

template <class Distrib, class... ParamArgs>
auto make_node(ParamArgs&&... args) {
    auto v = typename Distrib::value_t();
    auto params = make_params<Distrib>(std::forward<ParamArgs>(args)...);
    return make_tagged_tuple(unique_ptr_field<struct value>(std::move(v)),
                             value_field<struct params>(params));
}