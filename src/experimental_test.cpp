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
using std::unique_ptr;
using std::make_unique;
using std::tuple;

struct AbstractNode {
    virtual ~AbstractNode() = default;
};

struct Node {
    unique_ptr<AbstractNode> ptr;
};

struct alpha {};
struct beta {};
struct gamma {};

class MyModel {  // model with tag-addressable fields
    Node alpha{nullptr};
    Node beta{nullptr};
    Node& get(::alpha) { return alpha; }
    Node& get(::beta) { return beta; }

  public:
    template <class Tag>
    Node& get() {
        return get(Tag());
    }
};

struct MyNode : AbstractNode {  // concrete node implem
    int i{0};
    MyNode(int i) : i(i) {}
};

template <class Tag, class Type>
struct TypePair {};

struct NotFound {};

template <class... Decls>
struct TypeMap {
    template <class Key, class Type>
    static auto add() {
        return TypeMap<Decls..., TypePair<Key, Type>>();
    }

    template <class Key>
    static auto helper(tuple<>) {
        return NotFound();
    }

    template <class RequestedKey, class Key, class Value, class... DeclRest>
    static auto helper(tuple<TypePair<Key, Value>, DeclRest...>) {
        using if_equal = Value;
        using if_not_equal = decltype(helper<RequestedKey>(tuple<DeclRest...>()));
        constexpr bool equality = std::is_same<RequestedKey, Key>::value;
        return std::conditional_t<equality, if_equal, if_not_equal>();
    }

    template <class Key>
    static auto get() {
        return helper<Key>(tuple<Decls...>());
    }

    template <class Key>
    using get_t = decltype(get<Key>());

    template <class Key, class Type>
    using add_t = decltype(add<Key, Type>());
};

// template <class Tag, class Type, class Model, class... Args>
// auto set_node(Model& model, Args&&... args) {
//     model.get<Tag>().ptr = make_unique<Type>(std::forward<Args>(args)...);
//     return TypeDecl<Tag, Type>();
// }

TEST_CASE("Type map") {
    using my_map = TypeMap<TypePair<alpha, int>, TypePair<beta, double>>;
    using alpha_t = typename my_map::get_t<alpha>;
    using beta_t = typename my_map::get_t<beta>;

    CHECK((std::is_same<alpha_t, int>::value));
    CHECK((std::is_same<beta_t, double>::value));
}

TEST_CASE("Hello world") {
    MyModel model;
    // auto info = set_node<alpha, MyNode>(model, 1);
    // auto info2 = set_node<beta, MyNode>(model, 2);
    // using tinfo = TypeInfo<decltype(info), decltype(info2)>;

    model.get<alpha>().ptr = make_unique<MyNode>(1);
    model.get<beta>().ptr = make_unique<MyNode>(2);
    auto& a_ref = dynamic_cast<MyNode&>(*model.get<alpha>().ptr);
    auto& b_ref = dynamic_cast<MyNode&>(*model.get<beta>().ptr);
    CHECK(a_ref.i == 1);
    CHECK(b_ref.i == 2);
}