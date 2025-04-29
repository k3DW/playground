#include <array>
#include <charconv>
#include <cstddef>
#include <span>

namespace detail {

consteval std::size_t parse(std::span<const char> span) {
    std::size_t value;
    auto result = std::from_chars(span.data(), span.data() + span.size(), value);
    if (result.ec != std::errc()) {
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
