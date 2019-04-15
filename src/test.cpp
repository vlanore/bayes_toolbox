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

TEST_CASE("Draw in various distribs") {
    auto gen = make_generator();

    SUBCASE("exponential distribution") {
        exponential::value_t alpha;
        auto alpha_param = exponential::make_params(4);
        check_mean(alpha.value, [&]() { draw(alpha, alpha_param, gen); }, 0.25);
    }

    SUBCASE("gamma distribution") {
        gamma::value_t lambda;
        auto lambda_param = gamma::make_params(2, 3);
        check_mean(lambda.value, [&]() { draw(lambda, lambda_param, gen); }, 6);
    }

    SUBCASE("poisson distribution") {
        poisson::value_t alpha;
        auto alpha_param = poisson::make_params(4);
        check_mean(alpha.value, [&]() { draw(alpha, alpha_param, gen); }, 4);
    }
}

TEST_CASE("Lambda and rvalue constants as draw parameters") {
    auto gen = make_generator();

    exponential::value_t alpha;
    SUBCASE("lambda param") {
        auto alpha_param = exponential::make_params([]() { return 2; });
        check_mean(alpha.value, [&]() { draw(alpha, alpha_param, gen); }, 0.5);
    }
    SUBCASE("rvalue param") {
        auto alpha_param = exponential::make_params(2);
        check_mean(alpha.value, [&]() { draw(alpha, alpha_param, gen); }, 0.5);
    }
    SUBCASE("lvalue param") {
        double my_param = 17;
        auto alpha_param = exponential::make_params(my_param);
        my_param = 2;
        check_mean(alpha.value, [&]() { draw(alpha, alpha_param, gen); }, 0.5);
    }
}

TEST_CASE("Node construction") {
    auto gen = make_generator();
    SUBCASE("exponential") {
        auto alpha = exponential::make_node(4);
        check_mean(alpha.value.value, [&]() { draw(alpha, gen); }, 0.25);
    }
    SUBCASE("gamma") {
        auto alpha = gamma::make_node(2, 3);
        check_mean(alpha.value.value, [&]() { draw(alpha, gen); }, 6.0);
    }
    SUBCASE("poisson") {
        auto alpha = poisson::make_node(3);
        check_mean(alpha.value.value, [&]() { draw(alpha, gen); }, 3.0);
    }
    SUBCASE("exponential with ref") {
        double my_param = 17;
        auto alpha = exponential::make_node(my_param);
        my_param = 4;
        check_mean(alpha.value.value, [&]() { draw(alpha, gen); }, 0.25);
    }
}

TEST_CASE("Node ref") {
    auto gen = make_generator();

    double two = 2.;
    exponential::value_t alpha;
    auto alpha_param = exponential::make_params(two);
    auto alpha_ref = make_probnode_ref(alpha, alpha_param);

    check_mean(alpha.value, [&]() { draw(alpha_ref, gen); }, 0.5);
}

TEST_CASE("Poisson/gamma simple model: draw values") {
    auto gen = make_generator();

    auto k = exponential::make_node(0.5);
    auto theta = exponential::make_node(0.5);
    auto lambda = gamma::make_node(k.value.value, theta.value.value);
    auto counts = poisson::make_node(lambda.value.value);

    check_mean(counts.value.value,
               [&]() {
                   draw(k, gen);
                   draw(theta, gen);
                   draw(lambda, gen);
                   draw(counts, gen);
               },
               4, 2.0);
}

TEST_CASE("Very simple manual MCMC") {
    auto gen = make_generator();

    auto param = exponential::make_node(1);
    draw(param, gen);
    auto array =
        make_probnode_array(10, [&param](int) { return poisson::make_node(param.value.value); });
    clamp_array(array, 2, 2, 2, 1, 2, 1, 2, 3, 2, 3);

    vector<double> trace;
    for (int i = 0; i < 10000; i++) {
        for (int rep = 0; rep < 10; rep++) {
            auto param_backup = param;
            double logprob_before = exponential::logprob(param.value.value, 1);
            for (auto pnode : array) {
                logprob_before += poisson::logprob(pnode.value.value, param.value.value);
            }
            double log_hastings = scale(param.value.value, gen);
            double logprob_after = exponential::logprob(param.value.value, 1);
            for (auto pnode : array) {
                logprob_after += poisson::logprob(pnode.value.value, param.value.value);
            }
            double acceptance = logprob_after - logprob_before + log_hastings;
            bool accept = draw_uniform(gen) < exp(acceptance);
            if (!accept) { param.value.value = param_backup.value.value; }
        }
        trace.push_back(param.value.value);
    }
    double sum_trace = 0;
    for (auto e : trace) { sum_trace += e; }
    double sum_mean = sum_trace / 10000;
    CHECK(1.8 < sum_mean);  // should be somewhere close to 2.0 but biaised down due to prior
    CHECK(sum_mean < 2.1);
}

TEST_CASE("Very simple manual MCMC with partial log probs") {
    auto gen = make_generator();

    auto param = exponential::make_node(1);
    draw(param, gen);
    auto array =
        make_probnode_array(10, [&param](int) { return poisson::make_node(param.value.value); });
    clamp_array(array, 2, 2, 2, 1, 2, 1, 2, 3, 2, 3);

    vector<double> trace;
    for (int i = 0; i < 10000; i++) {
        for (int rep = 0; rep < 10; rep++) {
            auto param_backup = param;
            double logprob_before = exponential::partial_logprob_value(param.value.value, 1);
            for (auto pnode : array) {
                logprob_before +=
                    poisson::partial_logprob_param1(pnode.value.value, param.value.value);
            }
            double log_hastings = scale(param.value.value, gen);
            double logprob_after = exponential::partial_logprob_value(param.value.value, 1);
            for (auto pnode : array) {
                logprob_after +=
                    poisson::partial_logprob_param1(pnode.value.value, param.value.value);
            }
            double acceptance = logprob_after - logprob_before + log_hastings;
            bool accept = draw_uniform(gen) < exp(acceptance);
            if (!accept) { param.value.value = param_backup.value.value; }
        }
        trace.push_back(param.value.value);
    }
    double sum_trace = 0;
    for (auto e : trace) { sum_trace += e; }
    double sum_mean = sum_trace / 10000;
    CHECK(1.8 < sum_mean);  // should be somewhere close to 2.0 but biaised down due to prior
    CHECK(sum_mean < 2.1);
}

TEST_CASE("Better manual MCMC") {
    auto gen = make_generator();

    auto param = exponential::make_node(1);
    draw(param, gen);
    auto array =
        make_probnode_array(10, [&param](int) { return poisson::make_node(param.value.value); });
    clamp_array(array, 2, 2, 2, 1, 2, 1, 2, 3, 2, 3);

    vector<double> trace;
    for (int i = 0; i < 10000; i++) {
        for (int rep = 0; rep < 10; rep++) {
            auto param_backup = make_value_backup(param);
            double logprob_before = logprob(param) + logprob(array);
            double log_hastings = scale(param.value.value, gen);
            double logprob_after = logprob(param) + logprob(array);
            bool accept = decide(logprob_after - logprob_before + log_hastings, gen);
            if (!accept) { restore_from_backup(param, param_backup); }
        }
        trace.push_back(param.value.value);
    }
    double sum_trace = 0;
    for (auto e : trace) { sum_trace += e; }
    double sum_mean = sum_trace / 10000;
    CHECK(1.8 < sum_mean);  // should be somewhere close to 2.0 but biaised down due to prior
    CHECK(sum_mean < 2.1);
}