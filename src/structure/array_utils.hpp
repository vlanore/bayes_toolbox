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
#include <vector>
#include "Proxy.hpp"
#include "tagged_tuple/src/tagged_tuple.hpp"
#include "type_tag.hpp"

//==================================================================================================
// array param helpers for common cases

template <class T>
struct ret {};

// forward declarations
namespace overloads {
    template <class Node, class Return>
    auto one_to_one(node_tag, ret<Return>, Node& node) {
        return [&rv = raw_value(node)]() { return rv; };
    }

    template <class Node, class Return>
    auto n_to_one(node_tag, ret<Return>, Node& node) {
        return [&rv = raw_value(node)](int) { return rv; };
    }

    template <class Node, class Return>
    auto mn_to_one(node_tag, ret<Return>, Node& node) {
        return [&rv = raw_value(node)](int, int) { return rv; };
    }

    template <class Node, class Return>
    auto mn_to_m(node_tag, ret<Return>, Node& node) {
        return [&rv = get<value>(node)](int i, int) { return rv[i]; };
    }

    template <class Node, class Return>
    auto mn_to_n(node_tag, ret<Return>, Node& node) {
        return [&rv = get<value>(node)](int, int j) { return rv[j]; };
    }

    template <class Node, class Return>
    auto mnp_to_one(node_tag, ret<Return>, Node& node) {
        return [&rv = raw_value(node)](int, int, int) { return rv; };
    }

    /*
    template <class Node, class Return>
    auto mnp_to_m(node_tag, ret<Return>, Node& node) {
        return [&rv = raw_value(node)](int i, int, int) { return rv[i]; };
    }
    */

    template <class Node, class Return>
    auto n_to_n(node_tag, ret<Return>, Node& node) {
        return [&v = get<value>(node)](int i) { return v[i]; };
    }

    template <class Dnode, class Return>
    auto one_to_one(dnode_tag, ret<Return>, Dnode& dnode) {
        return [&rv = raw_value(dnode)]() { return rv; };
    }

    template <class Dnode, class Return>
    auto n_to_one(dnode_tag, ret<Return>, Dnode& dnode) {
        return [&rv = raw_value(dnode)](int) { return rv; };
    }

    template <class Dnode, class Return>
    auto mn_to_one(dnode_tag, ret<Return>, Dnode& dnode) {
        return [&rv = raw_value(dnode)](int, int) { return rv; };
    }

    template <class Dnode, class Return>
    auto mn_to_m(dnode_tag, ret<Return>, Dnode& dnode) {
        return [&rv = get<value>(dnode)](int i, int) { return rv[i]; };
    }

    template <class Dnode, class Return>
    auto mn_to_n(dnode_tag, ret<Return>, Dnode& dnode) {
        return [&rv = get<value>(dnode)](int, int j) { return rv[j]; };
    }

    template <class Dnode, class Return>
    auto n_to_n(dnode_tag, ret<Return>, Dnode& dnode) {
        return [&v = get<value>(dnode)](int i) { return v[i]; };
    }

    template <class Unknown, class Return>
    auto one_to_one(unknown_tag, ret<Return>, Unknown& u) {
        return [&u]() -> const Return& { return u; };
    }

    template <class Unknown, class Return>
    auto n_to_one(unknown_tag, ret<Return>, Unknown& u) {
        return [&u](int) -> const Return& { return u; };
    }

    template <class Unknown, class Return>
    auto mn_to_one(unknown_tag, ret<Return>, Unknown& u) {
        return [&u](int, int) -> const Return& { return u; };
    }

}  // namespace overloads

template <class Unknown, class Return = Unknown>
auto mn_to_m(std::vector<Unknown>& u) {
    return [&u](int i, int) -> const Return& { return u[i]; };
}

template <class Unknown, class Return = Unknown>
auto mn_to_n(std::vector<Unknown>& u) {
    return [&u](int, int j) -> const Return& { return u[j]; };
}

template <class Unknown, class Return = Unknown>
auto n_to_n(std::vector<Unknown>& u) {
    return [&u](int i) -> const Return& { return u[i]; };
}

template <class Unknown, class Return = Unknown>
auto n_to_one(Proxy<Unknown>& u) {
    return [&u](int) -> const Return& { return u.get(); };
}

template <class Unknown, class Return = Unknown>
auto mn_to_one(Proxy<Unknown>& u) {
    return [&u](int, int) -> const Return& { return u.get(); };
}

template <class Unknown, class Return = Unknown>
auto mn_to_m(Proxy<Unknown, int>& u) {
    return [&u](int i, int) -> const Return& { return u.get(i); };
}

template <class Unknown, class Return = Unknown>
auto mn_to_n(Proxy<Unknown, int>& u) {
    return [&u](int, int j) -> const Return& { return u.get(j); };
}

template <class Unknown, class Return = Unknown>
auto n_to_n(Proxy<Unknown, int>& u) {
    return [&u](int i) -> const Return& { return u.get(i); };
}

template <class T, class Return = T>
auto one_to_one(T& t) {
    return overloads::one_to_one(type_tag(t), ret<Return>{}, t);
}

template <class T, class Return = T>
auto n_to_one(T& t) {
    return overloads::n_to_one(type_tag(t), ret<Return>{}, t);
}

template <class T, class Return = T>
auto mn_to_one(T& t) {
    return overloads::mn_to_one(type_tag(t), ret<Return>{}, t);
}

template <class T, class Return = T>
auto mn_to_m(T& t) {
    return overloads::mn_to_m(type_tag(t), ret<Return>{}, t);
}

template <class T, class Return = T>
auto mn_to_n(T& t) {
    return overloads::mn_to_n(type_tag(t), ret<Return>{}, t);
}

template <class T, class Return = T>
auto n_to_n(T& t) {
    return overloads::n_to_n(type_tag(t), ret<Return>{}, t);
}

template <class T, class Return = T>
auto one_to_const(const T& value) {
    return [value]() -> const Return& { return value; };
}

template <class T, class Return = T>
auto n_to_const(const T& value) {
    return [value](int) -> const Return& { return value; };
}

template <class T, class Return = T>
auto mn_to_const(const T& value) {
    return [value](int, int) -> const Return& { return value; };
}

template<class Array, class Alloc>
auto n_to_mix(Array& array, Alloc& alloc)  {
    return [&array, &alloc] (int site) {return array[alloc[site]];};
}

template<class Array, class Alloc>
auto mn_to_mixn(Array& array, Alloc& alloc)  {
    return [&array, &alloc] (int branch, int site) {return array[alloc[site]];};
}


