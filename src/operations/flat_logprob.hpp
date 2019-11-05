#pragma once

template <class Distrib, class T, class F, class Params, class... Keys, class... Indexes>
static double logprob_unpack_params(Distrib, T& x, F f, const Params& params, std::tuple<Keys...>,
                   Indexes... is) {
    return f(Distrib{}, x, get<Keys>(params)(is...)...);
}

template<class Node, class F>
static double logprob_apply(lone_node_tag, Node& n, F f)   {
    using distrib = node_distrib_t<Node>;
    using keys = param_keys_t<distrib>;
    return logprob_unpack_params(distrib{}, raw_value(n), f, get<params>(n), keys());
}

template<class Array, class F>
static double logprob_apply(node_array_tag, Array& a, F f)    {
    using distrib = node_distrib_t<Array>;
    using keys = param_keys_t<distrib>;
    double tot = 0;
    for (size_t i=0; i<get<value>(a).size(); i++)   {
        tot += logprob_unpack_params(distrib{}, raw_value(a, i), f, get<params>(a), keys(), i);
    }
    return tot;
}

template<class Array, class F>
static double logprob_apply(node_array_tag, Array& a, F f, int i)    {
    using distrib = node_distrib_t<Array>;
    using keys = param_keys_t<distrib>;
    return logprob_unpack_params(distrib{}, raw_value(a, i), f, get<params>(a), keys(), i);
}

template<class Matrix, class F>
static double logprob_apply(node_matrix_tag, Matrix& a, F f)    {
    using distrib = node_distrib_t<Matrix>;
    using keys = param_keys_t<distrib>;
    double tot = 0;
    for (size_t i=0; i<get<value>(a).size(); i++)   {
        for (size_t j=0; i<get<value>(a)[0].size(); j++)   {
            tot += logprob_unpack_params(distrib{}, raw_value(a, i, j), f, get<params>(a), keys(), i, j);
        }
    }
    return tot;
}

template<class Matrix, class F>
static double logprob_apply(node_matrix_tag, Matrix& a, F f, int i, int j)    {
    using distrib = node_distrib_t<Matrix>;
    using keys = param_keys_t<distrib>;
    return logprob_unpack_params(distrib{}, raw_value(a, i, j), f, get<params>(a), keys(), i, j);
}

template <class Node, class... Args>
static double flat_logprob(Node& n, Args... args) {
    auto logprob_lambda = [] (auto distrib, auto& x, auto... params) { return decltype(distrib)::logprob(x,params...); };
    return logprob_apply(type_tag(n), n, logprob_lambda, args...);
}

