
#pragma once

#include <vector>
#include "datatypes.hpp"
#include "params.hpp"

template <class Tag, class Distrib, class RootDistrib>
using tree_process_metadata = metadata<type_list<node_tag, Tag>, type_map<property<distrib, Distrib>, property<root_distrib, RootDistrib>>>;

template<class Chrono>
auto time_frame(const Chrono& chrono)   {
    return [&ch = chrono] (int node) {return ch[node];};
}

template<class... Args>
static auto process_params(Args... args)  {
    return std::make_tuple(args...);
}

template<class... Args>
static auto root_params(Args... args)  {
    return std::make_tuple(args...);
}

template<class Distrib, class... Args, size_t... I>
static auto make_params_from_tuple(std::tuple<Args...> tuple, std::index_sequence<I...>)   {
    return make_params<Distrib>(std::get<I>(tuple)...);
}

template <class Distrib, class RootDistrib, class Tree, class TimeFrame, class ParamArgsTuple, class RootParamArgsTuple>
auto make_node_tree_process(Tree& tree, TimeFrame timeframe, ParamArgsTuple args, RootParamArgsTuple root_args) {

    size_t nnode = tree.nb_nodes();
    size_t nbranch = tree.nb_branches();
    std::vector<typename Distrib::instantT> node_vals(nnode);
    std::vector<typename Distrib::pathT> path_vals(nbranch);
    auto val = std::make_pair(node_vals, path_vals);
    auto initC = Distrib::make_init_constraint(node_vals[0]);
    std::vector<typename Distrib::Constraint> clamps(nnode, initC);

    static constexpr auto param_size = std::tuple_size<ParamArgsTuple>::value;
    auto param = make_params_from_tuple<Distrib>(args, std::make_index_sequence<param_size>{});

    static constexpr auto root_param_size = std::tuple_size<RootParamArgsTuple>::value;
    auto root_param = make_params_from_tuple<RootDistrib>(root_args, std::make_index_sequence<root_param_size>{});

    return make_tagged_tuple<tree_process_metadata<node_cond_tree_process_tag, Distrib, RootDistrib>>(
        /*
        unique_ptr_field<struct node_values>(std::move(node_vals)),
        unique_ptr_field<struct path_values>(std::move(path_vals)),
        */
        unique_ptr_field<struct value>(std::move(val)),
        ref_field<struct node_values>(node_vals),
        ref_field<struct path_values>(path_vals),
        unique_ptr_field<struct constraint>(std::move(clamps)),
        ref_field<tree_field>(tree), 
        value_field<time_frame_field>(timeframe), 
        value_field<struct params>(param),
        value_field<struct root_params>(root_param));
}

template <class Distrib, class RootDistrib, class Tree, class TimeFrame, class ParamArgsTuple, class RootParamArgsTuple>
auto make_node_tree_process_with_inits(Tree& tree, TimeFrame timeframe, typename Distrib::instantT instant_init, typename Distrib::pathT path_init, ParamArgsTuple args, RootParamArgsTuple root_args) {

    size_t nnode = tree.nb_nodes();
    size_t nbranch = tree.nb_branches();
    using node_vec = std::vector<typename Distrib::instantT>;
    using path_vec = std::vector<typename Distrib::pathT>;
    node_vec node_vals(nnode, instant_init);
    path_vec path_vals(nbranch, path_init);
    auto val = std::pair<node_vec&, path_vec&>(node_vals, path_vals);
    auto initC = Distrib::make_init_constraint(instant_init);
    std::vector<typename Distrib::Constraint> clamps(nnode, initC);

    static constexpr auto param_size = std::tuple_size<ParamArgsTuple>::value;
    auto param = make_params_from_tuple<Distrib>(args, std::make_index_sequence<param_size>{});

    static constexpr auto root_param_size = std::tuple_size<RootParamArgsTuple>::value;
    auto root_param = make_params_from_tuple<RootDistrib>(root_args, std::make_index_sequence<root_param_size>{});

    return make_tagged_tuple<tree_process_metadata<node_cond_tree_process_tag, Distrib, RootDistrib>>(
        unique_ptr_field<struct node_values>(std::move(node_vals)),
        unique_ptr_field<struct path_values>(std::move(path_vals)),
        value_field<struct value>(val),
        // unique_ptr_field<struct value>(std::move(val)),
        // ref_field<struct value>(node_vals),
        /*
        unique_ptr_field<struct value>(std::move(val)),
        ref_field<struct node_values>(node_vals),
        ref_field<struct path_values>(path_vals),
        */
        unique_ptr_field<struct constraint>(std::move(clamps)),
        ref_field<tree_field>(tree), 
        value_field<time_frame_field>(timeframe), 
        value_field<struct params>(param),
        value_field<struct root_params>(root_param));
}

