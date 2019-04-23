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

#include <string>
#include <tuple>
using std::get;
using std::string;
using std::tuple;

template <class Tag, class Type>
struct field {};

namespace ttuple_helper {  // tag -> index correspondance
    auto helper(std::tuple<>) { return std::tuple<>(); }

    template <class Tag, class Type, class... Rest>
    auto helper(tuple<field<Tag, Type>, Rest...>) {
        return std::tuple_cat(std::make_tuple(Tag()), helper(tuple<Rest...>()));
    }

    template <class... Fields>
    auto get_tags(tuple<Fields...> fields) {
        return helper(fields);
    }

    auto tuple_helper(std::tuple<>) { return std::tuple<>(); }

    template <class Tag, class Type, class... Rest>
    auto tuple_helper(tuple<field<Tag, Type>, Rest...>) {
        return std::tuple_cat(std::make_tuple(Type()), tuple_helper(tuple<Rest...>()));
    }

    template <class... Fields>
    auto get_tuple(tuple<Fields...> fields) {
        return tuple_helper(fields);
    }

    // INDEXES
    template <int, class Tag>
    auto index_helper(std::tuple<>) {
        return std::integral_constant<int, -1>();
    }

    template <int index, class Tag, class First, class... Rest>
    auto index_helper(std::tuple<First, Rest...>) {
        using t_if_same = std::integral_constant<int, index>;
        using t_if_different = decltype(index_helper<index + 1, Tag>(std::tuple<Rest...>()));
        using same_type = std::is_same<Tag, First>;
        return std::conditional_t<same_type::value, t_if_same, t_if_different>();
    }

    template <class Tag, class... Tags>
    constexpr auto get_index(std::tuple<Tags...> tags) {
        using result = decltype(index_helper<0, Tag>(tags));
        static_assert(result::value != -1, "Tag not found");
        return result::value;
    }

    template <class Tags, class Tuple>
    struct tagged_tuple {
        Tuple data;
        template <class... Args>
        tagged_tuple(Args&&... args) : data(std::forward<Args>(args)...) {}
    };

    template <class Fields>
    auto make_tagged_tuple_type() {
        using tags = decltype(get_tags(Fields()));
        using data = decltype(get_tuple(Fields()));
        return tagged_tuple<tags, data>();
    }
};  // namespace ttuple_helper

template <class Fields>
using ttuple = decltype(ttuple_helper::make_tagged_tuple_type<Fields>());

template <class AddressFirst, class Tags, class Tuple>
auto get(const ttuple_helper::tagged_tuple<Tags, Tuple>& ttuple) {
    return get<ttuple_helper::get_index<AddressFirst>(Tags())>(ttuple.data);
}

template <class AddressFirst, class... AddressRest, class TTuple>
auto get(const TTuple& ttuple) {
    return get<AddressRest...>(get<AddressFirst>(ttuple));
}

TEST_CASE("Basic tuple test") {
    struct alpha {};
    struct beta {};
    struct gamma {};

    using my_fields = std::tuple<field<alpha, int>, field<beta, std::string>>;
    SUBCASE("Manual tests of under-the-hood things") {
        using namespace ttuple_helper;
        using my_tags = decltype(get_tags(my_fields()));
        using my_tuple_t = decltype(get_tuple(my_fields()));

        my_tuple_t my_tuple{2, "hello"};

        using tlist = std::tuple<alpha, beta>;
        bool ok_tags = std::is_same<my_tags, tlist>::value;
        CHECK(ok_tags);
        CHECK(get_index<alpha>(tlist()) == 0);
        CHECK(get_index<beta>(tlist()) == 1);
        // CHECK(get_index<gamma>(tlist()) == -1); // triggers static assert

        auto a = get<get_index<beta>(tlist())>(my_tuple);
        CHECK(a == "hello");
        auto b = get<get_index<alpha>(tlist())>(my_tuple);
        CHECK(b == 2);
    }
    SUBCASE("Using user-level interface") {
        using hello_t = ttuple<my_fields>;
        hello_t my_other_struct{3, "hi"};
        CHECK(get<alpha>(my_other_struct) == 3);
        CHECK(get<beta>(my_other_struct) == "hi");
    }
    SUBCASE("Multiple levels") {
        using sttuple_t = ttuple<std::tuple<field<alpha, int>>>;
        using cttuple_t = ttuple<std::tuple<field<beta, sttuple_t>>>;
        cttuple_t my_tuple{sttuple_t{7}};
        CHECK(get<beta, alpha>(my_tuple) == 7);
    }
}