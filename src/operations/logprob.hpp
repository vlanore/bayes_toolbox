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

#include "across_model_nodes.hpp"
#include "across_nodes.hpp"
#include "structure/visitor.hpp"

class LogProbTraitVisitor : public TraitVisitor<LogProbTraitVisitor, is_node, is_dnode> {
    using Parent = TraitVisitor<LogProbTraitVisitor, is_node, is_dnode>;
    friend Parent;

    double& total;

    template <class Node>
    void operator()(verifies<is_node>, Node& node) {
        across_nodes(node, [& total = this->total](auto distrib, auto& x, auto... params) {
            total += decltype(distrib)::logprob(x, params...);
        });
    }

    template <class Dnode>
    void operator()(verifies<is_dnode>, Dnode& dnode) {
        across_nodes(dnode, [](auto distrib, auto& x, auto... params) {
            decltype(distrib)::gather(x, params...);
        });
    }

  public:
    LogProbTraitVisitor(double& total) : total(total) {}
    using Parent::operator();
};

// use of visitor deactivated (subsets do not pass through)
template <class T>
double logprob(T& x) {
    double result = 0;
    auto logprob_node = [&result] (auto distrib, auto& x, auto... params) {
        result += decltype(distrib)::logprob(x, params...);
    };
    across_nodes(x, logprob_node);
    // across_model_nodes(x, LogProbTraitVisitor{result});
    return result;
}
