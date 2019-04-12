#pragma once

#include <functional>
#include "ref_types.hpp"

CDRef make_param(const double& value) { return CDRef(value); }

DRef make_param(double& value) { return DRef(value); }

auto make_param(double&& value) {
    return [value]() { return value; };
}

auto make_param(std::function<double()> f) { return f; }

namespace distrib {
    namespace exponential {
        template <typename Rate>
        struct Param {
            Rate rate;
        };

        template <typename Rate>
        auto make_params(Rate&& rate) {
            Param<decltype(make_param(rate))> result = {make_param(rate)};
            return result;
        }
    };  // namespace exponential

    namespace gamma {
        template <typename Rate, typename Scale>
        struct Param {
            Rate shape;
            Scale scale;
        };

        template <typename Shape, typename Scale>
        auto make_params(Shape&& shape, Scale&& scale) {
            Param<decltype(make_param(shape)), decltype(make_param(scale))> result = {
                make_param(shape), make_param(scale)};
            return result;
        }
    };  // namespace gamma
};      // namespace distrib