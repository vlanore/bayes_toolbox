#pragma once

#include <assert.h>

template <class T>
class Proxy<T, size_t> {
    virtual T _get(size_t i) = 0;

  protected:
    // protected non-virtual destructor, as this interface is not
    // meant to be used with owning pointers
    ~Proxy() = default;

  public:
    // @fixme? maybe this should be a private virtual _gather for consistency
    virtual void gather() = 0;

    virtual size_t size() const = 0;
    T get(size_t i) {
#ifndef NDEBUG
        auto tmp = _get(i);
        gather();
        assert(_get(i) == tmp);
#endif
        return _get(i);
    }
};

template <class T>
class Proxy<T, size_t, size_t> {
    virtual T _get(size_t i, size_t j) = 0;

  protected:
    // protected non-virtual destructor, as this interface is not
    // meant to be used with owning pointers
    ~Proxy() = default;

  public:
    // @fixme? maybe this should be a private virtual _gather for consistency
    virtual void gather() = 0;

    virtual size_t size1() const = 0;
    virtual size_t size2() const = 0;

    T get(size_t i, size_t j) {
#ifndef NDEBUG
        auto tmp = _get(i,j);
        gather();
        assert(_get(i,j) == tmp);
#endif
        return _get(i,j);
    }
};


template <class T>
class Proxy<T, size_t, size_t, size_t> {
    virtual T _get(size_t i, size_t j, size_t k) = 0;

  protected:
    // protected non-virtual destructor, as this interface is not
    // meant to be used with owning pointers
    ~Proxy() = default;

  public:
    // @fixme? maybe this should be a private virtual _gather for consistency
    virtual void gather() = 0;

    virtual size_t size1() const = 0;
    virtual size_t size2() const = 0;
    virtual size_t size3() const = 0;

    T get(size_t i, size_t j, size_t k) {
#ifndef NDEBUG
        auto tmp = _get(i,j,k);
        gather();
        assert(_get(i,j,k) == tmp);
#endif
        return _get(i,j,k);
    }
};

struct ss_factory {
    template <class SS, class Lambda>
    class suffstat_proxy0 final : public Proxy<SS&> {
        SS _ss;
        Lambda _lambda;

        SS& _get() final { return _ss; }

      public:
        suffstat_proxy0(const SS& from, Lambda lambda) : _ss(from), _lambda(lambda) {}

        void gather() final {
            _ss.Clear();
            _lambda(_ss);
        }
    };

    template <class SS, class Lambda>
    static auto make_suffstat(Lambda lambda) {
        return std::make_unique<suffstat_proxy0<SS, Lambda>>(SS(), lambda);
    }

    template <class SS, class Lambda>
    static auto make_suffstat_with_init(const SS& from, Lambda lambda) {
        return std::make_unique<suffstat_proxy0<SS, Lambda>>(from, lambda);
    }

    template <class SS, class Lambda>
    class suffstat_proxy1 final : public Proxy<SS&> {
        SS _ss;
        Lambda _lambda;
        size_t _n;

        SS& _get() final { return _ss; }

      public:
        suffstat_proxy1(const SS& from, Lambda lambda, size_t n) : _ss(from), _lambda(lambda), _n(n) {}

        void gather() final {
            _ss.Clear();
            for (size_t i=0; i<_n; i++) { _lambda(_ss, i); }
        }
    };

    template <class SS, class Lambda>
    static auto make_suffstat(Lambda lambda, size_t n) {
        return std::make_unique<suffstat_proxy1<SS, Lambda>>(SS(), lambda, n);
    }

    template <class SS, class Lambda>
    static auto make_suffstat_with_init(SS&& from, Lambda lambda, size_t n) {
        return std::make_unique<suffstat_proxy1<SS, Lambda>>(std::forward<SS>(from), lambda, n);
    }

    template <class SS, class Lambda>
    class suffstat_proxy2 final : public Proxy<SS&> {
        SS _ss;
        Lambda _lambda;
        size_t _m;
        size_t _n;

