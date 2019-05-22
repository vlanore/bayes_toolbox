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
#include <type_traits>
#include "tagged_tuple/src/tagged_tuple.hpp"
#include "tags.hpp"

//==================================================================================================
template <class... Pairs>
using param_decl_t = minimpl::map<Pairs...>;

template <class ParamTag, class ParamRawValue>
using param = minimpl::pair<ParamTag, ParamRawValue>;

//==================================================================================================
template <class T>
struct ParamFactory {
    static auto make(const T& value) {
        return [&value]() -> const T& { return value; };
    }

    static auto make(T& value) {
        return [&value]() { return value; };
    }

    static auto make(T&& value) {
        return [value]() { return value; };
    }

    static auto make(std::function<T()> f) { return f; }
};

// reference types
template <class T>
struct ArrayParamFactory {
    static auto make(std::function<T(int)> f) { return f; }
};

//==================================================================================================
namespace helper {
    using namespace minimpl;

    template <class Factory, class Value>
    auto param_builder(std::true_type /* is a node */, Value& v) {
        return Factory::make(get<value, raw_value>(v));
    }

    template <class Factory, class Value>
    auto param_builder(std::false_type /* is not a node */, Value&& value) {
        return Factory::make(std::forward<Value>(value));
    }

    template <class, int, template <class> class>
    auto make_params_helper() {
        return tagged_tuple<>();
    }

    template <class ParamDecl, int index, template <class> class Factory, class First,
              class... Rest>
    auto make_params_helper(First&& first, Rest&&... rest) {
        using field = list_element_t<ParamDecl, index>;
        using field_tag = first_t<field>;
        using field_type = second_t<field>;
        using specialized_factory = Factory<field_type>;
        using param_is_tuple = is_tagged_tuple<std::remove_reference_t<First>>;

        auto param =
            param_builder<specialized_factory>(param_is_tuple(), std::forward<First>(first));
        auto recursive_call =
            make_params_helper<ParamDecl, index + 1, Factory>(std::forward<Rest>(rest)...);

        return push_front<field_tag>(recursive_call, std::move(param));
    }
};  // namespace helper

//==================================================================================================
template <class Distrib, class... ParamArgs>
auto make_params(ParamArgs&&... args) {
    using param_decl = typename Distrib::param_decl;
    static_assert(sizeof...(ParamArgs) == param_decl::size,
                  "Number of args does not match expected number");
    return helper::make_params_helper<param_decl, 0, ParamFactory>(
        std::forward<ParamArgs>(args)...);
}

template <class Distrib, class... ParamArgs>
auto make_array_params(ParamArgs&&... args) {
    using param_decl = typename Distrib::param_decl;
    static_assert(sizeof...(ParamArgs) == param_decl::size,
                  "Number of args does not match expected number");
    return helper::make_params_helper<param_decl, 0, ArrayParamFactory>(
        std::forward<ParamArgs>(args)...);
}
