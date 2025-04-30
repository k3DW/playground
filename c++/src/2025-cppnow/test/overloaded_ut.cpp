#include "overloaded.hpp"

namespace {

void basic_usage() {
    using F1 = decltype([](int x, double y) {
        return x + y;
    });
    using F2 = decltype([](auto&&... ts) {
        return sizeof...(ts);
    });
    constexpr auto fn = overloaded(F1{}, F2{});
    static_assert(fn(3, 4) == 2);
    static_assert(fn(3, 4.0) == 7.0);
}

void final_types() {
    struct F1 final {
        constexpr int operator()(int x, double y) const {
            return x + y;
        }
    };
    using F2 = decltype([](auto&&... ts) {
        return sizeof...(ts);
    });
    constexpr auto fn = overloaded(F1{}, F2{});
    // static_assert(fn(3, 4) == 2); // Todo: fix this
    static_assert(fn(3, 4.0) == 7.0);
}

} // namespace
