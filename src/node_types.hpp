#pragma once

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

namespace distrib {
    namespace exponential {
        template <typename Rate>
        auto make_node(Rate&& rate) {
            ProbNode<distrib::exponential::value_t,
                     decltype(distrib::exponential::make_params(rate))>
                result = {{0.}, distrib::exponential::make_params(rate)};
            return result;
        }
    };  // namespace exponential

    namespace gamma {
        template <typename Shape, typename Scale>
        auto make_node(Shape&& shape, Scale&& scale) {
            ProbNode<distrib::gamma::value_t, decltype(distrib::gamma::make_params(shape, scale))>
                result = {{0.}, distrib::gamma::make_params(shape, scale)};
            return result;
        }
    };  // namespace gamma
};      // namespace distrib