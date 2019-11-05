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

#include <iostream>
#include "bayes_toolbox.hpp"
using namespace std;

TOKEN(lambda1)
TOKEN(lambda2)
TOKEN(mrate)
TOKEN(K)

auto poisson_product(size_t n1, size_t n2) {

    auto lambda1 = make_node_array<gamma_ss>(n1, n_to_const(1.0), n_to_const(1.0));
    auto lambda2 = make_node_array<gamma_ss>(n2, n_to_const(1.0), n_to_const(1.0));

    auto mrate = make_dnode_matrix<product>(
            n1, n2, 
            [&l1 = get<value>(lambda1)] (int i, int) { return l1[i]; },
            [&l2 = get<value>(lambda2)] (int, int j) { return l2[j]; });

    auto K = make_node_matrix<poisson>(
            n1, n2,
            [&r = get<value>(mrate)] (int i, int j) { return r[i][j]; }
        );

    // clang-format off
    return make_model(
        lambda1_ = move(lambda1),
        lambda2_ = move(lambda2),
          mrate_ = move(mrate),
              K_ = move(K)
    );  // clang-format on
}

template <class Node, class MB, class Gen, class... IndexArgs>
void scaling_move(Node& node, MB blanket, Gen& gen, IndexArgs... args) {
    auto index = make_index(args...);
    auto bkp = backup(node, index);
    double logprob_before = logprob(blanket);
    double log_hastings = scale(raw_value(node, index), gen);
    bool accept = decide(logprob(blanket) - logprob_before + log_hastings, gen);
    if (!accept) { restore(node, bkp, index); }
}

int main() {
    auto gen = make_generator();

    size_t n1 = 2;
    size_t n2 = 3;
    auto m = poisson_product(n1, n2);

    // auto v = make_collection(lambda1_(m), lambda2_(m));
    // vector<vector<int>> initK(n1, vector<int>(n2,1));
    // set_value(K_(m), initK);

    cerr << "draw model\n";
    draw(lambda1_(m), gen);
    draw(lambda2_(m), gen);
    gather(mrate_(m));
    draw(K_(m), gen);
    cerr << "draw model ok\n";

    for (size_t i=0; i<n1; i++) {
        for (size_t j=0; j<n2; j++) {
            cerr << i << '\t' << j;
            cerr << '\t' << get<lambda1,value>(m)[i] * get<lambda2,value>(m)[j];
            cerr << '\t' << get<mrate,value>(m)[i][j];
            cerr << '\t' << get<K,value>(m)[i][j];
            cerr << '\n';
        }
    }
    cerr << '\n' << '\n';

    size_t nb_it = 3;
    // size_t nb_it = 100000;
    for (size_t it = 0; it < nb_it; it++) {

        sweet_scaling_move(lambda1_(m), matrix_row_logprob(K_(m)), gen, matrix_row_gather(mrate_(m)));
        sweet_scaling_move(lambda2_(m), matrix_column_logprob(K_(m)), gen, matrix_column_gather(mrate_(m)));

        /*
        auto logprob_and_update_lambda1 = [&m] (int i)   {
            auto subset_r = subsets::row(mrate_(m), i);
            auto subset_K = subsets::row(K_(m), i);
            auto collec = make_collection(subset_r, subset_K);
            return logprob(collec);
        };

        auto logprob_and_update_lambda2 = [&m] (int j)   {
            auto subset_r = subsets::column(mrate_(m), j);
            auto subset_K = subsets::column(K_(m), j);
            auto collec = make_collection(subset_r, subset_K);
            return logprob(collec);
        };
        */

        cerr << "=======\n";
        for (size_t i=0; i<n1; i++) {
            for (size_t j=0; j<n2; j++) {
                cerr << i << '\t' << j;
                cerr << '\t' << get<lambda1,value>(m)[i] * get<lambda2,value>(m)[j];
                cerr << '\t' << get<mrate,value>(m)[i][j];
                cerr << '\t' << get<K,value>(m)[i][j];
                cerr << '\n';
            }
        }
    }
}

