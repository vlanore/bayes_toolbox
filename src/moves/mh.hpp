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
#include "moves/proposals.hpp"

struct NoUpdate {
    void operator()() {}
    template<class... Index>
    void operator()(Index...) {}
};

struct mh_overloads {

    template <class Node, class LogProb, class Proposal, class Gen, class Update = NoUpdate>
    static void mh_move(lone_node_tag, Node& node, LogProb lp, Proposal P, size_t nrep, Gen& gen, Update update = {}) {
        for (size_t rep=0; rep<nrep; rep++) {
            auto bkp = backup(node);
            double logprob_before = logprob(node) + lp();
            double log_hastings = P(get<value>(node), gen);
            update();
            double logprob_after = logprob(node) + lp();
            bool accept = decide(logprob_after - logprob_before + log_hastings, gen);
            if (!accept) {
                restore(node, bkp);
                update();
            }
        }
    }

    template <class Node, class LogProb, class Proposal, class Gen, class Update = NoUpdate>
    static void mh_move(node_array_tag, Node& node, LogProb lp, Proposal P, size_t nrep, Gen& gen, Update update = {})   {
        for (size_t rep=0; rep<nrep; rep++) {
            for (size_t i=0; i<get<value>(node).size(); i++)    {
                auto subset = subsets::element(node,i);
                auto bkp = backup(subset);
                double logprob_before = logprob(subset) + lp(i);
                double log_hastings = P(get<value>(node)[i], gen);
                update(i);
                double logprob_after = logprob(subset) + lp(i);
                bool accept = decide(logprob_after - logprob_before + log_hastings, gen);
                if (!accept) {
                    restore(subset, bkp);
                    update(i);
                }
            }
        }
    }
};

template <class Node, class LogProb, class Proposal, class Gen, class... Update>
void mh_move(Node& node, LogProb lp, Proposal P, size_t nrep, Gen& gen, Update... update) {
    mh_overloads::mh_move(type_tag(node), node, lp, P, nrep, gen, update...);
}

template <class Node, class LogProb, class Gen, class... Update>
void scaling_move(Node& node, LogProb lp, double tuning, size_t nrep, Gen& gen, Update... update) {
    mh_move(node, lp, [tuning](auto& value, auto& gen) { return scale(value, tuning, gen); }, nrep, gen, update...);
}

template <class Node, class LogProb, class Gen, class... Update>
void sliding_move(Node& node, LogProb lp, double tuning, size_t nrep, Gen& gen, Update... update) {
    mh_move(node, lp, [tuning](auto& value, auto& gen) { return slide(value, tuning, gen); }, nrep, gen, update...);
}

template <class Node, class LogProb, class Gen, class... Update>
void slide_constrained_move(Node& node, LogProb lp, double tuning, double min, double max, size_t nrep, Gen& gen, Update... update)  {
    assert(raw_value(node) >= min && raw_value(node) <= max);
    mh_move(node, lp,
            [min, max, tuning](auto& value, auto& gen) { return slide_constrained(value, tuning, min, max, gen); },
            nrep,
            gen,
            update...);
}
