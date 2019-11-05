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

#include "gamma.hpp"

struct dirichlet {
    using T = std::vector<double>;
    using param_decl = param_decl_t<param<concentration, std::vector<double>>>;

    template <typename Gen>
    static void draw(T& x, const std::vector<double>& alpha, Gen& gen) {
        size_t k = x.size();
        assert(k == alpha.size());
        double sum_y{0};
        for (size_t i = 0; i < k; i++) {
            gamma_sr::draw(x[i], alpha[i], 1, gen);
            sum_y += x[i];
        }
        for (size_t i = 0; i < k; i++) { x[i] /= sum_y; }
    }

    static double logprob(T& x, const std::vector<double>& alpha) {
        size_t k = x.size();
        assert(k = alpha.size());
        double sum_alpha{0}, sum_lgam_alpha{0}, sum_alpha_logx{0};
        for (size_t i = 0; i < k; i++) {
            sum_alpha += alpha[i];
            sum_lgam_alpha += std::lgamma(alpha[i]);
            sum_alpha_logx += (alpha[i] - 1) * log(x[i]);
        }
        return sum_alpha_logx + std::lgamma(sum_alpha) - sum_lgam_alpha;
    }

    template <class SS, typename Gen>
    static void gibbs_resample(T& x, SS& ss, const std::vector<double>& alpha, Gen& gen)  {
        size_t k = x.size();
        assert(k == alpha.size());
        assert(k == ss.size());
        double sum_y{0};
        for (size_t i = 0; i < k; i++) {
            gamma_sr::draw(x[i], alpha[i] + ss[i], 1, gen);
            sum_y += x[i];
        }
        for (size_t i = 0; i < k; i++) { x[i] /= sum_y; }
    }
};

struct dirichlet_cic {
    using T = std::vector<double>;
    using param_decl = param_decl_t<param<center, std::vector<double>>, param<invconc, double>>;

    template <typename Gen>
    static void draw(T& x, const std::vector<double>& center, double invconc, Gen& gen) {
        size_t k = x.size();
        assert(k == center.size());
        double sum_y{0};
        for (size_t i = 0; i < k; i++) {
            gamma_sr::draw(x[i], center[i] / invconc, 1, gen);
            sum_y += x[i];
        }
        for (size_t i = 0; i < k; i++) { x[i] /= sum_y; }
    }

    static double logprob(T& x, const std::vector<double>& center, double invconc) {
        size_t k = x.size();
        assert(k == center.size());
        double sum_alpha{0}, sum_lgam_alpha{0}, sum_alpha_logx{0};
        for (size_t i = 0; i < k; i++) {
            double alpha = center[i] / invconc;
            sum_alpha += alpha;
            sum_lgam_alpha += std::lgamma(alpha);
            sum_alpha_logx += (alpha - 1) * log(x[i]);
        }
        return sum_alpha_logx + std::lgamma(sum_alpha) - sum_lgam_alpha;
    }
};
