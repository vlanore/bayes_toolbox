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

#include "basic_moves.hpp"
#include "distributions/exponential.hpp"
#include "distributions/gamma.hpp"
#include "distributions/poisson.hpp"
#include "operations/backup.hpp"
#include "operations/logprob.hpp"
#include "operations/raw_value.hpp"
#include "structure/array_utils.hpp"
#include "structure/type_tag.hpp"
#include "structure/view.hpp"
#include "suffstat_utils.hpp"
#include "tagged_tuple/src/fancy_syntax.hpp"
using namespace std;

#define NB_POINTS 10000
#define PRECISION 0.025

template <class T>
void check_mean(T& target, function<void()> draw, double expected_mean,
                double precision_fact = 1.0) {
    T sum = 0;
    for (int i = 0; i < NB_POINTS; i++) {
        draw();
        sum += target;
    }
    CHECK(double(sum) / NB_POINTS ==
          doctest::Approx(expected_mean).epsilon(precision_fact * PRECISION));
}

TEST_CASE("Check that dist types have correct size") {
    size_t ds = sizeof(double);
    size_t es = sizeof(exponential::T);
    CHECK(ds == es);
}

TEST_CASE("Param making") {
    spos_real x = {1.};
    auto params = make_params<gamma_ss>(2, x);
    x = 3;

    // order in tuple is important for unpacking in calls
    CHECK(std::get<0>(params.data)() == 2);
    CHECK(std::get<1>(params.data)() == 3);
    CHECK(get<shape>(params)() == 2);
    CHECK(get<struct scale>(params)() == 3);
}

TEST_CASE("Datatype conversions") {
    spos_real spr = 1;
    pos_real pr = spr;
    real r = spr;
    real r2 = pr;
    CHECK(pr == 1);
    CHECK(r == 1);
    CHECK(r2 == 1);
}

// TEST_CASE("Node construction") {
//     auto gen = make_generator();
//     SUBCASE("exponential") {
//         auto alpha = make_node<exponential>(4);
//         check_mean(get<value>(alpha).value, [&]() { draw(alpha, gen); }, 0.25, 2.0);
//     }
//     SUBCASE("gamma") {
//         auto alpha = make_node<gamma_ss>(2, 3);
//         check_mean(get<value>(alpha).value, [&]() { draw(alpha, gen); }, 6.0, 2.0);
//     }
//     SUBCASE("poisson") {
//         auto alpha = make_node<poisson>(3);
//         check_mean(get<value>(alpha).value, [&]() { draw(alpha, gen); }, 3.0, 2.0);
//     }
//     SUBCASE("exponential with ref") {
//         double my_param = 17;
//         auto alpha = make_node<exponential>(my_param);
//         my_param = 4;
//         check_mean(get<value>(alpha).value, [&]() { draw(alpha, gen); }, 0.25, 2.0);
//     }
// }

// TEST_CASE("auto detection of nodes in make_param") {
//     auto k = make_node<exponential>(0.5);
//     get<value>(k).value = 17;
//     auto k2 = make_params<exponential>(k);
//     get<value>(k).value = 7;
//     CHECK(get<rate>(k2)() == 7);  // check it's by reference
// }

// TEST_CASE("Poisson/gamma simple model: draw values") {
//     auto gen = make_generator();

//     auto k = make_node<exponential>(0.5);
//     auto theta = make_node<exponential>(0.5);
//     auto lambda = make_node<gamma_ss>(k, theta);
//     auto counts = make_node<poisson>(lambda);

//     check_mean(get<value>(counts).value,
//                [&]() {
//                    draw(k, gen);
//                    draw(theta, gen);
//                    draw(lambda, gen);
//                    draw(counts, gen);
//                },
//                4, 3.0);
// }

// TEST_CASE("Make array params") {
//     auto p = make_array_params<gamma_ss>(n_to_constant(1.0), n_to_constant(2.0));
//     CHECK(get<shape>(p)(2) == 1.0);
//     CHECK(get<struct scale>(p)(17) == 2.0);
// }

// TEST_CASE("make array") {
//     auto a = make_node_array<exponential>(12, n_to_constant(1.0));
//     auto b = make_node_array<exponential>(12, n_to_constant(1.0));
//     auto c = make_node_array<gamma_ss>(12, n_to_n(a), n_to_n(b));
//     CHECK(get<value>(a).size() == 12);
//     CHECK(get<params, rate>(a)(10) == 1.0);
//     raw_value(a, 2) = 17.0;
//     raw_value(b, 2) = 19.0;
//     CHECK(get<params, shape>(c)(2) == 17.0);
//     CHECK(get<params, struct scale>(c)(2) == 19.0);
// }

