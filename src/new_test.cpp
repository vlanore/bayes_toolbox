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

#include <sstream>
#include "bayes_toolbox.hpp"
#include "doctest.h"
#include "structure/new_view.hpp"
#include "structure/visitor.hpp"
using namespace std;

TEST_CASE("Basic test for new views") {
    auto a = make_node_array<exponential>(5, n_to_const(1.0));
    set_value(a, {1, 2, 3, 4, 5});
    auto b = make_node_array<gamma_ss>(5, n_to_n(a), n_to_const(2.0));
    set_value(b, {6, 7, 8, 9, 10});

    auto third_and_fourth_element = [](auto& n, auto f) {
        apply(f, n, 3);
        apply(f, n, 4);
    };
    auto s = make_subset(b, third_and_fourth_element);
    auto sa = make_subset(a, [](auto& n, auto f) {
        for (size_t i = 0; i < get<value>(n).size(); i++) { apply(f, n, i); }
    });

    stringstream ss;
    auto add_to_stream = [&ss](auto& x) { ss << x << "; "; };
    s.across_values(add_to_stream);

    auto add_params_to_stream = [&ss](auto distrib, auto& value, auto... params) {
        std::vector<double> ps = {params...};
        ss << "<" << typeid(distrib).name() << ": " << value << ", ";
        for (auto e : ps) { ss << e << " "; }
        ss << "> ";
    };
    s.across_nodes(add_params_to_stream);

    auto col = make_collection(s, sa);
    across_values(col, add_to_stream);
    across_nodes(col, add_params_to_stream);

    cout << ss.str() << "\n";
}

TEST_CASE("Pre-made subsets") {
    auto a = make_node_array<exponential>(3, n_to_const(1.0));
    auto m =
        make_node_matrix<exponential>(3, 2, [& v = get<value>(a)](int i, int) { return v[i]; });
    auto gen = make_generator();
    draw(a, gen);
    draw(m, gen);

    auto c = make_collection(subsets::element(a, 1), subsets::row(m, 1));

    stringstream ss;
    auto add_params_to_stream = [&ss](auto distrib, auto& value, auto... params) {
        std::vector<double> ps = {params...};
        ss << "<" << typeid(distrib).name() << ": " << value << ", ";
        for (auto e : ps) { ss << e << " "; }
        ss << "> ";
    };

    across_nodes(c, add_params_to_stream);
    cout << "========================\n" << ss.str() << "\n";
}

TEST_CASE("new backup implem") {
    auto gen = make_generator();
    auto a = make_node_array<exponential>(5, n_to_const(1.0));
    auto a2 = make_node_array<exponential>(5, n_to_n(a));

    auto c = make_collection(a, subsets::element(a2, 2));
    draw(c, gen);
    cout << raw_value(a, 2) << ": " << &raw_value(a, 2) << "\n";

    auto ba = backup(a);
    draw(c, gen);
    cout << raw_value(a, 2) << ": " << &raw_value(a, 2) << "\n";
    restore(a, ba);
    cout << raw_value(a, 2) << ": " << &raw_value(a, 2) << "\n";

    auto bc = backup(c);
    cout << raw_value(a2, 2) << ": " << &raw_value(a2, 2) << "\n";
    draw(c, gen);
    cout << raw_value(a2, 2) << ": " << &raw_value(a2, 2) << "\n";
    restore(c, bc);
    cout << raw_value(a2, 2) << ": " << &raw_value(a2, 2) << "\n";
}

template <class T>
using is_double = std::is_same<T, double>;

template <class T>
using is_int = std::is_same<T, int>;

struct IntDoubleVisitor : public Visitor<IntDoubleVisitor, is_double, is_int, is_node> {
    std::ostream& s;
    IntDoubleVisitor(std::ostream& s) : s(s) {}

    void operator()(verifies<is_int>, int& i) { s << "Got int " << i << ". "; }

    void operator()(verifies<is_double>, double& x) { s << "Got double " << x << ". "; }

    template <class Node>
    void operator()(verifies<is_node>, Node&) {
        s << "Got a node. ";
    }

    using Visitor<IntDoubleVisitor, is_double, is_int, is_node>::operator();
};

TEST_CASE("Visitor") {
    std::stringstream ss;
    IntDoubleVisitor v{ss};

    int i = 2;
    double x = 3;
    size_t n = 5;

    v(i);
    v(x);
    v(n);

    CHECK(ss.str() == "Got int 2. Got double 3. ");

    auto mynode = make_node<exponential>(1.0);
    int j = 7;
    auto c = make_collection(mynode, j);
    c.across_elements(v);
    CHECK(ss.str() == "Got int 2. Got double 3. Got a node. Got int 7. ");
}