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
more generally, to use and opebeta_ it in the same conditions as regards security.

The fact that you are presently reading this means that you have had knowledge of the CeCILL-C
license and that you accept its terms.*/

#include <iostream>
#include "basic_moves.hpp"
#include "distributions/exponential.hpp"
#include "distributions/gamma.hpp"
#include "distributions/poisson.hpp"
#include "mcmc_utils.hpp"
#include "operations/backup.hpp"
#include "operations/logprob.hpp"
#include "operations/raw_value.hpp"
#include "operations/set_value.hpp"
#include "structure/View.hpp"
#include "structure/array_utils.hpp"
#include "suffstat_utils.hpp"
#include "tagged_tuple/src/fancy_syntax.hpp"
using namespace std;

TOKEN(alpha_)
TOKEN(beta_)
TOKEN(lambda)
TOKEN(K)

auto poisson_gamma(size_t size, size_t size2) {
    auto alpha_ = make_node<exponential>(1.0);
    auto beta_ = make_node<exponential>(1.0);
    auto lambda = make_node_array<gamma_sr>(size, n_to_one(alpha_), n_to_one(beta_));
    auto K = make_node_matrix<poisson>(size, size2,
                                       [& v = get<value>(lambda)](int i, int) { return v[i]; });
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

    constexpr size_t nb_it{100'000}, len_lambda{5}, len_K{3};
    auto m = poisson_gamma(len_lambda, len_K);

    auto v = make_view<alpha_, beta_, lambda, K>(m);

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

    double alpha__sum{0}, beta__sum{0}, lambda_sum{0};

    for (size_t it = 0; it < nb_it; it++) {
        scaling_move(alpha__(m), logprob_of_blanket(make_view<alpha_, lambda>(m)), gen);
        scaling_move(beta__(m), logprob_of_blanket(make_view<beta_, lambda>(m)), gen);
        alpha__sum += raw_value(alpha__(m));
        beta__sum += raw_value(beta__(m));

        for (size_t i = 0; i < len_lambda; i++) {
            auto lambda_mb = make_view(make_ref<K>(m, i), make_ref<lambda>(m, i));
            mh_move(lambda_(m), logprob_of_blanket(lambda_mb),
                    [i](auto& value, auto& gen) { return scale(value[i], gen); }, gen);
            lambda_sum += raw_value(lambda_(m), i);
        }
    }

    std::cout << "alpha_MCMC = " << alpha__sum / float(nb_it) << "\n"
              << "alpha_bkp = " << bkp_alpha << "\n"
              << "beta_ = " << beta__sum / float(nb_it) << "\n"
              << "beta_bkp = " << bkp_beta << "\n";
}