// TEST_CASE("Draw in array") {
//     auto gen = make_generator();
//     auto a = make_node_array<exponential>(12, n_to_constant(1.0));
//     draw(a, gen);
// }

// TEST_CASE("Logprobs") {
//     auto n1 = make_node<poisson>(2.0);
//     auto n2 = make_node<poisson>(2.0);
//     auto n3 = make_node<poisson>(2.0);
//     raw_value(n1) = 1;
//     raw_value(n2) = 2;
//     raw_value(n3) = 3;
//     auto a = make_node_array<poisson>(3, n_to_constant(2.0));
//     set_value(a, {1, 2, 3});
//     CHECK(logprob(n1) == logprob(a, 0));
//     CHECK(logprob(n2) == logprob(a, 1));
//     CHECK(logprob(n3) == logprob(a, 2));
//     CHECK(logprob(a) == logprob(n1) + logprob(n2) + logprob(n3));
// }

// // TEST_CASE("MCMC with nodes") {
// //     auto gen = make_generator();

// //     auto param = make_node<exponential>(1);
// //     draw(param, gen);
// //     auto array = make_node_array<poisson>(20, n_to_one(param));
// //     set_value(array, 2, 2, 2, 1, 2, 1, 2, 3, 2, 3, 2, 2, 2, 1, 2, 1, 2, 3, 2, 3);

// //     vector<double> trace;
// //     for (int i = 0; i < 10000; i++) {
// //         for (int rep = 0; rep < 10; rep++) {
// //             auto param_backup = make_value_backup(param);
// //             double logprob_before = logprob(param) + logprob(array);
// //             double log_hastings = scale(get<value>(param).value, gen);
// //             double logprob_after = logprob(param) + logprob(array);
// //             bool accept = decide(logprob_after - logprob_before + log_hastings, gen);
// //             if (!accept) { restore_from_backup(param, param_backup); }
// //         }
// //         trace.push_back(get<value>(param).value);
// //     }
// //     double mean_trace = mean(trace);
// //     CHECK(1.9 < mean_trace);  // should be somewhere close to 2.0 but biaised down due to
// prior
// //     CHECK(mean_trace < 2);
// // }

// TEST_CASE("Sum and mean functions") {
//     vector<double> vd = {1, 2, 3, 4.2, 5.1, 6};
//     vector<int> vi = {1, 2, 3, 4, 5};

//     CHECK(sum(vd) == doctest::Approx(21.3));
//     CHECK(sum(vi) == 15);
//     CHECK(mean(vi) == doctest::Approx(3));
// }

// struct n1 {};
// struct n2 {};
// struct n3 {};

// template <class Arg>
// auto my_model(Arg&& arg) {
//     auto a = make_node<exponential>(1.0);
//     auto b = make_node<gamma_ss>(a, std::forward<Arg>(arg));
//     return make_model(node<n1>(a), node<n2>(b));
// }

// TEST_CASE("Basic model test") {
//     auto gen = make_generator();

//     auto a = make_node<exponential>(1.0);
//     auto m = my_model(a);

//     check_mean(get<n2, value>(m).value,
//                [&]() {
//                    draw(a, gen);
//                    draw(get<n1>(m), gen);
//                    draw(get<n2>(m), gen);
//                },
//                1, 2.0);
// }

// TEST_CASE("Forall on views") {
//     struct n1 {};
//     struct n2 {};
//     struct n3 {};
//     auto f = [](auto& x, NoIndex) { x += x; };
//     auto m = make_model(value_field<n1>(5), value_field<n2, std::string>("ab"));
//     auto v = make_view<n1, n2>(m);
//     forall_in_view(v, f);
//     forall_in_view(v, f);
//     CHECK(get<n1>(m) == 20);
//     CHECK(get<n2>(m) == "abababab");
// }

// TEST_CASE("Basic view test") {
//     auto gen = make_generator();
//     auto a = make_node<exponential>(1.0);
//     auto m = my_model(a);
//     auto v = make_view<n1, n2>(m);
//     CHECK(is_view<decltype(v)>::value);
//     CHECK(not is_view<decltype(m)>::value);
//     check_mean(get<n2, value>(m).value,
//                [&]() {
//                    draw(a, gen);
//                    draw(v, gen);
//                },
//                1, 2.0);
// }

