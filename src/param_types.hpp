#pragma once

#include <functional>
#include "ref_types.hpp"

template <class T>
struct ParamFactory {
    static CRef<T> make(const T& value) { return CRef<T>(value); }

    static Ref<T> make(T& value) { return Ref<T>(value); }

    static auto make(T&& value) {
        return [value]() { return value; };
    }

    static auto make(std::function<T()> f) { return f; }
};

namespace distrib {
    namespace exponential {
        template <typename Rate>
        struct Param {
            Rate rate;
        };

        template <typename Rate>
        auto make_params(Rate&& rate) {
            Param<decltype(ParamFactory<double>::make(std::forward<Rate>(rate)))> result = {
                ParamFactory<double>::make(std::forward<Rate>(rate))};
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
            Param<decltype(ParamFactory<double>::make(std::forward<Shape>(shape))),
                  decltype(ParamFactory<double>::make(std::forward<Scale>(scale)))>
                result = {ParamFactory<double>::make(std::forward<Shape>(shape)),
                          ParamFactory<double>::make(std::forward<Scale>(scale))};
            return result;
        }
    };  // namespace gamma

    namespace poisson {
        template <typename Rate>
        struct Param {
            Rate rate;
        };

        template <typename Rate>
        auto make_params(Rate&& rate) {
            Param<decltype(ParamFactory<double>::make(std::forward<Rate>(rate)))> result = {
                ParamFactory<double>::make(std::forward<Rate>(rate))};
            return result;
        }
    };  // namespace poisson
};      // namespace distrib