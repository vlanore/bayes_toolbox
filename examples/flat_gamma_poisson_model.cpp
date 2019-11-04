/*Copyright or © or Copr. CNRS (2019). Contributors2
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
more generally, to use and opebeta_ it in the same conditions as regards security.

The fact that you are presently reading this means that you have had knowledge of the CeCILL-C
license and that you accept its terms.*/

#include <iostream>
#include "bayes_toolbox.hpp"
using namespace std;

TOKEN(alpha_)
TOKEN(beta_)
TOKEN(lambda)
TOKEN(K)

auto poisson_gamma(size_t size) {
    auto alpha_ = make_node<exponential>(1.0);
    auto beta_ = make_node<exponential>(1.0);
    auto lambda = make_node_array<gamma_sr>(size, n_to_one(alpha_), n_to_one(beta_));
    auto K = make_node_array<poisson>(size, n_to_n(lambda));

    // clang-format off
    return make_model(
        alpha__ = move(alpha_),
         beta__ = move(beta_),
        lambda_ = move(lambda),
             K_ = move(K)
    );  // clang-format on
}

int main() {
    auto gen = make_generator();

    size_t nb_it = 100000;
    size_t n = 5;

    auto m = poisson_gamma(n);

    auto v = make_collection(alpha__(m), beta__(m), lambda_(m), K_(m));

    // Generate initial model parameters, including "observed" values K
    draw(v, gen);
    // Save simulated values
    auto bkp_alpha = get<alpha_, value>(m);
    auto bkp_beta = get<beta_, value>(m);
    auto bkp_lambda = get<lambda, value>(m);
    auto bkp_K = get<K, value>(m);

    // Reset model
    draw(v, gen);
    // Set observations
    set_value(K_(m), bkp_K);

    double alpha__sum{0}, beta__sum{0};

    for (size_t it = 0; it < nb_it; it++) {

        auto logprob_lambda = [&lam = lambda_(m)] () {return flat_logprob(lam);};
        flat_scaling_move(alpha__(m), logprob_lambda, gen);
        flat_scaling_move(beta__(m), logprob_lambda, gen);

        auto logprob_K = [&m] (int i) {return flat_logprob(K_(m), i);};
        flat_scaling_move(lambda_(m), logprob_K, gen);

        alpha__sum += raw_value(alpha__(m));
        beta__sum += raw_value(beta__(m));
    }

    std::cout << "alpha_MCMC = " << alpha__sum / float(nb_it) << "\n"
              << "alpha_bkp = " << bkp_alpha << "\n"
              << "beta_ = " << beta__sum / float(nb_it) << "\n"
              << "beta_bkp = " << bkp_beta << "\n";
}
