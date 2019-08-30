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

#include <sstream>
#include "distributions/exponential.hpp"
#include "distributions/gamma.hpp"
#include "distributions/poisson.hpp"
#include "operations/draw.hpp"
#include "operations/element_vpv.hpp"
#include "operations/element_vv.hpp"
#include "operations/raw_value.hpp"
#include "operations/row_vpv.hpp"
#include "operations/row_vv.hpp"
#include "structure/ValueParamView.hpp"
#include "structure/array_utils.hpp"
#include "structure/new_view.hpp"
#include "structure/node.hpp"
using namespace std;

TEST_CASE("Basic ValueView test") {
    auto my_array = make_node_array<poisson>(3, n_to_constant(1.0));
    set_value(my_array, {3, 4, 5});

    auto my_custom_view = make_valueview([& v = get<value>(my_array)](auto f) {
        for (auto e : v) { f(e); }
    });

    int sum = 0;
    my_custom_view([&sum](int e) { sum += e; });
    CHECK(sum == 12);
}

TEST_CASE("Element_vv") {
    auto my_node = make_node<exponential>(1.0);
    auto my_array = make_node_array<poisson>(3, n_to_one(my_node));
    set_value(my_node, 1.2);
    set_value(my_array, {2, 3, 7});

    auto view1 = element_vv(my_node);
    auto view2 = element_vv(my_array, 1);

    std::stringstream ss;
    auto f = [&ss](auto&& x) { ss << x << ";"; };
    view1(f);
    view2(f);
    CHECK(ss.str() == "1.2;3;");
}

TEST_CASE("Row_vv") {
    auto my_matrix = make_node_matrix<poisson>(3, 2, [](int, int) { return 1.0; });
    set_value(my_matrix, {{4, 7}, {11, 3}, {5, 13}});

    auto my_view = row_vv(my_matrix, 1);

    int sum = 0;
    my_view([&sum](auto&& e) { sum += e; });
    CHECK(sum == 14);  // 11 + 3
}

TEST_CASE("Element_vpv") {
    auto my_node = make_node<gamma_ss>(1.0, 1.0);
    auto my_array = make_node_array<gamma_ss>(3, n_to_one(my_node), n_to_constant(1.4));
    set_value(my_node, 1.2);
    set_value(my_array, {2.2, 3.3, 7.7});

    auto view1 = element_vpv(my_node);
    auto view2 = element_vpv(my_array, 1);

    std::stringstream ss;
    auto f = [&ss](auto x, auto shape, auto scale) {
        ss << x << ":" << shape << "," << scale << ";";
    };
    view1(f);
    view2(f);
    CHECK(ss.str() == "1.2:1,1;3.3:1.2,1.4;");
}

TEST_CASE("Row_vpv") {
    auto my_matrix = make_node_matrix<gamma_ss>(3, 2, [](int i, int j) { return double(i + j); },
                                                [](int i, int j) { return double(i - j); });
    set_value(my_matrix, {{4.1, 2.5}, {7.2, 8.1}, {2.13, 9.3}});

    auto my_view = row_vpv(my_matrix, 1);

    std::stringstream ss;
    auto f = [&ss](auto x, auto shape, auto scale) {
        ss << x << ":" << shape << "," << scale << ";";
    };
    my_view(f);
    CHECK(ss.str() == "7.2:1,1;8.1:2,0;");
}

TEST_CASE("Basic ValueParamView test") {
    auto my_array = make_node_array<poisson>(3, n_to_constant(1.0));
    set_value(my_array, {3, 4, 5});

    auto my_custom_view =
        make_valueparamview([& v = get<value>(my_array), &p = get<params, rate>(my_array)](auto f) {
            for (size_t i = 0; i < v.size(); i++) { f(v[i], p(i)); }
        });

    std::stringstream ss;
    my_custom_view([&ss](int value, double param) { ss << "v:" << value << "(" << param << ");"; });
    CHECK(ss.str() == "v:3(1);v:4(1);v:5(1);");
}

TEST_CASE("is_iterator") {
    auto f = []() {};
    auto h = [](auto, auto) {};
    auto i = [](auto&&) {};

    CHECK(!is_itfunc<decltype(f)>::value);
    CHECK(!is_itfunc<decltype(h)>::value);
    CHECK(is_itfunc<decltype(i)>::value);
}

TEST_CASE("Iterating over nodes") {
    auto a = make_node<poisson>(1.0);
    auto b = make_node_array<gamma_ss>(3, n_to_constant(1.0), n_to_constant(1.0));
    set_value(a, -1);
    set_value(b, {-1, -1, -1});

    auto all_it = node_collection(a, b);

    auto gen = make_generator();
    auto f = [&gen](auto& node) { draw(node, gen); };
    all_it(f);

    CHECK(raw_value(a) != -1);
    CHECK(raw_value(b, 0) != -1);
    CHECK(raw_value(b, 1) != -1);
    CHECK(raw_value(b, 2) != -1);

    // auto g = [](int& i) { cout << i << endl; };
    // auto g = [](auto f) {
    //     int i = 2;
    //     f(i);
    // };
    // auto bad_it = make_node_view(g); // triggers static_asserts
}

// TEST_CASE("element/row itfuncs") {
//     auto a = make_node_array<poisson>(3, n_to_constant(1.0));
//     auto m = make_node_matrix<exponential>(3, 2, [](int, int) { return 1.0; });
//     set_value(a, {11, 12, 13});
//     set_value(m, {{11.21, 13.23}, {12.22, 23.23}, {13.23, 33.23}});

//     double sum = 0;
//     auto f = [&sum](auto& value) { sum += value; };

//     auto ea = element(a, 1);
//     auto em = element(m, 0, 1);
//     auto rm = row(m, 1);

//     ea(f);
//     em(f);
//     CHECK(sum == 12 + 13.23);
//     rm(f);
//     CHECK(sum == 12 + 13.23 + 12.22 + 23.23);
// }

// TEST_CASE("ith_element") {
//     auto a = make_node_array<poisson>(3, n_to_constant(1.0));
//     auto m = make_node_matrix<poisson>(2, 2, [](int, int) { return 1.0; });
//     set_value(a, {11, 12, 13});
//     set_value(m, {{2, 42}, {3, 43}});

//     auto v = ith_element(a);
//     auto v2 = jth_element(a);
//     auto v3 = ijth_element(m);

//     int sum = 0;
//     auto f = [&sum](auto& value) { sum += value; };

//     v(0)(f);       // 11
//     v(1)(f);       // 12
//     v(2)(f);       // 13
//     v2(17, 1)(f);  // 12
//     v3(1, 0)(f);   // 3
//     CHECK(sum == (11 + 12 + 13 + 12 + 3));
// }

// TEST_CASE("itfunc collections") {
//     auto e = make_node<poisson>(1.0);
//     auto a = make_node_array<exponential>(3, n_to_constant(1.0));
//     set_value(e, 1);
//     set_value(a, {1.2, 2.3, 3.4});

//     auto col = make_valueview_collection(e, element(a, 1));
//     auto col2 = make_valueview_collection_i(ith_element(a));

//     double sum = 0;
//     auto f = [&sum](auto& value) { sum += value; };

//     col(f);
//     col2(2)(f);
//     CHECK(sum == 1 + 2.3 + 3.4);
// }