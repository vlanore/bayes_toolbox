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

#include "operations/draw.hpp"

template <typename Gen>
double draw_uniform(Gen& gen) {  // @todo: move elsewhere
    std::uniform_real_distribution<double> distrib(0, 1);
    return distrib(gen);
}

template <class ProbNode>
auto make_value_backup(ProbNode& node) {
    return get<value>(node);
}

template <class ProbNode, class Backup>
void restore_from_backup(ProbNode& node, const Backup& backup) {
    get<value>(node) = backup;
}

template <typename Gen>
bool decide(double logprob, Gen& gen) {
    return draw_uniform(gen) < exp(logprob);
}

/*
template <class MB>
auto logprob_of_blanket(MB blanket) {
    return [blanket]() mutable { return logprob(blanket); };
}
*/

template <class Node>
auto simple_logprob(Node& node) {
    return [&node] () {return logprob(node);};
}

template <class Node>
auto array_element_logprob(Node& node)  {
    return [&node] (int i) {
        auto subset = subsets::element(node,i);
        return logprob(subset);
    };
}

template <class Node>
auto matrix_row_logprob(Node& node)  {
    return [&node] (int i) {
        auto subset = subsets::row(node,i);
        return logprob(subset);
    };
}

template <class Node>
auto matrix_column_logprob(Node& node)  {
    return [&node] (int i) {
        auto subset = subsets::column(node,i);
        return logprob(subset);
    };
}

template <class Node>
auto matrix_element_logprob(Node& node)  {
    return [&node] (int i, int j) {
        auto subset = subsets::element(node,i,j);
        return logprob(subset);
    };
}

template <class Node>
auto simple_gather(Node& node) {
    return [&node] () {return gather(node);};
}

template <class Node>
auto array_element_gather(Node& node)  {
    return [&node] (int i) {
        auto subset = subsets::element(node,i);
        gather(subset);
    };
}

template <class Node>
auto matrix_row_gather(Node& node)  {
    return [&node] (int i) {
        auto subset = subsets::row(node,i);
        gather(subset);
    };
}

template <class Node>
auto matrix_column_gather(Node& node)  {
    return [&node] (int i) {
        auto subset = subsets::column(node,i);
        gather(subset);
    };
}

template <class Node>
auto matrix_element_gather(Node& node)  {
    return [&node] (int i, int j) {
        auto subset = subsets::element(node,i,j);
        gather(subset);
    };
}

template <class Var, class SS>
auto suffstat_logprob(Var& var, Proxy<SS&>& ss)   {
    return [&var, &ss] () {return ss.get().GetLogProb(get<value>(var));};
}

template <class Var1, class Var2, class SS>
auto suffstat_logprob(Var1& var1, Var2& var2, Proxy<SS&>& ss)   {
    return [&var1, &var2, &ss] () {return ss.get().GetLogProb(get<value>(var1), get<value>(var2));};
}

template <class Var, class SS>
auto suffstat_array_element_logprob(Var& var, Proxy<SS&, int>& ss)   {
    return [&var, &ss] (int i) {return ss.get().GetLogProb(raw_value(var,i));};
}

