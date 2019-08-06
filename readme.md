# Bayes' toolbox  	&#128736;

[![Build Status](https://travis-ci.org/vlanore/bayes_toolbox.svg?branch=master)](https://travis-ci.org/vlanore/bayes_toolbox) [![codecov](https://codecov.io/gh/vlanore/bayes_toolbox/branch/master/graph/badge.svg)](https://codecov.io/gh/vlanore/bayes_toolbox) [![licence CeCILL](https://img.shields.io/badge/license-CeCILL--C-blue.svg)](http://www.cecill.info/licences.en.html)

**Bayes toolbox** is a C++14 header-only library for high-performance Bayesian inference that enables low-level optimizations.
Data types are kept as simple as possible (e.g. structs with raw ` double` members) but the library provides powerful polymorphic operations on those datatypes.
Polymorphism is obtained at compile-time with template metaprogramming to minimize runtime overhead.

Here is a small example of what can be done with the library:
```cpp
// simple graphical model
auto alpha = make_node<exponential>(1.0);
auto mu = make_node<exponential>(1.0);
auto lambda = make_node_array<gamma_ss>(5, n_to_one(alpha), n_to_one(mu)); //node array

auto gen = make_generator(); // random generator
draw(alpha, gen); // draw in distribution
draw(mu, gen);
draw(lambda, gen); // draws whole array
double my_logprob = logprob(alpha) + logprob(mu) + logprob(lambda);
```

Bayes' toolbox uses the following header-only libraries:
* tagged_tuple - [github repo](https://github.com/vlanore/tagged_tuple) - [CeCill-C license](LICENSE.txt)
* doctest - [github repo](https://github.com/onqtam/doctest) - [MIT license](utils/LICENSE.txt)
