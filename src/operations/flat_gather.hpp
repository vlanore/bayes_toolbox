#pragma once

template <class Distrib, class T, class F, class Params, class... Keys, class... Indexes>
static void gather_unpack_params(Distrib, T& x, F f, const Params& params, std::tuple<Keys...>,
                   Indexes... is) {
    f(Distrib{}, x, get<Keys>(params)(is...)...);
}

template<class Node, class F>
static void gather_apply(lone_node_tag, Node& n, F f)   {
    using distrib = node_distrib_t<Node>;
    using keys = param_keys_t<distrib>;
    gather_unpack_params(distrib{}, raw_value(n), f, get<params>(n), keys());
}

template<class Array, class F>
static void gather_apply(node_array_tag, Array& a, F f)    {
    using distrib = node_distrib_t<Array>;
    using keys = param_keys_t<distrib>;
    for (size_t i=0; i<get<value>(a).size(); i++)   {
        gather_unpack_params(distrib{}, raw_value(a, i), f, get<params>(a), keys(), i);
    }
}

template<class Array, class F>
static void gather_apply(node_array_tag, Array& a, F f, int i)    {
    using distrib = node_distrib_t<Array>;
    using keys = param_keys_t<distrib>;
    gather_unpack_params(distrib{}, raw_value(a, i), f, get<params>(a), keys(), i);
}

template <class Node, class... Args>
static void flat_gather(Node& n, Args... args) {
    auto gather_lambda = [] (auto distrib, auto& x, auto... params) { decltype(distrib)::gather(x,params...); };
    gather_apply(type_tag(n), n, gather_lambda, args...);
}

