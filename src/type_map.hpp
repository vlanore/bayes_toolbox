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

#include <tuple>
using std::tuple;

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