        SS& _get() final { return _ss; }

      public:
        suffstat_proxy2(const SS& from, Lambda lambda, size_t m, size_t n) : _ss(from), _lambda(lambda), _m(m), _n(n) {}

        void gather() final {
            _ss.Clear();
            for (size_t i=0; i<_m; i++) { 
                for (size_t j=0; j<_n; j++)    {
                    _lambda(_ss, i, j); 
                }
            }
        }
    };

    template <class SS, class Lambda>
    static auto make_suffstat(Lambda lambda, size_t m, size_t n) {
        return std::make_unique<suffstat_proxy2<SS, Lambda>>(SS(), lambda, m, n);
    }

    template <class SS, class Lambda>
    static auto make_suffstat_with_init(SS&& from, Lambda lambda, size_t m, size_t n) {
        return std::make_unique<suffstat_proxy2<SS, Lambda>>(std::forward<SS>(from), lambda, m, n);
    }

    template <class SS, class Lambda>
    class suffstat_proxy3 final : public Proxy<SS&> {
        SS _ss;
        Lambda _lambda;
        size_t _m;
        size_t _n;
        size_t _p;

        SS& _get() final { return _ss; }

      public:
        suffstat_proxy3(const SS& from, Lambda lambda, size_t m, size_t n, size_t p) : _ss(from), _lambda(lambda), _m(m), _n(n), _p(p) {}

        void gather() final {
            _ss.Clear();
            for (size_t i=0; i<_m; i++) { 
                for (size_t j=0; j<_n; j++)    {
                    for (size_t k=0; k<_n; k++)    {
                        _lambda(_ss, i, j, k); 
                    }
                }
            }
        }
    };

    template <class SS, class Lambda>
    static auto make_suffstat(Lambda lambda, size_t m, size_t n, size_t p) {
        return std::make_unique<suffstat_proxy3<SS, Lambda>>(SS(), lambda, m, n, p);
    }

    template <class SS, class Lambda>
    static auto make_suffstat_with_init(SS&& from, Lambda lambda, size_t m, size_t n, size_t p) {
        return std::make_unique<suffstat_proxy3<SS, Lambda>>(std::forward<SS>(from), lambda, m, n, p);
    }

    template <class SS, class Lambda>
    class suffstat_array_proxy0 final : public Proxy<SS&, size_t> {
        std::vector<SS> _ss;
        Lambda _lambda;

        SS& _get(size_t i) final { return _ss[i]; }

      public:
        suffstat_array_proxy0(size_t size, const SS& from, Lambda lambda)
            : _ss(size, from), _lambda(lambda) {}

        size_t size() const { return _ss.size(); }

        void gather() final {
            for (size_t i=0; i<_ss.size(); i++) { _ss[i].Clear(); }
            _lambda(_ss);
        }
    };

    template <class SS, class Lambda>
    static auto make_suffstat_array(size_t size, Lambda lambda) {
        return std::make_unique<suffstat_array_proxy0<SS, Lambda>>(size, SS(), lambda);
    }

    template <class SS, class Lambda>
    static auto make_suffstat_array_with_init(size_t size, const SS& from, Lambda lambda) {
        return std::make_unique<suffstat_array_proxy0<SS, Lambda>>(size, from, lambda);
    }

    template <class SS, class Lambda>
    class suffstat_array_proxy1 final : public Proxy<SS&, size_t> {
        std::vector<SS> _ss;
        Lambda _lambda;
        size_t _n;

        SS& _get(size_t i) final { return _ss[i]; }

      public:
        suffstat_array_proxy1(size_t size, const SS& from, Lambda lambda, size_t n)
            : _ss(size, from), _lambda(lambda), _n(n) {}

        size_t size() const { return _ss.size(); }

        void gather() final {
            for (size_t i=0; i<_ss.size(); i++) {
                _ss[i].Clear();
            }
            for (size_t i=0; i<_n; i++) {
                _lambda(_ss, i);
            }
        }
    };

