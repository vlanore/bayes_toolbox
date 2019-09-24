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