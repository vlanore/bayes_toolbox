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

#include "distrib_utils.hpp"
#include "math_utils.hpp"

struct gamma_ss {
    using raw_type = double;

    using T = distrib_value_type<raw_type, gamma_ss>;

    using param_decl = param_decl_t<param<shape, double>, param<struct scale, double>>;

    template <typename Gen>
    static double draw(double shape, double scale, Gen& gen) {
        std::gamma_distribution<double> distrib(positive_real(shape), positive_real(scale));
        return distrib(gen);
    }

    static double logprob(double x, double k, double theta) {
        return -std::lgamma(k) - k * log(theta) + (k - 1) * log(x) - x / theta;
    }

    static double partial_logprob_value(double x, double k, double theta) {
        return (k - 1) * log(x) - x / theta;
    }

    static double partial_logprob_param1(double, double k, double theta) {
        return -std::lgamma(k) - k * log(theta) + (k - 1);
    }

    static double partial_logprob_param2(double x, double k, double theta) {
        return -k * log(theta) - x / theta;
    }
};

struct gamma_ss_suffstats {
    double sum;
    double sum_log;
    size_t N;

    using distrib = gamma_ss;

    static gamma_ss_suffstats gather(const std::vector<typename gamma_ss::T>& array) {
        return {::sum(array), ::sum(array), array.size()};
    }

    static double logprob(gamma_ss_suffstats ss, double k, double theta) {
        return -ss.N * std::lgamma(k) - ss.N * k * log(theta) + (k - 1) * ss.sum_log -
               (1 / theta) * ss.sum;
    }
};