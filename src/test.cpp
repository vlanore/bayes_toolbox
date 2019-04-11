#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "config.hpp"
#include "distrib_draw.hpp"
using namespace std;

#define NB_POINTS 10000
#define PRECISION 0.025

TEST_CASE("Draw in various distribs") {
    auto gen = make_generator();

    distrib::constant_t four = {4};
    distrib::exponential_t alpha;
    distrib::exponential_param_t alpha_param = {four.value};
    double sum = 0;
    for (int i = 0; i < NB_POINTS; i++) {
        draw(alpha, alpha_param, gen);
        sum += alpha.value;
    }
    // expected mean is 1/rate = 1/4
    CHECK(sum / NB_POINTS == doctest::Approx(0.25).epsilon(PRECISION));

    distrib::gamma_t lambda;
    sum = 0;
    for (int i = 0; i < NB_POINTS; i++) {
        draw(lambda, 2, 3, gen);
        sum += lambda.value;
    }
    // expected mean is shape * scale = 2 * 3 = 6
    CHECK(sum / NB_POINTS == doctest::Approx(6).epsilon(PRECISION));
}

TEST_CASE("Gamma/Exp super simple model") {
    auto gen = make_generator();

    distrib::constant_t two = {2};
    distrib::exponential_t k;
    distrib::exponential_param_t k_param = {two.value};
    distrib::exponential_t theta;
    distrib::exponential_param_t theta_param = {two.value};
    distrib::gamma_t lambda;

    double sum = 0;
    for (int i = 0; i < NB_POINTS; i++) {
        draw(k, k_param, gen);
        draw(theta, theta_param, gen);
        draw(lambda, k.value, theta.value, gen);
        sum += lambda.value;
    }
    // expected mean is 1/2 * 1/2 = 0.25
    CHECK(sum / NB_POINTS == doctest::Approx(0.25).epsilon(PRECISION));
}