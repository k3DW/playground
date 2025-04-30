#include "constant.hpp"
#include "tuple.hpp"

constexpr mguid::tuple<int, double, char> tup{5, 6.0, 'a'};

static_assert(tup.get(0_c) == 5);
static_assert(tup.get(1_c) == 6.0);
static_assert(tup.get(2_c) == 'a');

static_assert(std::is_same_v<decltype(auto(tup.get(0_c))), int>);
static_assert(std::is_same_v<decltype(auto(tup.get(1_c))), double>);
static_assert(std::is_same_v<decltype(auto(tup.get(2_c))), char>);

static_assert(std::is_same_v<decltype(auto(tup[0_c])), int>);
static_assert(std::is_same_v<decltype(auto(tup[1_c])), double>);
static_assert(std::is_same_v<decltype(auto(tup[2_c])), char>);

namespace {

void compile_test() {
    auto [a,b,c] = tup;
}

} // namespace
