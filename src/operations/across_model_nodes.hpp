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

#include "structure/new_view.hpp"
#include "structure/node.hpp"

template <class T, class F>
void across_model_nodes(T& x, F&& f);  // forward decl

namespace overloads {
    template <class Node, class F>
    void across_model_nodes(node_tag, Node& n, F f) {
        f(n);
    }

    template <class... SubsetArgs, class F>
    void across_model_nodes(unknown_tag, NodeSubset<SubsetArgs...>& subset, F f) {
        f(subset);
    }

    template <class... CollecArgs, class F>
    void across_model_nodes(unknown_tag, SetCollection<CollecArgs...>& colec, F f) {
        colec.across_elements([f](auto& x) { ::across_model_nodes(x, f); });
    }

}  // namespace overloads

template <class T, class F>
void across_model_nodes(T& x, F&& f) {
    overloads::across_model_nodes(type_tag(x), x, std::forward<F>(f));
}