    template <class SS, class Lambda>
    static auto make_suffstat_array(size_t size, Lambda lambda, size_t n) {
        return std::make_unique<suffstat_array_proxy1<SS, Lambda>>(size, SS(), lambda, n);
    }

    template <class SS, class Lambda>
    static auto make_suffstat_array_with_init(size_t size, const SS& from, Lambda lambda, size_t n) {
        return std::make_unique<suffstat_array_proxy1<SS, Lambda>>(size, from, lambda, n);
    }

    template <class SS, class Lambda>
    class suffstat_array_proxy2 final : public Proxy<SS&, size_t> {
        std::vector<SS> _ss;
        Lambda _lambda;
        size_t _m;
        size_t _n;

        SS& _get(size_t i) final { return _ss[i]; }

      public:
        suffstat_array_proxy2(size_t size, const SS& from, Lambda lambda, size_t m, size_t n)
            : _ss(size, from), _lambda(lambda), _m(m), _n(n) {}

        size_t size() const { return _ss.size(); }

        void gather() final {
            for (size_t i=0; i<_ss.size(); i++) {
                _ss[i].Clear();
            }
            for (size_t i=0; i<_m; i++) {
                for (size_t j=0; j<_n; j++) {
                    _lambda(_ss, i, j);
                }
            }
        }
    };

    template <class SS, class Lambda>
    static auto make_suffstat_array(size_t size, Lambda lambda, size_t m, size_t n) {
        return std::make_unique<suffstat_array_proxy2<SS, Lambda>>(size, SS(), lambda, m, n);
    }

    template <class SS, class Lambda>
    static auto make_suffstat_array_with_init(size_t size, const SS& from, Lambda lambda, size_t m, size_t n) {
        return std::make_unique<suffstat_array_proxy2<SS, Lambda>>(size, from, lambda, m, n);
    }

    template <class SS, class Lambda>
    class suffstat_matrix_proxy0 final : public Proxy<SS&, size_t, size_t> {
        std::vector<std::vector<SS>> _ss;
        Lambda _lambda;

        SS& _get(size_t i, size_t j) final { return _ss[i][j]; }

      public:
        suffstat_matrix_proxy0(size_t size1, size_t size2, const SS& from, Lambda lambda)
            : _ss(size1, std::vector<SS>(size2,from)), _lambda(lambda) {}

        size_t size1() const { return _ss.size(); }
        size_t size2() const { return _ss[0].size(); }

        void gather() final {
            for (size_t i=0; i<_ss.size(); i++) { 
                for (size_t j=0; j<_ss[0].size(); j++) {
                    _ss[i][j].Clear(); 
                }
            }
            _lambda(_ss);
        }
    };

    template <class SS, class Lambda>
    static auto make_suffstat_matrix(size_t size1, size_t size2, Lambda lambda) {
        return std::make_unique<suffstat_matrix_proxy0<SS, Lambda>>(size1, size2, SS(), lambda);
    }

    template <class SS, class Lambda>
    static auto make_suffstat_matrix_with_init(size_t size1, size_t size2, const SS& from, Lambda lambda) {
        return std::make_unique<suffstat_matrix_proxy0<SS, Lambda>>(size1, size2, from, lambda);
    }

    template <class SS, class Lambda>
    class suffstat_matrix_proxy1 final : public Proxy<SS&, size_t, size_t> {
        std::vector<std::vector<SS>> _ss;
        Lambda _lambda;
        size_t _n;

        SS& _get(size_t i, size_t j) final { return _ss[i][j]; }

      public:
        suffstat_matrix_proxy1(size_t size1, size_t size2, const SS& from, Lambda lambda, size_t n)
            : _ss(size1, std::vector<SS>(size2, from)), _lambda(lambda), _n(n) {}

        size_t size1() const { return _ss.size(); }
        size_t size2() const { return _ss[0].size(); }

