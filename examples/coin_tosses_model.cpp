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
#include "bayes_toolbox.hpp"
using namespace std;

TOKEN(p)
TOKEN(bern)

// Model definition
auto bernoulli_model(size_t size) {
    auto p = make_node<uniform>(0., 1.);              // p ~ U(0,1)
    auto bern = make_node_array<bernoulli>(size, p);  // bern ~ Bern(p)
    return make_model(                                //
        p_ = move(p),                                 //
        bern_ = move(bern)                            //
    );
}

int main() {
    auto gen = make_generator();

    constexpr size_t nb_it{100'000};
    constexpr size_t burn_in{nb_it / 10};

    int n_obs = 2;
    auto m = bernoulli_model(n_obs);
    auto v = make_collection(p_(m), bern_(m));
    draw(v, gen);

    set_value(bern_(m), {true, true});

    double p_sum{0};

    for (size_t it = 0; it < nb_it; ++it) {
        // propose move for p, provided a Markov blanket of p
        slide_constrained_move(p_(m), simple_logprob(bern_(m)), 1.0, 0., 1., 1, gen);
        if (it >= burn_in) { p_sum += raw_value(p_(m)); }
    }
    float p_mean = p_sum / float(nb_it - burn_in);
    std::cout << "Coin Tosses" << std::endl;
    std::cout << "p = " << p_mean << std::endl;
    if (std::abs(p_mean - (n_obs + 1.) / (n_obs + 2.)) > 0.1) {
        return 1;
    } else {
        return 0;
    }
}
