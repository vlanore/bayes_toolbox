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

#include "structure/distrib_utils.hpp"
#include "utils/math_utils.hpp"

struct gamma_ss {
    using T = pos_real;

    using param_decl = param_decl_t<param<shape, spos_real>, param<struct scale, spos_real>>;

    template <typename Gen>
    static void draw(T& x, spos_real shape, spos_real scale, Gen& gen) {
        std::gamma_distribution<double> distrib(positive_real(shape), positive_real(scale));
        x = {distrib(gen)};
    }

    static real logprob(T x, spos_real k, spos_real theta) {
        return -std::lgamma(k) - k * log(theta) + (k - 1) * log(x) - x / theta;
    }

    static real partial_logprob_value(T x, spos_real k, spos_real theta) {
        return (k - 1) * log(x) - x / theta;
    }

    static real partial_logprob_param1(T, spos_real k, spos_real theta) {
        return -std::lgamma(k) - k * log(theta) + (k - 1);
    }

    static real partial_logprob_param2(T x, spos_real k, spos_real theta) {
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

    static double logprob(gamma_ss_suffstats ss, spos_real k, spos_real theta) {
        return -ss.N * std::lgamma(k) - ss.N * k * log(theta) + (k - 1) * ss.sum_log -
               (1 / theta) * ss.sum;
    }
};

struct gamma_sr {
    using T = pos_real;

    using param_decl = param_decl_t<param<shape, spos_real>, param<struct rate, spos_real>>;

    template <typename Gen>
    static void draw(T& x, spos_real shape, spos_real rate, Gen& gen) {
        std::gamma_distribution<double> distrib(positive_real(shape), 1 / positive_real(rate));
        x = {distrib(gen)};
    }

    static real logprob(T x, spos_real alpha, spos_real beta) {
        return alpha * log(beta) - std::lgamma(alpha) + (alpha - 1) * log(x) - beta * x;
    }

    static real partial_logprob_value(T x, spos_real alpha, spos_real beta) {
        return (alpha - 1) * log(x) - beta * x;
    }

    static real partial_logprob_param1(T x, spos_real alpha, spos_real beta) {
        return alpha * log(beta) - std::lgamma(alpha) + (alpha - 1) * log(x);
    }

    static real partial_logprob_param2(T x, spos_real alpha, spos_real beta) {
        return alpha * log(beta) - beta * x;
    }
};

struct gamma_mi {
    using T = pos_real;

    using param_decl = param_decl_t<param<gam_mean, spos_real>, param<gam_invshape, spos_real>>;

    template <typename Gen>
    static void draw(T& x, spos_real mean, spos_real invshape, Gen& gen) {
        std::gamma_distribution<double> distrib(1. / positive_real(invshape),
                                                positive_real(mean) * positive_real(invshape));
        x = {distrib(gen)};
    }

    static real logprob(T x, spos_real mean, spos_real invshape) {
        double alpha = 1. / invshape;
        double beta = mean * invshape;
        return alpha * log(beta) - std::lgamma(alpha) + (alpha - 1) * log(x) - beta * x;
    }
};