        void gather() final {
            for (size_t i=0; i<_ss.size(); i++) { 
                for (size_t j=0; j<_ss[0].size(); j++) {
                    _ss[i][j].Clear(); 
                }
            }
            for (size_t i=0; i<_n; i++) {
                _lambda(_ss, i);
            }
        }
    };

    template <class SS, class Lambda>
    static auto make_suffstat_matrix(size_t size1, size_t size2, Lambda lambda, size_t n) {
        return std::make_unique<suffstat_matrix_proxy1<SS, Lambda>>(size1, size2, SS(), lambda, n);
    }

    template <class SS, class Lambda>
    static auto make_suffstat_matrix_with_init(size_t size1, size_t size2, const SS& from, Lambda lambda, size_t n) {
        return std::make_unique<suffstat_matrix_proxy1<SS, Lambda>>(size1, size2, from, lambda, n);
    }

    template <class SS, class Lambda>
    class suffstat_matrix_proxy2 final : public Proxy<SS&, size_t, size_t> {
        std::vector<std::vector<SS>> _ss;
        Lambda _lambda;
        size_t _m;
        size_t _n;

        SS& _get(size_t i, size_t j) final { return _ss[i][j]; }

      public:
        suffstat_matrix_proxy2(size_t size1, size_t size2, const SS& from, Lambda lambda, size_t m, size_t n)
            : _ss(size1, std::vector<SS>(size2, from)), _lambda(lambda), _m(m), _n(n) {}

        size_t size1() const { return _ss.size(); }
        size_t size2() const { return _ss[0].size(); }

        void gather() final {
            for (size_t i=0; i<_ss.size(); i++) { 
                for (size_t j=0; j<_ss[0].size(); j++) {
                    _ss[i][j].Clear(); 
                }
            }
            for (size_t i=0; i<_m; i++) {
                for (size_t j=0; j<_n; j++) {
                    _lambda(_ss, i, j);
                }
            }
        }
    };

    template <class SS, class Lambda>
    static auto make_suffstat_matrix(size_t size1, size_t size2, Lambda lambda, size_t m, size_t n) {
        return std::make_unique<suffstat_matrix_proxy2<SS, Lambda>>(size1, size2, SS(), lambda, m, n);
    }

    template <class SS, class Lambda>
    static auto make_suffstat_matrix_with_init(size_t size, const SS& from, Lambda lambda, size_t m, size_t n) {
        return std::make_unique<suffstat_matrix_proxy2<SS, Lambda>>(size, from, lambda, m, n);
    }

    template <class SS, class Lambda>
    class suffstat_cubix_proxy0 final : public Proxy<SS&, size_t, size_t, size_t> {
        std::vector<std::vector<std::vector<SS>>> _ss;
        Lambda _lambda;

        SS& _get(size_t i, size_t j, size_t k) final { return _ss[i][j][k]; }

      public:
        suffstat_cubix_proxy0(size_t size1, size_t size2, size_t size3, const SS& from, Lambda lambda)
            : _ss(size1, std::vector<std::vector<SS>>(size2, std::vector<SS>(size3, from))), _lambda(lambda) {}

        size_t size1() const { return _ss.size(); }
        size_t size2() const { return _ss[0].size(); }
        size_t size3() const { return _ss[0][0].size(); }

        void gather() final {
            for (size_t i=0; i<_ss.size(); i++) { 
                for (size_t j=0; j<_ss[0].size(); j++) {
                    for (size_t k=0; k<_ss[0][0].size(); k++) {
                        _ss[i][j][k].Clear(); 
                    }
                }
            }
            _lambda(_ss);
        }
    };

    template <class SS, class Lambda>
    static auto make_suffstat_cubix(size_t size1, size_t size2, size_t size3, Lambda lambda) {
        return std::make_unique<suffstat_cubix_proxy0<SS, Lambda>>(size1, size2, size3, SS(), lambda);
    }

