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
#include "param_types.hpp"
using std::forward;

template <typename Value, typename Params>
struct ProbNode {
    Value value;
    Params params;
};

template <typename Value, typename Params>
struct ProbNodeRef {
    Value& value;
    Params& params;
};

template <typename Value, typename Params>
struct ProbNodeValueRef {
    Value& value;
    Params params;
};

template <typename Value, typename Params>
auto make_probnode_ref(Value& value, Params& params) {
    ProbNodeRef<Value, Params> result = {value, params};
    return result;
}

template <typename Value, typename... ParamsArgs>
auto make_probnode_vref(Value& value, ParamsArgs&&... args) {
    using distrib = typename Value::distrib;
    auto params = distrib::make_params(forward<ParamsArgs>(args)...);
    return ProbNodeValueRef<Value, decltype(params)>{value, params};
}

template <typename Constructor>
auto make_array(size_t size, Constructor f) {
    std::vector<decltype(f(0))> result;
    for (size_t i = 0; i < size; i++) { result.push_back(f(i)); }
    return result;
}

template <typename Value, typename Params>
struct ProbNodeArray {
    std::vector<Value> values;
    std::vector<Params> params;
    std::vector<ProbNodeRef<Value, Params>> nodes;
};

template <typename Distrib, typename... Params>
auto make_probnode_array(size_t size, Params&&... params) {
    auto param_struct = Distrib::make_params(forward<Params>(params)...);
    ProbNodeArray<typename Distrib::value_t, decltype(param_struct)> result;
    result.values.reserve(size);
    result.params.reserve(size);
    for (size_t i = 0; i < size; i++) {
        result.values.push_back({0});
        result.params.push_back(param_struct);
        result.nodes.push_back({result.values.back(), result.params.back()});
    }
    return result;
}

template <typename Value, typename Params, typename... Values>
void clamp_array(ProbNodeArray<Value, Params>& array, Values... values) {
    using raw_type = typename Value::distrib::raw_type;
    std::vector<raw_type> value_vec{values...};
    assert(array.values.size() == value_vec.size());
    for (size_t i = 0; i < array.values.size(); i++) { array.values.at(i).value = value_vec.at(i); }
}