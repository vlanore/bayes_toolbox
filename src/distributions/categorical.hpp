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

struct categorical {
    using T = pos_integer;

    using param_decl = param_decl_t<param<weights, std::vector<double>>>;

    template <typename Gen>
    static void draw(T& x, const std::vector<double>& w, Gen& gen) {
        std::discrete_distribution<T> distrib(w.begin(), w.end());
        x = distrib(gen);
    }

    static double logprob(const T& x, const std::vector<double>& w) { return log(w[x]); }

    template <class LogProb, typename Gen>
    static void gibbs_resample(T& x, LogProb logprob, const std::vector<double>& w, Gen& gen)  {
        std::vector<double> logp(w.size(),0);
        double max = 0;
        for (size_t i=0; i<w.size(); i++)   {
            logp[i] = log(w[i]) + logprob(i);
            if (!i || (max < logp[i]))  {
                max = logp[i];
            }
        }
        double tot = 0;
        std::vector<double> post(w.size(),0);
        for (size_t i=0; i<w.size(); i++)   {
            post[i] = exp(logp[i] - max);
            tot += post[i];
        }
        for (size_t i=0; i<w.size(); i++)   {
            post[i] /= tot;
        }
        std::discrete_distribution<T> distrib(post.begin(), post.end());
        x = distrib(gen);
    }
};
