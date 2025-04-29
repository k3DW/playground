#include "constant.hpp"

static_assert(0_c .value == 0);
static_assert(14_c .value == 14);
static_assert(99_c .value == 99);
static_assert(100_c .value == 100);

static_assert(std::is_same_v<decltype(0_c), std::integral_constant<std::size_t, 0>>);
static_assert(std::is_same_v<decltype(14_c), std::integral_constant<std::size_t, 14>>);
static_assert(std::is_same_v<decltype(99_c), std::integral_constant<std::size_t, 99>>);
static_assert(std::is_same_v<decltype(100_c), std::integral_constant<std::size_t, 100>>);

static_assert(9999999_c .value == 9999999);
static_assert(99999999_c .value == 99999999);
static_assert(999999999_c .value == 999999999);
static_assert(9999999999_c .value == 9999999999);
static_assert(99999999999_c .value == 99999999999);
static_assert(999999999999_c .value == 999999999999);
static_assert(9999999999999_c .value == 9999999999999);

static_assert(std::is_same_v<decltype(9999999_c), std::integral_constant<std::size_t, 9999999>>);
static_assert(std::is_same_v<decltype(99999999_c), std::integral_constant<std::size_t, 99999999>>);
static_assert(std::is_same_v<decltype(999999999_c), std::integral_constant<std::size_t, 999999999>>);
static_assert(std::is_same_v<decltype(9999999999_c), std::integral_constant<std::size_t, 9999999999>>);
static_assert(std::is_same_v<decltype(99999999999_c), std::integral_constant<std::size_t, 99999999999>>);
static_assert(std::is_same_v<decltype(999999999999_c), std::integral_constant<std::size_t, 999999999999>>);
static_assert(std::is_same_v<decltype(9999999999999_c), std::integral_constant<std::size_t, 9999999999999>>);

static_assert(4294967295_c .value == 4294967295);
static_assert(4294967296_c .value == 4294967296);

static_assert(std::is_same_v<decltype(4294967295_c), std::integral_constant<std::size_t, 4294967295>>);
static_assert(std::is_same_v<decltype(4294967296_c), std::integral_constant<std::size_t, 4294967296>>);
