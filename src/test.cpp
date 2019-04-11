#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "config.hpp"
#include "distrib_draw.hpp"
using namespace std;

#define NB_POINTS 10000
#define PRECISION 0.025

TEST_CASE("Draw in various distribs") {
    auto gen = make_generator();

    distrib::exponential_t alpha;
    double sum = 0;
    for (int i = 0; i < NB_POINTS; i++) {
        draw(alpha, 4, gen);
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

    distrib::exponential_t k;
    distrib::exponential_t theta;
    distrib::gamma_t lambda;

    double sum = 0;
    for (int i = 0; i < NB_POINTS; i++) {
        draw(k, 2, gen);
        draw(theta, 2, gen);
        draw(lambda, k.value, theta.value, gen);
        sum += lambda.value;
    }
    // expected mean is 1/2 * 1/2 = 0.25
    CHECK(sum / NB_POINTS == doctest::Approx(0.25).epsilon(PRECISION));
}