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

#include "distrib_draw.hpp"
#include "exponential.hpp"
#include "gamma.hpp"
#include "tagged_tuple/src/tagged_tuple.hpp"

struct alpha_ {};
struct beta_ {};
struct gamma_ {};

#define NB_POINTS 10000
#define PRECISION 0.025

template <class T>
void check_mean(T& target, std::function<void()> draw, double expected_mean,
                double precision_fact = 1.0) {
    T sum = 0;
    for (int i = 0; i < NB_POINTS; i++) {
        draw();
        sum += target;
    }
    CHECK(double(sum) / NB_POINTS ==
          doctest::Approx(expected_mean).epsilon(precision_fact * PRECISION));
}

template <class A>
auto make_my_model(A&& a) {
    auto b = make_node<exponential>(1);
    auto c = make_node<struct gamma>(std::forward<A>(a), b);
    return make_tagged_tuple(value_field<beta_>(std::move(b)), value_field<gamma_>(std::move(c)));
}

TEST_CASE("Model with unique_pointers") {
    auto gen = make_generator();

    auto a = make_node<exponential>(1);
    auto m = make_my_model(a);

    check_mean(get<gamma_, value, raw_value>(m),
               [&]() {
                   draw(a, gen);
                   draw(get<beta_>(m), gen);
                   draw(get<gamma_>(m), gen);
               },
               1);

    auto m2 = make_my_model(1);
    check_mean(get<gamma_, value, raw_value>(m2),
               [&]() {
                   draw(get<beta_>(m2), gen);
                   draw(get<gamma_>(m2), gen);
               },
               1);
}