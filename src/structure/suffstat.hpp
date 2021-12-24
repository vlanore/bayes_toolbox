#pragma once

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
        size_t  _n;

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
    class suffstat_array_proxy0 final : public Proxy<SS&, size_t> {
        std::vector<SS> _ss;
        Lambda _lambda;

        SS& _get(size_t i) final { return _ss[i]; }
        size_t size() const { return _ss.size(); }

      public:
        suffstat_array_proxy0(size_t size, const SS& from, Lambda lambda)
            : _ss(size, from), _lambda(lambda) {}

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
        size_t size() const { return _ss.size(); }

      public:
        suffstat_array_proxy1(size_t size, const SS& from, Lambda lambda, size_t n)
            : _ss(size, from), _lambda(lambda), _n(n) {}

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
        size_t size() const { return _ss.size(); }

      public:
        suffstat_array_proxy2(size_t size, const SS& from, Lambda lambda, size_t m, size_t n)
            : _ss(size, from), _lambda(lambda), _m(m), _n(n) {}

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
        std::vector<std::vector<SS>> _ss;
        Lambda _lambda;
        size_t _m;
        size_t _n;

        SS& _get(size_t i, size_t j, size_t k) final { return _ss[i][j][k]; }

      public:
        suffstat_cubix_proxy2(size_t size1, size_t size2, size_t size3, const SS& from, Lambda lambda, size_t m, size_t n)
            : _ss(size1, std::vector<std::vector<SS>>(size2, std::vector<SS>(size3, from))), _lambda(lambda), _m(m), _n(n) {}

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

