#pragma once

#include <array>
#include <charconv>
#include <cstddef>
#include <span>

namespace detail {

consteval std::size_t parse(std::span<const char> span) {
    std::size_t value;
    auto result = std::from_chars(span.data(), span.data() + span.size(), value);
    if (result.ec != std::errc() or result.ptr != span.data() + span.size()) {
        throw; // Parse failed
    }
    return value;
}

} // namespace detail

template <char... cs>
constexpr auto operator ""_c() {
    static constexpr std::array<char, sizeof...(cs)> arr{cs...};
    static constexpr auto value = detail::parse(arr);
    return std::integral_constant<std::size_t, value>{};
}

// Open questions:
//   1. What should the underlying integral type be?
//   2. Should we support string literals? Or just int literals?
//   3. `constexpr_wrapper` versus `integral_constant`?