// TOKEN(tok1);

// TEST_CASE("Views with indices") {
//     // auto gen = make_generator();
//     auto a = make_node_array<exponential>(5, n_to_constant(2.0));
//     auto m = make_model(tok1_ = move(a));
//     set_value(get<tok1>(m), {0, 0, 0, 0, 0});
//     // auto v = make_view(make_ref<tok1>(m, ArrayIndex{2}));
//     // draw(v, gen);

//     // TODO !
// }

// // TEST_CASE("node backups") {
// //     auto node = make_backuped_node<exponential>(1);
// //     auto array = make_backuped_node_array<poisson>(5, n_to_one(node));
// //     auto& a_3 = raw_value(array, 3);
// //     get_raw_value(node) = 1.3;
// //     set_value(array, 2, 4, 5, 8, 9);
// //     backup(node);
// //     backup(array);
// //     CHECK(a_3 == 8);

// //     get_raw_value(node) = 3.1;
// //     set_value(array, 8, 9, 0, 12, 3);
// //     CHECK(get_raw_value(node) == 3.1);
// //     CHECK(raw_value(array, 0) == 8);
// //     CHECK(raw_value(array, 1) == 9);
// //     CHECK(raw_value(array, 2) == 0);
// //     CHECK(raw_value(array, 3) == 12);
// //     CHECK(raw_value(array, 4) == 3);
// //     CHECK(a_3 == 12);

// //     restore(node);
// //     restore(array);
// //     CHECK(get_raw_value(node) == 1.3);
// //     CHECK(raw_value(array, 0) == 2);
// //     CHECK(raw_value(array, 1) == 4);
// //     CHECK(raw_value(array, 2) == 5);
// //     CHECK(raw_value(array, 3) == 8);
// //     CHECK(raw_value(array, 4) == 9);
// //     CHECK(a_3 == 8);
// // }

// // TEST_CASE("view backups") {
// //     auto m = []() {
// //         auto a = make_backuped_node<exponential>(1);
// //         auto b = make_backuped_node<exponential>(1);
// //         auto c = make_backuped_node<exponential>(1);
// //         return make_model(node<n1>(a), node<n2>(b), node<n3>(c));
// //     }();
// //     get_raw_value(get<n1>(m)) = 1.;
// //     get_raw_value(get<n2>(m)) = 2.;
// //     get_raw_value(get<n3>(m)) = 3.;
// //     auto v = make_view<n1, n3>(m);
// //     backup(v);
// //     get_raw_value(get<n1>(m)) = 4;
// //     get_raw_value(get<n2>(m)) = 5;
// //     get_raw_value(get<n3>(m)) = 6;
// //     restore(v);
// //     CHECK(get_raw_value(get<n1>(m)) == 1);
// //     CHECK(get_raw_value(get<n2>(m)) == 5);
// //     CHECK(get_raw_value(get<n3>(m)) == 3);
// // }

// // TEST_CASE("MCMC with views and backups") {
// //     auto gen = make_generator();

// //     auto param = make_backuped_node<exponential>(1);
// //     draw(param, gen);
// //     auto array = make_node_array<poisson>(20, n_to_one(param));
// //     set_value(array, 2, 2, 2, 1, 2, 1, 2, 3, 2, 3, 2, 2, 2, 1, 2, 1, 2, 3, 2, 3);

// //     auto m = make_model(node<n1>(param), node<n2>(array));
// //     auto v = make_view<n1, n2>(m);

// //     vector<double> trace;
// //     for (int i = 0; i < 10000; i++) {
// //         for (int rep = 0; rep < 10; rep++) {
// //             backup(get<n1>(m));
// //             double logprob_before = logprob(v);
// //             double log_hastings = scale(get<n1, value>(m).value, gen);
// //             bool accept = decide(logprob(v) - logprob_before + log_hastings, gen);
// //             if (!accept) { restore(get<n1>(m)); }
// //         }
// //         trace.push_back(get<n1, value>(m).value);
// //     }
// //     double mean_trace = mean(trace);
// //     CHECK(1.9 < mean_trace);  // should be somewhere close to 2.0 but biaised down due to
// prior
// //     CHECK(mean_trace < 2);
// // }

// // TEST_CASE("Suffstats") {
// //     auto array = make_node_array<poisson>(5, n_to_constant(1.0));
// //     set_value(array, 1, 2, 3, 4, 5);
// //     auto ss = make_suffstat<poisson_suffstat>(array);
// //     CHECK(!is_up_to_date(ss));

