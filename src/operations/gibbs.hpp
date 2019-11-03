
struct gibbs    {

    template <class Distrib, class T, class SS, class F, class Params, class... Keys, class... Indexes>
    static void unpack_params(Distrib, T& x, SS& ss, F f, const Params& params, std::tuple<Keys...>,
                       Indexes... is) {
        f(Distrib{}, x, ss, get<Keys>(params)(is...)...);
    }

    template<class Node, class SS, class F>
    static void gibbs_apply(lone_node_tag, Node& n, SS& ss, F f)   {
        using distrib = node_distrib_t<Node>;
        using keys = param_keys_t<distrib>;
        unpack_params(distrib{}, raw_value(n), ss.get(), f, get<params>(n), keys());
    }

    template<class Array, class SS, class F>
    static void gibbs_apply(node_array_tag, Array& a, SS& ss, F f)    {
        using distrib = node_distrib_t<Array>;
        using keys = param_keys_t<distrib>;
        for (size_t i=0; i<get<value>(a).size(); i++)   {
            unpack_params(distrib{}, raw_value(a, i), ss.get(i), f, get<params>(a), keys(), i);
        }
    }
};

template <class Node, class SS, class Gen>
static void gibbs_resample(Node& n, SS& ss, Gen& gen) {
    auto gibbs_lambda = [&gen] (auto distrib, auto& x, auto& s, auto... params) { decltype(distrib)::gibbs_resample(x,s,params...,gen); };
    gibbs::gibbs_apply(type_tag(n), n, ss, gibbs_lambda);
}

