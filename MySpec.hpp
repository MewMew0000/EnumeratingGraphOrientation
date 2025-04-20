
#include "tdzdd/DdSpec.hpp"

class Combination: public tdzdd::DdSpec<Combination, int, 2> {
    int n;
    int k;
    public:
    Combination(int n, int k): n(n), k(k) {}
    int getRoot(int& state) {
            state = 0;
            return n;
    }
    int getChild(int& state, int level, int value) const {
            state += value;
            if (--level == 0) return (state == k) ? -1 : 0;
            if (state > k) return 0;
            if (state + level < k) return 0;
            return level;
    }
};