// //     gather(ss);
// //     CHECK(get<suffstat>(ss).sum == 15);
// //     CHECK(get<suffstat>(ss).N == 5);
// //     CHECK(is_up_to_date(ss));

// //     set_value(array, 1, 1, 1, 1, 1);
// //     CHECK(!is_up_to_date(ss));
// //     gather(ss);
// //     CHECK(get<suffstat>(ss).sum == 5);
// //     CHECK(is_up_to_date(ss));

// //     CHECK(get<params, rate>(ss)(0) == 1.0);
// //     CHECK(logprob(ss) == logprob(array));
// // }

// TEST_CASE("type_tag") {
//     auto a = make_node<exponential>(1);
//     auto ss = make_suffstat<gamma_ss_suffstats>(a);
//     auto m = make_model(tok1_ = move(a));
//     struct {
//         int a;
//     } s;
//     auto v = make_view<tok1>(m);

//     auto t1 = type_tag(a);
//     auto t2 = type_tag(m);
//     auto t3 = type_tag(s);
//     auto t4 = type_tag(v);
//     auto t5 = type_tag(make_view<tok1>(m));
//     auto t6 = type_tag(ss);

//     CHECK(std::is_same<decltype(t1), lone_node_tag>::value);
//     CHECK(std::is_same<decltype(t2), model_tag>::value);
//     CHECK(std::is_same<decltype(t3), unknown_tag>::value);
//     CHECK(std::is_same<decltype(t4), view_tag>::value);
//     CHECK(std::is_same<decltype(t5), view_tag>::value);
//     CHECK(std::is_same<decltype(t6), suffstat_tag>::value);
// }

// TEST_CASE("raw_value") {
//     auto n = make_node<poisson>(1);
//     get<value>(n).value = 2;
//     CHECK(raw_value(n) == 2);

//     auto a = make_node_array<poisson>(5, n_to_constant(2.0));
//     set_value(a, {1, 2, 3, 4, 5});
//     CHECK(raw_value(a, 3) == 4);
//     raw_value(a, 3) = 5;
//     CHECK(raw_value(a, 3) == 5);

//     const auto& cn = n;
//     const auto& ca = a;
//     CHECK(raw_value(ca, 3) == 5);
//     CHECK(raw_value(cn) == 2);
// }

// TEST_CASE("Matrix basic tests") {
//     auto m = make_node_matrix<poisson>(2, 2, [](int, int) { return 1.0; });
//     // @todo: make set_value/array a polymorphic set_value function
//     set_value(m, {{0, 1}, {2, 3}});
//     CHECK(raw_value(m, 0, 0) == 0);
//     CHECK(raw_value(m, 0, 1) == 1);
//     CHECK(raw_value(m, 1, 0) == 2);
//     CHECK(raw_value(m, 1, 1) == 3);

//     auto& x = raw_value(m, 0, 0);
//     auto& y = raw_value(m, 1, 1);
//     set_value(m, 1, {7, 8});
//     CHECK(x == 0);
//     CHECK(y == 8);
//     CHECK(raw_value(m, 1, 0) == 7);
//     CHECK(raw_value(m, 1, 1) == 8);
// }

// TEST_CASE("Backup/restore") {
//     auto n = make_node<poisson>(1.0);
//     raw_value(n) = 17;
//     auto bn = backup(n);
//     raw_value(n) = 11;
//     CHECK(raw_value(n) == 11);
//     restore(n, bn);
//     CHECK(raw_value(n) == 17);

//     auto a = make_node_array<poisson>(3, n_to_constant(1.0));
//     set_value(a, {11, 12, 13});
//     auto ba = backup(a);
//     auto ba1 = backup(a, 1);
//     set_value(a, {1, 2, 3});
//     CHECK(raw_value(a, 0) == 1);
//     CHECK(raw_value(a, 1) == 2);
//     CHECK(raw_value(a, 2) == 3);
//     restore(a, ba1, 1);
//     CHECK(raw_value(a, 0) == 1);
//     CHECK(raw_value(a, 1) == 12);
//     CHECK(raw_value(a, 2) == 3);
//     restore(a, ba);
//     CHECK(raw_value(a, 0) == 11);
//     CHECK(raw_value(a, 1) == 12);
//     CHECK(raw_value(a, 2) == 13);

//     // @todo: write tests for matrix cases
// }