    template <class SS, class Lambda>
    static auto make_suffstat_cubix_with_init(size_t size1, size_t size2, size_t size3, const SS& from, Lambda lambda) {
        return std::make_unique<suffstat_cubix_proxy0<SS, Lambda>>(size1, size2, size3, from, lambda);
    }

    template <class SS, class Lambda>
    class suffstat_cubix_proxy1 final : public Proxy<SS&, size_t, size_t, size_t> {
        std::vector<std::vector<std::vector<SS>>> _ss;
        Lambda _lambda;
        size_t _n;

        SS& _get(size_t i, size_t j, size_t k) final { return _ss[i][j][k]; }

      public:
        suffstat_cubix_proxy1(size_t size1, size_t size2, size_t size3, const SS& from, Lambda lambda, size_t n)
            : _ss(size1, std::vector<std::vector<SS>>(size2, std::vector<SS>(size3, from))), _lambda(lambda), _n(n) {}

        size_t size1() const { return _ss.size(); }
        size_t size2() const { return _ss[0].size(); }
        size_t size3() const { return _ss[0][0].size(); }

        void gather() final {
            for (size_t i=0; i<_ss.size(); i++) { 
                for (size_t j=0; j<_ss[0].size(); j++) {
                    for (size_t k=0; k<_ss[0][0].size(); k++) {
                        _ss[i][j][k].Clear(); 
                    }
                }
            }
            for (size_t i=0; i<_n; i++) {
                _lambda(_ss, i);
            }
        }
    };

    template <class SS, class Lambda>
    static auto make_suffstat_cubix(size_t size1, size_t size2, size_t size3, Lambda lambda, size_t n) {
        return std::make_unique<suffstat_cubix_proxy1<SS, Lambda>>(size1, size2, size3, SS(), lambda, n);
    }

    template <class SS, class Lambda>
    static auto make_suffstat_cubix_with_init(size_t size1, size_t size2, size_t size3, const SS& from, Lambda lambda, size_t n) {
        return std::make_unique<suffstat_cubix_proxy1<SS, Lambda>>(size1, size2, size3, from, lambda, n);
    }

    template <class SS, class Lambda>
    class suffstat_cubix_proxy2 final : public Proxy<SS&, size_t, size_t, size_t> {
        std::vector<std::vector<std::vector<SS>>> _ss;
        Lambda _lambda;
        size_t _m;
        size_t _n;

        SS& _get(size_t i, size_t j, size_t k) final { return _ss[i][j][k]; }

      public:
        suffstat_cubix_proxy2(size_t size1, size_t size2, size_t size3, const SS& from, Lambda lambda, size_t m, size_t n)
            : _ss(size1, std::vector<std::vector<SS>>(size2, std::vector<SS>(size3, from))), _lambda(lambda), _m(m), _n(n) {}

        size_t size1() const { return _ss.size(); }
        size_t size2() const { return _ss[0].size(); }
        size_t size3() const { return _ss[0][0].size(); }

        void gather() final {
            for (size_t i=0; i<_ss.size(); i++) { 
                for (size_t j=0; j<_ss[0].size(); j++) {
                    for (size_t k=0; k<_ss[0][0].size(); k++) {
                        _ss[i][j][k].Clear(); 
                    }
                }
            }
            for (size_t i=0; i<_m; i++) {
                for (size_t j=0; j<_n; j++) {
                    _lambda(_ss, i, j);
                }
            }
        }
    };

    template <class SS, class Lambda>
    static auto make_suffstat_cubix(size_t size1, size_t size2, size_t size3, Lambda lambda, size_t m, size_t n) {
        return std::make_unique<suffstat_cubix_proxy2<SS, Lambda>>(size1, size2, size3, SS(), lambda, m, n);
    }

    template <class SS, class Lambda>
    static auto make_suffstat_cubix_with_init(size_t size1, size_t size2, size_t size3, const SS& from, Lambda lambda, size_t m, size_t n) {
        return std::make_unique<suffstat_cubix_proxy2<SS, Lambda>>(size1, size2, size3, from, lambda, m, n);
    }

};

