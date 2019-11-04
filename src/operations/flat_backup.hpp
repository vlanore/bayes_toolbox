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

// #include "raw_value.hpp"
// #include "structure/introspection.hpp"

template <class T, class... Args>
auto flat_backup(T& x, Args... args);  // forward decl

namespace backup_overloads {
    template <class T>
    auto backup(node_tag, T& node) {
        return get<value>(node);
    }

    template <class T>
    auto backup(node_array_tag, T& a, int i)    {
        return get<value>(a)[i];
    }
}  // namespace overloads

template <class T, class... Args>
auto flat_backup(T& x, Args... args) {
    return backup_overloads::backup(type_tag(x), x, args...);
}

template <class T, class Backup, class... Args>
void flat_restore(T& x, Backup& b, Args... args);  // forward decl

namespace restore_overloads {

    template <class Node, class T = typename node_distrib_t<Node>::T>
    void restore(node_matrix_tag, Node& node, matrix<T>& backup) {
        assert(backup.size() == get<value>(node).size());
        assert(backup.size() > 0);
        assert(backup.at(0).size() == get<value>(node).at(0).size());
        for (size_t i = 0; i < backup.size(); i++) {
            get<value>(node)[i].assign(backup[i].begin(), backup[i].end());
        }
    }

    template <class Node, class T = typename node_distrib_t<Node>::T>
    void restore(node_array_tag, Node& node, std::vector<T>& backup) {
        assert(backup.size() == get<value>(node).size());
        get<value>(node).assign(backup.begin(), backup.end());
    }

    template <class Node, class T = typename node_distrib_t<Node>::T>
    void restore(node_array_tag, Node& a, T& backup, int i) {
        assert(i < get<value>(a).size());
        get<value>(a)[i] = backup;
    }

    template <class Node, class T = typename node_distrib_t<Node>::T>
    void restore(lone_node_tag, Node& node, T& backup) {
        get<value>(node) = backup;
    }

}  // namespace overloads

template <class T, class Backup, class... Args>
void flat_restore(T& x, Backup& b, Args... args) {
    restore_overloads::restore(type_tag(x), x, b, args...);
}
