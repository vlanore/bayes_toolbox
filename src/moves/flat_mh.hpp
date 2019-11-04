
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
#include "operations/flat_backup.hpp"
#include "moves/proposals.hpp"

struct mh_overloads {

    struct LoneNoUpdate {
        void operator()() {}
    };

    struct ArrayNoUpdate {
        void operator()(int) {}
    };

    template <class Node, class LogProb, class Proposal, class Gen, class Update = LoneNoUpdate>
    static void mh_move(lone_node_tag, Node& node, LogProb lp, Proposal P, Gen& gen, Update update = {}) {
        auto bkp = flat_backup(node);
        double logprob_before = flat_logprob(node) + lp();
        double log_hastings = P(get<value>(node), gen);
        update();
        double logprob_after = flat_logprob(node) + lp();
        bool accept = decide(logprob_after - logprob_before + log_hastings, gen);
        if (!accept) {
            flat_restore(node, bkp);
            update();
        }
    }

    template <class Node, class LogProb, class Proposal, class Gen, class Update = ArrayNoUpdate>
    static void mh_move(node_array_tag, Node& node, LogProb lp, Proposal P, Gen& gen, Update update = {})   {
        for (size_t i=0; i<get<value>(node).size(); i++)    {
            auto bkp = flat_backup(node,i);
            double logprob_before = flat_logprob(node,i) + lp(i);
            double log_hastings = P(get<value>(node)[i], gen);
            update(i);
            double logprob_after = flat_logprob(node,i) + lp(i);
            bool accept = decide(logprob_after - logprob_before + log_hastings, gen);
            if (!accept) {
                flat_restore(node, bkp, i);
                update(i);
            }
        }
    }
};

template <class Node, class LogProb, class Proposal, class Gen, class... Update>
void flat_mh_move(Node& node, LogProb lp, Proposal P, Gen& gen, Update... update) {
    mh_overloads::mh_move(type_tag(node), node, lp, P, gen, update...);
}

template <class Node, class LogProb, class Gen, class... Update>
void flat_scaling_move(Node& node, LogProb lp, Gen& gen, Update... update) {
    flat_mh_move(node, lp, [](auto& value, auto& gen) { return scale(value, gen); }, gen, update...);
}

