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

#include "array_utils.hpp"
#include "backup.hpp"
#include "basic_moves.hpp"
#include "draw.hpp"
#include "exponential.hpp"
#include "gamma.hpp"
#include "logprob.hpp"
#include "mcmc_utils.hpp"
#include "poisson.hpp"
#include "suffstat_utils.hpp"
#include "view.hpp"

struct alpha {};
struct mu {};
struct lambda {};
struct lambda_ss {};
struct K {};

auto poisson_gamma(size_t size) {
    auto alpha_ = make_backuped_node<exponential>(1.0);
    auto mu_ = make_backuped_node<exponential>(1.0);
    auto lambda_ = make_node_array<gamma_ss>(size, n_to_one(alpha_), n_to_one(mu_));
    auto K_ = make_node_array<poisson>(size, n_to_n(lambda_));

    return make_model(node<alpha>(alpha_), node<mu>(mu_), node<lambda>(lambda_), node<K>(K_));
}

template <class Node, class MB, class Gen>
void scaling_move(Node& node, MB blanket, Gen& gen) {
    backup(node);
    double logprob_before = logprob(blanket);
    double log_hastings = scale(get_raw_value(node), gen);
    bool accept = decide(logprob(blanket) - logprob_before + log_hastings, gen);
    if (!accept) { restore(node); }
}

int main() {
    auto gen = make_generator();

    auto m = poisson_gamma(10);
    auto v = make_view<alpha, mu, lambda>(m);

    draw(v, gen);
    clamp_array(get<K>(m), 1, 2, 3, 1, 2, 1, 2, 1, 2, 1);

    for (int it = 0; it < 10000; it++) {
        scaling_move(get<alpha>(m), make_view<alpha, lambda>(m), gen);
        scaling_move(get<mu>(m), make_view<mu, lambda>(m), gen);
    }
}