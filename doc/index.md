# Bayes' toolbox documentation

This document presents the *Bayes' toolbox* library with feature-by-feature examples.
It is assumed the reader has a basic understanding of traditional C++98.
Modern C++ features and external library features should be explicitly mentioned when they are needed.

## Creating probabilistic nodes

**Prerequisites:** this section uses the `auto` keyword that is a C++11 feature.
[Here](https://www.learncpp.com/cpp-tutorial/4-8-the-auto-keyword/) is a tutorial which explains how `auto` works. Basically, `auto var = value` tells the compiler to auto-deduce the type of `var` from the type of `value`.
This is important for library users as it allows to auto-deduce the type of objects with very complex types.

```cpp
#include <iostream>
#include "distributions/exponential.hpp"
#include "structure/node.hpp"

int main() {
    // create an probabilistic with exponential distribution
    // and constant parameter set to 1.0
    auto my_node = make_node<exponential>(1.0);

    // display node value in stdout
    std::cout << get<value>(my_node) << "\n";
}
```
