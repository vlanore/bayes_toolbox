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
#include "moves/proposals.hpp"

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
