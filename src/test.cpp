#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "config.hpp"
#include "distrib_draw.hpp"
using namespace std;

#define NB_POINTS 10000
#define PRECISION 0.025

void check_mean(double& target, function<void()> draw, double expected_mean) {
    double sum = 0;
    for (int i = 0; i < NB_POINTS; i++) {
        draw();
        sum += target;
    }
    CHECK(sum / NB_POINTS == doctest::Approx(expected_mean).epsilon(PRECISION));
}

TEST_CASE("Draw in various distribs") {
    auto gen = make_generator();

    SUBCASE("exponential distribution") {
        distrib::exponential::value_t alpha;
        auto alpha_param = distrib::exponential::make_params(4);
        check_mean(alpha.value, [&]() { draw(alpha, alpha_param, gen); }, 0.25);
    }

    SUBCASE("gamma distribution") {
        distrib::gamma::value_t lambda;
        auto lambda_param = distrib::gamma::make_params(2, 3);
        check_mean(lambda.value, [&]() { draw(lambda, lambda_param, gen); }, 6);
    }
}

TEST_CASE("Gamma/Exp super simple model") {
    auto gen = make_generator();

    distrib::exponential::value_t k;
    auto k_param = distrib::exponential::make_params(2);
    distrib::exponential::value_t theta;
    auto theta_param = distrib::exponential::make_params(2);
    distrib::gamma::value_t lambda;
    auto lambda_param = distrib::gamma::make_params(k.value, theta.value);
    check_mean(lambda.value,
               [&]() {
                   draw(k, k_param, gen);
                   draw(theta, theta_param, gen);
                   draw(lambda, lambda_param, gen);
               },
               0.25);
}

TEST_CASE("Lambda and rvalue constants as draw parameters") {
    auto gen = make_generator();

    distrib::exponential::value_t alpha;
    SUBCASE("lambda param") {
        auto alpha_param = distrib::exponential::make_params([]() { return 2; });
        check_mean(alpha.value, [&]() { draw(alpha, alpha_param, gen); }, 0.5);
    }
    SUBCASE("rvalue param") {
        auto alpha_param = distrib::exponential::make_params(2);
        check_mean(alpha.value, [&]() { draw(alpha, alpha_param, gen); }, 0.5);
    }
    SUBCASE("lvalue param") {
        double my_param = 17;
        auto alpha_param = distrib::exponential::make_params(my_param);
        my_param = 2;
        check_mean(alpha.value, [&]() { draw(alpha, alpha_param, gen); }, 0.5);
    }
}