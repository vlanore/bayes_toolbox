#pragma once

namespace distrib {
    namespace exponential {
        struct value_t {
            double value;
        };
    };  // namespace exponential

    namespace gamma {
        struct value_t {
            double value;
        };
    };  // namespace gamma

    namespace constant {
        struct value_t {
            const double value;
        };
    };  // namespace constant
};      // namespace distrib