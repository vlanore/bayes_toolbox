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

#include <iostream>
#include "basic_moves.hpp"
#include "distributions/bernoulli.hpp"
#include "distributions/beta.hpp"
#include "distributions/exponential.hpp"
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

TOKEN(beta_weight_a)
TOKEN(beta_weight_b)
TOKEN(p)
TOKEN(bern)

// Model definition
auto bernoulli_model(size_t size) {
    auto beta_weight_a = make_node<exponential>(1.);
    auto beta_weight_b = make_node<exponential>(1.);
    auto p = make_node<beta_ss>(beta_weight_a, beta_weight_b);  // p ~ U(0,1)
    auto bern = make_node_array<bernoulli>(size, n_to_one(p));  // bern ~ Bern(p)
    // clang-format off
    return make_model(
            beta_weight_a_ = move(beta_weight_a),
            beta_weight_b_ = move(beta_weight_b),
            p_ = move(p),
            bern_ = move(bern)
    );                      // clang-format on
}

int main() {
    auto gen = make_generator();

    constexpr size_t nb_it{100'000};
    int n_obs = 2;
    auto m = bernoulli_model(n_obs);
    auto v = make_view<beta_weight_a, beta_weight_b, p, bern>(m);
    draw(v, gen);
    set_value(bern_(m), {true, true});

    auto v_weight_a = make_view<beta_weight_a, p>(m);
    auto v_weight_b = make_view<beta_weight_b, p>(m);

    double p_sum{0};
    for (size_t it = 0; it < nb_it; ++it) {
        // propose move for p, provided a Markov blanket of p
        scaling_move(beta_weight_a_(m), logprob_of_blanket(v_weight_a), gen);
        scaling_move(beta_weight_b_(m), logprob_of_blanket(v_weight_b), gen);
        slide_constrained_move(p_(m), logprob_of_blanket(v), gen, 0., 1.);
        p_sum += raw_value(p_(m));
    }
    float p_mean = p_sum / float(nb_it);
    std::cout << "p = " << p_mean << std::endl;
    return 0;
}