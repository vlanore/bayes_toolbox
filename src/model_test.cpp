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
#include "exponential.hpp"
#include "gamma.hpp"
#include "logprob.hpp"
#include "math_utils.hpp"
#include "mcmc_utils.hpp"
#include "poisson.hpp"
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
#define AUTO_PARAM(name, init) decltype((init)) name = (init);
#define AUTO_DPARAM(name, init) decltype((init)) name;
#define CARG(type) std::declval<type>()
#define FORW(value) std::forward<decltype((value))>((value))

struct poisson_gamma {
    template <class P1, class P2>
    struct model_t {
        AUTO_DPARAM(lambda, gamma::make_node(CARG(P1), CARG(P2)));
        AUTO_PARAM(k, poisson::make_node(lambda.value.value));

        model_t(P1&& p1, P2&& p2) : lambda(gamma::make_node(FORW(p1), FORW(p2))) {}
    };

    template <class P1, class P2>
    static auto make_model(P1&& p1, P2&& p2) {
        return make_templated_struct<model_t>(FORW(p1), FORW(p2));
    }
};

TEST_CASE("Automatic model arg deduction") {
    // using dfunc = decltype(ParamFactory<double>::make(2));
    // using LT = ProbNode<gamma::value_t, gamma::Param<dfunc, dfunc>>;
    // using KT = ProbNode<poisson::value_t, poisson::Param<DRef>>;
    auto gen = make_generator();
    SUBCASE("Constant params") {
        auto model = poisson_gamma::make_model(1, 2);

        using ptype = decltype(model.lambda.params);
        using dfunc = decltype(ParamFactory<double>::make(1.0));
        CHECK(std::is_same<ptype, gamma::Param<dfunc, dfunc>>::value);

        check_mean(model.lambda.value.value, [&]() { draw(model.lambda, gen); }, 2.0);
        check_mean(model.k.value.value,
                   [&]() {
                       draw(model.lambda, gen);
                       draw(model.k, gen);
                   },
                   2.0);
    }
    SUBCASE("Reference params") {
        double p1 = 0.0;
        double p2 = 0.0;
        auto model = poisson_gamma::make_model(p1, p2);
        p1 = 1.0;
        p2 = 2.0;

        using ptype = decltype(model.lambda.params);
        CHECK(std::is_same<ptype, gamma::Param<DRef, DRef>>::value);

        check_mean(model.lambda.value.value, [&]() { draw(model.lambda, gen); }, 2.0);
        check_mean(model.k.value.value,
                   [&]() {
                       draw(model.lambda, gen);
                       draw(model.k, gen);
                   },
                   2.0);
    }
}

// struct poisson_gamma_2 {
//     struct model_data_t {
//         gamma::value_t lambda;
//         poisson::value_t k;
//     };

//     template <class P1, class P2>
//     struct params_t {
//         P1 p1;
//         P2 p2;
//     };

//     template <class P1, class P2>
//     static auto make_params(P1&& p1, P2&& p2) {
//         return make_templated_struct<params_t>(ParamFactory<double>::make(forward<P1>(p1)),
//                                                ParamFactory<double>::make(forward<P2>(p2)));
//     }

//     template <class Params>
//     struct model_t {
//         Params params;
//         model_data_t data;
//         AUTO_PARAM(lambda, (make_probnode_vref(data.lambda, params.p1, params.p2)));
//         AUTO_PARAM(k, (make_probnode_vref(data.k, data.lambda.value)));
//     };

//     template <class... ParamArgs>
//     static auto make_model(ParamArgs&&... args) {
//         auto params = make_params(forward<ParamArgs>(args)...);
//         return model_t<decltype(params)>{.params = params};
//     }
// };

// TEST_CASE("Another approach to models (with independent data") {
//     auto gen = make_generator();
//     auto model = poisson_gamma_2::make_model(1, 2);

//     // using lptype = decltype(model.lambda.params);
//     // using dfunc = decltype(ParamFactory<double>::make(1.0));
//     // CHECK(std::is_same<lptype, gamma::Param<dfunc, dfunc>>::value);
//     // using kptype = decltype(model.k.params);
//     // CHECK(std::is_same<kptype, poisson::Param<DRef>>::value);

//     check_mean(model.lambda.value.value, [&]() { draw(model.lambda, gen); }, 2.0);
//     check_mean(model.k.value.value,
//                [&]() {
//                    draw(model.lambda, gen);
//                    draw(model.k, gen);
//                },
//                2.0);
// }