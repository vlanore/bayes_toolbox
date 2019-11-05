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
#include "datatypes.hpp"
#include "params.hpp"

template <class Tag, class Distrib>
using dnode_metadata = metadata<type_list<dnode_tag, Tag>, type_map<property<distrib, Distrib>>>;

template <class Distrib, class... ParamArgs>
auto make_dnode(ParamArgs&&... args) {
    auto v = typename Distrib::T();
    auto params = make_params<Distrib>(std::forward<ParamArgs>(args)...);
    return make_tagged_tuple<dnode_metadata<lone_dnode_tag, Distrib>>(
        unique_ptr_field<struct value>(std::move(v)), value_field<struct params>(params));
}

template <class Distrib, class... ParamArgs>
auto make_dnode_array(size_t size, ParamArgs&&... args) {
    std::vector<typename Distrib::T> values(size);
    auto params = make_array_params<Distrib>(std::forward<ParamArgs>(args)...);
    return make_tagged_tuple<dnode_metadata<dnode_array_tag, Distrib>>(
        unique_ptr_field<struct value>(std::move(values)), value_field<struct params>(params));
}

template <class Distrib, class... ParamArgs>
auto make_dnode_matrix(size_t size_x, size_t size_y, ParamArgs&&... args) {
    // @todo: change to better data structure (instead of vector of vectors)
    matrix<typename Distrib::T> values(size_x, std::vector<typename Distrib::T>(size_y));
    auto params = make_matrix_params<Distrib>(std::forward<ParamArgs>(args)...);
    return make_tagged_tuple<dnode_metadata<dnode_matrix_tag, Distrib>>(
        unique_ptr_field<struct value>(std::move(values)), value_field<struct params>(params));
}

template <class Distrib, class... ParamArgs>
auto make_dnode_with_init(typename Distrib::T c, ParamArgs&&... args) {
    auto v = typename Distrib::T(c);
    auto params = make_params<Distrib>(std::forward<ParamArgs>(args)...);
    return make_tagged_tuple<dnode_metadata<lone_dnode_tag, Distrib>>(
        unique_ptr_field<struct value>(std::move(v)), value_field<struct params>(params));
}

template <class Distrib, class... ParamArgs>
auto make_dnode_array_with_init(size_t size, typename Distrib::T c, ParamArgs&&... args) {
    std::vector<typename Distrib::T> values(size, c);
    auto params = make_array_params<Distrib>(std::forward<ParamArgs>(args)...);
    return make_tagged_tuple<dnode_metadata<dnode_array_tag, Distrib>>(
        unique_ptr_field<struct value>(std::move(values)), value_field<struct params>(params));
}

template <class Distrib, class... ParamArgs>
auto make_dnode_matrix_with_init(size_t size_x, size_t size_y, typename Distrib::T c, ParamArgs&&... args) {
    // @todo: change to better data structure (instead of vector of vectors)
    matrix<typename Distrib::T> values(size_x, std::vector<typename Distrib::T>(size_y, c));
    auto params = make_matrix_params<Distrib>(std::forward<ParamArgs>(args)...);
    return make_tagged_tuple<dnode_metadata<dnode_matrix_tag, Distrib>>(
        unique_ptr_field<struct value>(std::move(values)), value_field<struct params>(params));
}
