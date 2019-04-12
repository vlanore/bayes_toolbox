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
    auto alpha_param = distrib::exponential::make_params(four.value);
    double sum = 0;
    for (int i = 0; i < NB_POINTS; i++) {
        draw(alpha, alpha_param.rate, gen);
        sum += alpha.value;
    }
    // expected mean is 1/rate = 1/4
    CHECK(sum / NB_POINTS == doctest::Approx(0.25).epsilon(PRECISION));

    distrib::constant_t two = {2};
    distrib::constant_t three = {3};
    distrib::gamma_t lambda;
    auto lambda_param = distrib::gamma::make_params(two.value, three.value);
    sum = 0;
    for (int i = 0; i < NB_POINTS; i++) {
        draw(lambda, lambda_param.shape, lambda_param.scale, gen);
        sum += lambda.value;
    }
    // expected mean is shape * scale = 2 * 3 = 6
    CHECK(sum / NB_POINTS == doctest::Approx(6).epsilon(PRECISION));
}

TEST_CASE("Gamma/Exp super simple model") {
    auto gen = make_generator();

    distrib::constant_t two = {2};
    distrib::exponential_t k;
    auto k_param = distrib::exponential::make_params(two.value);
    distrib::exponential_t theta;
    auto theta_param = distrib::exponential::make_params(two.value);
    distrib::gamma_t lambda;
    auto lambda_param = distrib::gamma::make_params(k.value, theta.value);

    double sum = 0;
    for (int i = 0; i < NB_POINTS; i++) {
        draw(k, k_param.rate, gen);
        draw(theta, theta_param.rate, gen);
        draw(lambda, lambda_param.shape, lambda_param.scale, gen);
        sum += lambda.value;
    }
    // expected mean is 1/2 * 1/2 = 0.25
    CHECK(sum / NB_POINTS == doctest::Approx(0.25).epsilon(PRECISION));
}

TEST_CASE("Lambda as draw parameters") {
    auto gen = make_generator();

    distrib::exponential_t alpha;
    auto alpha_param = distrib::exponential::make_params([]() { return 2; });
    double sum = 0;
    for (int i = 0; i < NB_POINTS; i++) {
        draw(alpha, alpha_param.rate, gen);
        sum += alpha.value;
    }
    // expected mean is 1/rate = 1/4
    CHECK(sum / NB_POINTS == doctest::Approx(0.5).epsilon(PRECISION));
}