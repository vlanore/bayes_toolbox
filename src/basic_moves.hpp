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

#include "mcmc_utils.hpp"
#include "operations/backup.hpp"
#include "operations/draw.hpp"

template <typename Gen>
double scale(double& value, Gen& gen, double tuning = 1.0) {
    auto multiplier = tuning * (draw_uniform(gen) - 0.5);
    value *= exp(multiplier);
    return multiplier;
}

template <class Gen>
double slide(double& value, Gen& gen, double tuning = 1.0) {
    auto slide_amount = tuning * (draw_uniform(gen) - 0.5);
    value += slide_amount;
    return 0.;
}

template <class Gen>
double slide_constrained(double& value, double min, double max, Gen& gen, double tuning = 1.0) {
    assert(value >= min && value <= max);
    slide(value, gen, tuning);
    while (value < min || value > max) {
        if (value < min) { value = 2 * min - value; }
        if (value > max) { value = 2 * max - value; }
    }
    return 0.;
}

template <class Gen>
double profile_move(std::vector<double>& vec, double tuning, Gen& gen) {
    size_t n = vec.size();
    assert(n > 1);

    // draw two distinct indices in the vector
    size_t i1 = std::uniform_int_distribution<size_t>(0, n - 1)(gen);
    size_t i2 = std::uniform_int_distribution<size_t>(0, n - 2)(gen);
    if (i1 == i2) { i2 += 1; }  // avoid collision

    // slide move v1 and compensate to keep v1+v2 sum constant
    double &v1{vec[i1]}, &v2{vec[i2]};
    assert(v1 >= 0 && v2 >= 0);
    double v1_before = v1;
    slide_constrained(v1, 0, v1 + v2, gen, tuning);
    v2 += v1_before - v1;  // compensation

    return 0.;  // sliding move
}

struct NoUpdate {
    void operator()() {}
};

/**
 * Generic Metropolis-Hastings move function
 * Proposal distribution should be a lambda function that returns Hastings log ratio
 */
template <class Node, class LogProb, class Proposal, class Gen, class Update = NoUpdate>
void mh_move(Node& node, LogProb lp, Proposal P, Gen& gen, Update update = {}) {
    auto bkp = backup(node);
    double logprob_before = lp();
    double log_hastings = P(get<value>(node), gen);
    update();
    bool accept = decide(lp() - logprob_before + log_hastings, gen);
    if (!accept) {
        restore(node, bkp);
        update();
    }
}

template <class Node, class LogProb, class Gen>
void scaling_move(Node& node, LogProb lp, Gen& gen) {
    mh_move(node, lp, [](auto& value, auto& gen) { return scale(value, gen); }, gen);
}

template <class Node, class LogProb, class Gen>
void slide_constrained_move(Node& node, LogProb lp, Gen& gen, double min, double max) {
    assert(raw_value(node) >= min && raw_value(node) <= max);
    mh_move(node, lp,
            [min, max](auto& value, auto& gen) { return slide_constrained(value, min, max, gen); },
            gen);
}

template <class MB>
auto logprob_of_blanket(MB blanket) {
    return [blanket]() { return logprob(blanket); };
}