/*Copyright or © or Copr. CNRS (2019). Contributors2
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

#include <assert.h>
#include "Proxy.hpp"
#include "operations/raw_value.hpp"
#include "operations/set_value.hpp"

//==================================================================================================
// array param helpers for common cases

// forward declarations

template <class T>
auto one_to_one(T& t);

template <class T>
auto n_to_one(T& t);

template <class T>
auto mn_to_one(T& t);

template <class T>
auto mn_to_m(T& t);

template <class T>
auto mn_to_n(T& t);

template <class T>
auto n_to_n(T& t);

namespace overloads {
    template <class Node>
    auto one_to_one(node_tag, Node& node) {
        return [&rv = raw_value(node)]() { return rv; };
    }

    template <class Node>
    auto n_to_one(node_tag, Node& node) {
        return [&rv = raw_value(node)](int) { return rv; };
    }

    template <class Node>
    auto mn_to_one(node_tag, Node& node) {
        return [&rv = raw_value(node)](int, int) { return rv; };
    }

    template <class Node>
    auto mn_to_m(node_tag, Node& node) {
        return [&rv = get<value>(node)](int i, int) { return rv[i]; };
    }

    template <class Node>
    auto mn_to_n(node_tag, Node& node) {
        return [&rv = get<value>(node)](int, int j) { return rv[j]; };
    }

    template <class Node>
    auto n_to_n(node_tag, Node& node) {
        return [&v = get<value>(node)](int i) { return v[i]; };
    }

    template <class Unknown>
    auto one_to_one(unknown_tag, Unknown& u) {
        return [&u]() -> const Unknown& { return u; };
    }

    template <class Unknown>
    auto n_to_one(unknown_tag, Unknown& u) {
        return [&u](int) -> const Unknown& { return u; };
    }

    template <class Unknown>
    auto mn_to_one(unknown_tag, Unknown& u) {
        return [&u](int, int) -> const Unknown& { return u; };
    }

    template <class Unknown>
    auto mn_to_m(unknown_tag, std::vector<Unknown>& u) {
        return [&u](int i, int) -> const Unknown& { return u[i]; };
    }

    template <class Unknown>
    auto mn_to_n(unknown_tag, std::vector<Unknown>& u) {
        return [&u](int, int j) -> const Unknown& { return u[j]; };
    }

    template <class Unknown>
    auto n_to_n(unknown_tag, std::vector<Unknown>& u) {
        return [&u](int i) -> const Unknown& { return u[i]; };
    }

    template <class Unknown>
    auto n_to_one(unknown_tag, Proxy<Unknown>& u) {
        return [&u](int) -> const Unknown& { return u.get(); };
    }

    template <class Unknown>
    auto mn_to_one(unknown_tag, Proxy<Unknown>& u) {
        return [&u](int, int) -> const Unknown& { return u.get(); };
    }

    template <class Unknown>
    auto mn_to_m(unknown_tag, Proxy<Unknown, int>& u) {
        return [&u](int i, int) -> const Unknown& { return u.get(i); };
    }

    template <class Unknown>
    auto mn_to_n(unknown_tag, Proxy<Unknown, int>& u) {
        return [&u](int, int j) -> const Unknown& { return u.get(j); };
    }

    template <class Unknown>
    auto n_to_n(unknown_tag, Proxy<Unknown, int>& u) {
        return [&u](int i) -> const Unknown& { return u.get(i); };
    }

}  // namespace overloads

template <class T>
auto one_to_one(T& t) {
    return overloads::one_to_one(type_tag(t), t);
}

template <class T>
auto n_to_one(T& t) {
    return overloads::n_to_one(type_tag(t), t);
}

template <class T>
auto mn_to_one(T& t) {
    return overloads::mn_to_one(type_tag(t), t);
}

template <class T>
auto mn_to_m(T& t) {
    return overloads::mn_to_m(type_tag(t), t);
}

template <class T>
auto mn_to_n(T& t) {
    return overloads::mn_to_n(type_tag(t), t);
}

template <class T>
auto n_to_n(T& t) {
    return overloads::n_to_n(type_tag(t), t);
}

template <class T>
auto one_to_const(const T& value) {
    return [value]() -> const T& { return value; };
}

template <class T>
auto n_to_const(const T& value) {
    return [value](int) -> const T& { return value; };
}

template <class T>
auto mn_to_const(const T& value) {
    return [value](int, int) -> const T& { return value; };
}
