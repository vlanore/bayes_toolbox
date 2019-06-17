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

#include "model.hpp"
#include "reference.hpp"

template <class... Refs>
struct view {
    static_assert(list_and<is_ref, type_list<Refs...>>::value,
                  "View template params should only be refs");

    view(tuple_construct, Refs&&... refs) : refs(std::forward<Refs>(refs)...) {}
    std::tuple<Refs...> refs;
    static constexpr size_t size() { return sizeof...(Refs); }
};

template <class T>
struct is_view : std::false_type {};

template <class... Refs>
struct is_view<view<Refs...>> : std::true_type {};

template <class... Refs>
auto make_view(Refs&&... refs) {
    return view<Refs...>(tuple_construct(), std::forward<Refs>(refs)...);
}

template <class... Tags, class Model>
auto make_view(Model& model) {  // @todo: maybe this version is not necessary (too specific)
    static_assert(is_model<Model>::value, "Expected a reference to a prob model");
    return make_view(make_ref<Tags>(model)...);
}

template <class View, class F, size_t... Is>
void forall_in_view_impl(View& view, const F& f, std::index_sequence<Is...>) {
    static_assert(is_view<View>::value, "Expected a reference to a view");
    std::vector<int> ignore = {(f(get<Is>(view.refs).node_ref, get<Is>(view.refs).index), 0)...};
}

template <class View, class F>
void forall_in_view(View& view, const F& f) {
    static_assert(is_view<View>::value, "Expected a reference to a view");
    forall_in_view_impl(view, f, std::make_index_sequence<View::size()>());
}