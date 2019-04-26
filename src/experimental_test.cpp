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

#include <memory>
#include <tuple>
#include "tagged_tuple.hpp"
using std::make_unique;
using std::move;
using std::tuple;
using std::unique_ptr;

struct IntNode {
    int i{0};
    IntNode(int i) : i(i) {}
    static auto make(int value) { return make_unique<IntNode>(value); }
};

struct RefNode {
    int& i;
    RefNode(int& i) : i(i) {}
    static auto make(int& ref) { return make_unique<RefNode>(ref); }
};

template <class Tag1, class Type1, class Tag2, class Type2>
auto make_model(Tag1, unique_ptr<Type1>& ptr1, Tag2, unique_ptr<Type2>& ptr2) {
    using fields = tuple<field<Tag1, unique_ptr<Type1>>, field<Tag2, unique_ptr<Type2>>>;
    return ttuple<fields>(move(ptr1), move(ptr2));
};

struct alpha {};
struct beta {};

struct poisson_gamma {
    static auto make() {
        auto a = IntNode::make(3);
        auto b = RefNode::make(a->i);
        return make_model(alpha(), a, beta(), b);
    }
};

TEST_CASE("Hello world") {
    auto model = poisson_gamma::make();
    CHECK(get_ref<alpha>(model)->i == 3);
    CHECK(get_ref<beta>(model)->i == 3);
}