#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

#include "config.hpp"
#include "distrib_draw.hpp"
using namespace std;

TEST_CASE("Draw in various distribs") {
    auto gen = make_generator();
    distrib::exponential_t alpha;
    draw(alpha, 1, gen);
    CHECK(alpha.value > 0);
}

TEST_CASE("Gamma/Exp super simple model") {
    // distrib::exponential_t alpha;
    // config::param::mode_t alpha_mode = config::param::fixed;

    // distrib::exponential_t mu;
    // config::param::mode_t mu_mode = config::param::independent;

    // distrib::gamma_t lambda;
    // config::param::mode_t lambda_mode = config::param::independent;
}