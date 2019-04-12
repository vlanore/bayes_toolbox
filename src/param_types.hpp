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
            Param<decltype(ParamFactory<double>::make(rate))> result = {
                ParamFactory<double>::make(rate)};
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
            Param<decltype(ParamFactory<double>::make(shape)),
                  decltype(ParamFactory<double>::make(scale))>
                result = {ParamFactory<double>::make(shape), ParamFactory<double>::make(scale)};
            return result;
        }
    };  // namespace gamma
};      // namespace distrib