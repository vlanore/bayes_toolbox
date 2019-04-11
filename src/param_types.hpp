#pragma once

namespace distrib {
    struct exponential_param_t {
        const double& rate;
    };

    struct gamma_param_t {
        const double& shape;
        const double& scale;
    };
};  // namespace distrib