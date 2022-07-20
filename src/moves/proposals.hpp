/*Copyright or © or Copr. CNRS (2019). Contributors2
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

#pragma once

#include "mcmc_utils.hpp"
#include "operations/backup.hpp"
#include "operations/draw.hpp"

template <typename Gen>
double scale(double& value, double tuning, Gen& gen)    {
    auto multiplier = tuning * (draw_uniform(gen) - 0.5);
    value *= exp(multiplier);
    return multiplier;
}

template <class Gen>
double slide(double& value, double tuning, Gen& gen)    {
    auto slide_amount = tuning * (draw_uniform(gen) - 0.5);
    value += slide_amount;
    return 0.;
}

template <class Gen>
double slide_constrained(double& value, double tuning, double min, double max, Gen& gen)    {
    assert(value >= min && value <= max);
    slide(value, tuning, gen);
    while (value < min || value > max) {
        if (value < min) { value = 2 * min - value; }
        if (value > max) { value = 2 * max - value; }
    }
    return 0.;
}

template <class Gen>
double profile_move(std::vector<double>& vec, double tuning, Gen& gen) {
    size_t n = vec.size();
    assert(n > 1);

    // draw two distinct indices in the vector
    size_t i1 = std::uniform_int_distribution<size_t>(0, n - 1)(gen);
    size_t i2 = std::uniform_int_distribution<size_t>(0, n - 2)(gen);
    if (i1 <= i2) { i2 += 1; }  // avoid collision

    // slide move v1 and compensate to keep v1+v2 sum constant
    double &v1{vec[i1]}, &v2{vec[i2]};
    assert(v1 >= 0 && v2 >= 0);
    double v1_before = v1;
    slide_constrained(v1, tuning, 0, v1 + v2, gen);
    v2 += v1_before - v1;  // compensation

    return 0.;  // sliding move
}

// draw n out of N with replacement
template<class Gen>
void draw_with_replacement(std::vector<int>& tab, int N, Gen& gen) {
    int n = tab.size();
    std::vector<int> index(N,0);
    for (int i=0; i<n; i++) {
        int trial = (int)(draw_uniform(gen) * (N-i));
        for (int k=0; k<N; k++) {
            if (index[k] != 0) {
                if (trial >= k) {
                    trial++;
                }
            }
        }
        if (trial == N) {
            std::cerr << "error in draw from urn: overflow\n";
            exit(1);
        }
        tab[i] = trial;
        if (index[trial] != 0) {
            std::cerr << "error in draw from urn: chose twice the same\n";
            exit(1);
        }
        index[trial] = 1;
    }
}

template <class Gen>
double profile_move(std::vector<double>& profile, int n, double tuning, Gen& gen) {
    int dim = profile.size();
    double ret = 0;
    /*
    if (n == 0) {  // dirichlet
        double oldprofile[dim];
        for (int i = 0; i < dim; i++) {
            oldprofile[i] = profile[i];
        }
        double total = 0;
        for (int i = 0; i < dim; i++) {
            profile[i] = Random::sGamma(tuning * oldprofile[i]);
            if (profile[i] == 0) {
                std::cerr << "error in dirichlet resampling : 0 \n";
                exit(1);
            }
            total += profile[i];
        }
        double logHastings = 0;
        for (int i = 0; i < dim; i++) {
            profile[i] /= total;
            logHastings += -Random::logGamma(tuning * oldprofile[i]) +
                           Random::logGamma(tuning * profile[i]) -
                           (tuning * profile[i] - 1.0) * log(oldprofile[i]) +
                           (tuning * oldprofile[i] - 1.0) * log(profile[i]);
        }
        return logHastings;
    }
    */
    if (2*n > dim) {
        n = dim/2;
    }
    std::vector<int> indices(2*n,0);
    draw_with_replacement(indices,dim,gen);
    for (int i=0; i<n; i++) {
        int i1 = indices[2*i];
        int i2 = indices[2*i+1];
        double tot = profile[i1] + profile[i2];
        double x = profile[i1];

        double h = tot * tuning * (draw_uniform(gen) - 0.5);
        x += h;
        while ((x < 0) || (x > tot)) {
            if (x < 0) {
                x = -x;
            }
            if (x > tot) {
                x = 2 * tot - x;
            }
        }
        profile[i1] = x;
        profile[i2] = tot - x;
        if (!profile[i1]) {
            profile[i1] = 1e-50;
        }
        if (!profile[i2]) {
            profile[i2] = 1e-50;
        }
    }
    return ret;
}


struct proposals    {
    static auto scaling(double tuning) {
        return [tuning] (auto& value, auto& gen) {return scale(value, tuning, gen);};
    }

    static auto sliding(double tuning) {
        return [tuning] (auto& value, auto& gen) {return slide(value, tuning, gen);};
    }
};
