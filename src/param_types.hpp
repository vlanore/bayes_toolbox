#pragma once

template <typename T>
class Ref {
    T& ref;

  public:
    Ref(T& ref) : ref(ref) {}
    T& operator()() { return ref; }
    const T& operator()() const { return ref; }
};

template <typename T>
class CRef {
    const T& ref;

  public:
    CRef(const T& ref) : ref(ref) {}
    const T& operator()() const { return ref; }
};

using DRef = Ref<double>;
using CDRef = CRef<double>;
using IRef = Ref<int>;
using CIRef = CRef<int>;

namespace distrib {
    struct exponential_param_t {
        CDRef rate;
    };

    struct gamma_param_t {
        CDRef shape;
        CDRef scale;
    };
};  // namespace distrib