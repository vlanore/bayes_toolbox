#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "config.hpp"
#include "distrib_draw.hpp"
using namespace std;

#define NB_POINTS 10000
#define PRECISION 0.025

TEST_CASE("Draw in various distribs") {
    auto gen = make_generator();

    distrib::constant::value_t four = {4};
    distrib::exponential::value_t alpha;
    auto alpha_param = distrib::exponential::make_params(four.value);
    double sum = 0;
    for (int i = 0; i < NB_POINTS; i++) {
        draw(alpha, alpha_param, gen);
        sum += alpha.value;
    }
    // expected mean is 1/rate = 1/4
    CHECK(sum / NB_POINTS == doctest::Approx(0.25).epsilon(PRECISION));

    distrib::constant::value_t two = {2};
    distrib::constant::value_t three = {3};
    distrib::gamma::value_t lambda;
    auto lambda_param = distrib::gamma::make_params(two.value, three.value);
    sum = 0;
    for (int i = 0; i < NB_POINTS; i++) {
        draw(lambda, lambda_param, gen);
        sum += lambda.value;
    }
    // expected mean is shape * scale = 2 * 3 = 6
    CHECK(sum / NB_POINTS == doctest::Approx(6).epsilon(PRECISION));
}

TEST_CASE("Gamma/Exp super simple model") {
    auto gen = make_generator();

    distrib::constant::value_t two = {2};
    distrib::exponential::value_t k;
    auto k_param = distrib::exponential::make_params(two.value);
    distrib::exponential::value_t theta;
    auto theta_param = distrib::exponential::make_params(two.value);
    distrib::gamma::value_t lambda;
    auto lambda_param = distrib::gamma::make_params(k.value, theta.value);

    double sum = 0;
    for (int i = 0; i < NB_POINTS; i++) {
        draw(k, k_param, gen);
        draw(theta, theta_param, gen);
        draw(lambda, lambda_param, gen);
        sum += lambda.value;
    }
    // expected mean is 1/2 * 1/2 = 0.25
    CHECK(sum / NB_POINTS == doctest::Approx(0.25).epsilon(PRECISION));
}

TEST_CASE("Lambda and rvalue constants as draw parameters") {
    auto gen = make_generator();

    distrib::exponential::value_t alpha;
    double sum = 0;
    SUBCASE("lambda param") {
        auto alpha_param = distrib::exponential::make_params([]() { return 2; });
        for (int i = 0; i < NB_POINTS; i++) {
            draw(alpha, alpha_param, gen);
            sum += alpha.value;
        }
    }
    SUBCASE("rvalue param") {
        auto alpha_param = distrib::exponential::make_params(2);
        for (int i = 0; i < NB_POINTS; i++) {
            draw(alpha, alpha_param, gen);
            sum += alpha.value;
        }
    }
    // expected mean is 1/rate = 1/2
    CHECK(sum / NB_POINTS == doctest::Approx(0.5).epsilon(PRECISION));
}