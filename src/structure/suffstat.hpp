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
        int _n;

        SS& _get() final { return _ss; }

      public:
        suffstat_proxy1(const SS& from, Lambda lambda, int n) : _ss(from), _lambda(lambda), _n(n) {}

        void gather() final {
            _ss.Clear();
            for (int i = 0; i < _n; i++) { _lambda(_ss, i); }
        }
    };

    template <class SS, class Lambda>
    static auto make_suffstat(Lambda lambda, int n) {
        return std::make_unique<suffstat_proxy1<SS, Lambda>>(SS(), lambda, n);
    }

    template <class SS, class Lambda>
    static auto make_suffstat_with_init(SS&& from, Lambda lambda, int n) {
        return std::make_unique<suffstat_proxy1<SS, Lambda>>(std::forward<SS>(from), lambda, n);
    }

    template <class SS, class Lambda>
    class suffstat_array_proxy0 final : public Proxy<SS&, int> {
        std::vector<SS> _ss;
        Lambda _lambda;

        SS& _get(int i) final { return _ss[i]; }
        size_t size() const { return _ss.size(); }

      public:
        suffstat_array_proxy0(size_t size, const SS& from, Lambda lambda)
            : _ss(size, from), _lambda(lambda) {}

        void gather() final {
            for (size_t i = 0; i < _ss.size(); i++) { _ss[i].Clear(); }
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
    class suffstat_array_proxy1 final : public Proxy<SS&, int> {
        std::vector<SS> _ss;
        Lambda _lambda;
        int _n;

        SS& _get(int i) final { return _ss[i]; }
        size_t size() const { return _ss.size(); }

      public:
        suffstat_array_proxy1(size_t size, const SS& from, Lambda lambda, int n)
            : _ss(size, from), _lambda(lambda), _n(n) {}

        void gather() final {
            for (size_t i = 0; i < _ss.size(); i++) { _ss[i].Clear(); }
            for (int i = 0; i < _n; i++) { _lambda(_ss, i); }
        }
    };

    template <class SS, class Lambda>
    static auto make_suffstat_array(size_t size, Lambda lambda, int n) {
        return std::make_unique<suffstat_array_proxy1<SS, Lambda>>(size, SS(), lambda, n);
    }

    template <class SS, class Lambda>
    static auto make_suffstat_array_with_init(size_t size, const SS& from, Lambda lambda, int n) {
        return std::make_unique<suffstat_array_proxy1<SS, Lambda>>(size, from, lambda, n);
    }

    template <class SS, class Lambda>
    class suffstat_matrix_proxy0 final : public Proxy<SS&, int, int> {
        std::vector<std::vector<SS>> _ss;
        Lambda _lambda;

        SS& _get(int i, int j) final { return _ss[i][j]; }
        size_t size1() const { return _ss.size(); }
        size_t size2() const { return _ss[0].size(); }

      public:
        suffstat_matrix_proxy0(size_t size1, size_t size2, const SS& from, Lambda lambda)
            : _ss(size1, std::vector<SS>(size2,from)), _lambda(lambda) {}

        void gather() final {
            for (size_t i = 0; i < _ss.size(); i++) { 
                for (size_t j=0; j< _ss[0].size(); j++) {
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
    class suffstat_matrix_proxy1 final : public Proxy<SS&, int, int> {
        std::vector<std::vector<SS>> _ss;
        Lambda _lambda;
        int _n;

        SS& _get(int i, int j) final { return _ss[i][j]; }
        size_t size1() { return _ss.size(); }
        size_t size2() { return _ss[0].size(); }

      public:
        suffstat_matrix_proxy1(size_t size1, size_t size2, const SS& from, Lambda lambda, int n)
            : _ss(size1, std::vector<SS>(size2, from)), _lambda(lambda), _n(n) {}

        void gather() final {
            for (size_t i = 0; i < _ss.size(); i++) { 
                for (size_t j=0; j< _ss[0].size(); j++) {
                    _ss[i][j].Clear(); 
                }
            }
            for (int i = 0; i < _n; i++) { _lambda(_ss, i); }
        }
    };

    template <class SS, class Lambda>
    static auto make_suffstat_matrix(size_t size1, size_t size2, Lambda lambda, int n) {
        return std::make_unique<suffstat_matrix_proxy1<SS, Lambda>>(size1, size2, SS(), lambda, n);
    }
};
