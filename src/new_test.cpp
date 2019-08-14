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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "distributions/gamma.hpp"
#include "distributions/poisson.hpp"
#include "operations/raw_value.hpp"
#include "structure/array_utils.hpp"
#include "structure/new_view.hpp"
#include "structure/node.hpp"
using namespace std;

TEST_CASE("Iterating over nodes") {
    auto f = [](auto& rv) { cout << rv << "\n"; };

    auto a = make_node<poisson>(1.0);
    auto b = make_node_array<gamma_ss>(3, n_to_constant(1.0), n_to_constant(1.0));

    auto a_it = get_apply_single(a);
    auto b_it = get_apply_array(b);

    set_value(a, 3);
    set_value(b, {7.1, 8.2, 9.3});

    a_it(f);
    b_it(f);

    auto all_it = node_collection(a, b);

    // CHECK(is_node_iterator<decltype(all_it)>::value);
    // CHECK(!is_node_iterator<decltype(a_it)>::value);

    // using t = decltype(std::declval<decltype(a_it)>()(helper::node_tester{}));

    // auto g = [](auto f) {
    //     int i = 2;
    //     f(i);
    // };
    // CHECK(!is_node_iterator<decltype(g)>::value);

    
    // auto bad_it = make_node_view(g);

    // auto rev_apply = [f](auto& x) { x(f); };

    // all_it(rev_apply);
}