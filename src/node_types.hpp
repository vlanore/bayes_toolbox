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
#include "distrib_types.hpp"
#include "param_types.hpp"

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
auto make_probnode_ref(Value& value, Params& params) {
    ProbNodeRef<Value, Params> result = {value, params};
    return result;
}

template <typename Constructor>
auto make_probnode_array(size_t size, Constructor f) {
    std::vector<decltype(f(0))> result;
    for (size_t i = 0; i < size; i++) { result.push_back(f(i)); }
    return result;
}

template <typename Value, typename Params, typename... Values>
void clamp_array(std::vector<ProbNode<Value, Params>>& nodes, Values... values) {
    assert(nodes.size() > 0);
    using raw_type = decltype(nodes.at(0).value.value);
    std::vector<raw_type> value_vec{values...};
    assert(nodes.size() == value_vec.size());
    for (size_t i = 0; i < nodes.size(); i++) { nodes.at(i).value.value = value_vec.at(i); }
}

namespace distrib {
    namespace exponential {
        template <typename Rate>
        auto make_node(Rate&& rate) {
            ProbNode<distrib::exponential::value_t,
                     decltype(distrib::exponential::make_params(std::forward<Rate>(rate)))>
                result = {{0.}, distrib::exponential::make_params(std::forward<Rate>(rate))};
            return result;
        }
    };  // namespace exponential

    namespace gamma {
        template <typename Shape, typename Scale>
        auto make_node(Shape&& shape, Scale&& scale) {
            ProbNode<distrib::gamma::value_t,
                     decltype(distrib::gamma::make_params(std::forward<Shape>(shape),
                                                          std::forward<Scale>(scale)))>
                result = {{0.},
                          distrib::gamma::make_params(std::forward<Shape>(shape),
                                                      std::forward<Scale>(scale))};
            return result;
        }
    };  // namespace gamma

    namespace poisson {
        template <typename Rate>
        auto make_node(Rate&& rate) {
            ProbNode<distrib::poisson::value_t,
                     decltype(distrib::poisson::make_params(std::forward<Rate>(rate)))>
                result = {{0}, distrib::poisson::make_params(std::forward<Rate>(rate))};
            return result;
        }
    };  // namespace poisson
};      // namespace distrib