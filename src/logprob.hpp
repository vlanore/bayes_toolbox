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

#include "distrib_utils.hpp"
#include "overloading.hpp"

/*==================================================================================================
~~ Overloads that unpack parameters ~~
==================================================================================================*/
template <typename Distrib, class T = typename Distrib::raw_value, typename Param,
          class... ParamKeys, class... Indexes>
auto logprob_helper(const T& value, const Param& param, std::tuple<ParamKeys...>,
                    Indexes... indexes) {
    return Distrib::logprob(value, get<ParamKeys>(param)(indexes...)...);
}

template <class Distrib, class T = typename Distrib::T, class Param>
double logprob(const std::vector<T>& value, const Param& param) {
    using keys = map_key_list_t<typename Distrib::param_decl>;
    double result = 0;
    for (size_t i = 0; i < value.size(); i++) {
        result += logprob_helper<Distrib>(value[i].value, param, keys(), i);
    }
    return result;
}

template <class Distrib, class Param>
double logprob(const typename Distrib::T& value, const Param& param) {
    using keys = map_key_list_t<typename Distrib::param_decl>;
    return logprob_helper<Distrib>(value.value, param, keys());
}

template <class SS, class Param>
double logprob(const SS& ss, const Param& param) {
    using distrib = typename SS::distrib;
    using keys = map_key_list_t<typename distrib::param_decl>;
    // TODO check that params are identical?
    return logprob_helper<SS, SS>(ss, param, keys(), 0);
}

/*==================================================================================================
~~ Generic version that unpacks probnode objects ~~
==================================================================================================*/
template <class SS>
double logprob_selector(SS& ss, suffstat_tag) {
    using ss_t = metadata_get_property<suffstat_type, metadata_t<SS>>;
    return logprob<ss_t>(get<suffstat>(ss), get<params>(ss));
}

template <class ProbNode>
double logprob_selector(ProbNode& node, node_tag) {
    using distrib = node_distrib_t<ProbNode>;
    return logprob<distrib>(get<value>(node), get<params>(node));
}

template <class View>
double logprob_selector(View& view, view_tag) {
    double result = 0;
    forall_in_view(view, [&result](auto& node) { result += logprob(node); });
    return result;
}

template <class Something>
double logprob(Something& x) {
    return logprob_selector(x, type_tag(x));
}