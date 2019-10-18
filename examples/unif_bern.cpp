// p unif(0,1)
// b bern(p)

// fixer b à 1, et normalement 2/3

#include <iostream>
#include "basic_moves.hpp"
#include "distributions/bernoulli.hpp"
#include "distributions/uniform.hpp"
#include "mcmc_utils.hpp"
#include "operations/backup.hpp"
#include "operations/logprob.hpp"
#include "operations/raw_value.hpp"
using namespace std;

int main() {
    auto gen = make_generator();

    auto a = make_node<uniform>(0, 1);
    auto b = make_node<bernoulli>(a);

    get<value>(b) = 1;
    draw(a, gen);
    cout << "a=" << get<value>(a) << "\n";
    double sum_a = 0;

    for (int i = 0; i < 10'000; i++) {
        auto bkp = backup(a);
        double log_prob_before = logprob(a) + logprob(b);
        double log_hastings = slide_constrained(get<value>(a), 0, 1, gen);
        bool accept = decide(logprob(a) + logprob(b) - log_prob_before + log_hastings, gen);
        if (!accept) { restore(a, bkp); }
        sum_a += get<value>(a);
    }
    cout << "a=" << sum_a / 10'000. << "\n";
}
