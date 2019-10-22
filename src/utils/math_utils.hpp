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

#include <vector>
#include "tagged_tuple/src/tagged_tuple.hpp"

auto sum(std::vector<double> v) {
    double result = 0;
    for (auto e : v) { result += e; }
    return result;
}

auto sum(std::vector<int> v) {
    int result = 0;
    for (auto e : v) { result += e; }
    return result;
}

template <class Value>
auto sum(std::vector<Value> v) {
    typename Value::distrib::raw_type result = 0;
    for (auto e : v) { result += e; }
    return result;
}

template <class Value>
auto sum_log(std::vector<Value> v) {
    typename Value::distrib::raw_type result = 0;
    for (auto e : v) { result += log(e); }
    return result;
}

template <class Value>
auto mean(std::vector<Value> v) {
    return sum(v) / v.size();
}

struct constants {
    static constexpr double pi() { return 3.14159265358979323846; }
};