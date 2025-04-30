// Copyright Matthew Guidry
#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <numeric>
#include <limits>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace mguid {

namespace detail {

template <std::size_t I, typename... TRest>
struct type_at_index;

template <std::size_t TIdx, typename TFirst, typename... TRest>
struct type_at_index<TIdx, TFirst, TRest...>
    : type_at_index<TIdx - 1, TRest...> {};

template <typename TFirst, typename... TRest>
struct type_at_index<0, TFirst, TRest...> {
    using type = TFirst;
};

template <typename TType, typename... Ts>
constexpr std::size_t index_of_type() {
    std::size_t idx = 0, found = std::numeric_limits<std::size_t>::max();
    ((std::is_same_v<TType, Ts> ? (found = idx, false) : (++idx, false)), ...);
    return found;
}

template <typename TType>
void test_implicitly_default_constructible(TType) {}

template <typename T>
concept is_implicitly_default_constructible =
    requires() { test_implicitly_default_constructible<T>({}); };
}  // namespace detail

template <typename... T>
struct element;

template <typename T>
struct element<T> {
    template <typename TRet, std::size_t TIdx, typename Self>
    constexpr decltype(auto) get(this Self&& self) {
        static_assert(TIdx == 0, "");
        return std::forward_like<Self>(self.value);
    }

    constexpr auto operator<=>(const element&) const = default;
    T value;
};

template <typename TFirst, typename... TRest>
struct element<TFirst, TRest...> {
    template <typename TRet, std::size_t TIdx, typename Self>
    constexpr decltype(auto) get(this Self&& self) {
        if constexpr (TIdx == 0) {
            return std::forward_like<Self>(self.value);
        } else {
            return std::forward_like<Self>(self.next_element).template get<TRet, TIdx - 1>();
        }
    }
    constexpr auto operator<=>(const element&) const = default;

    TFirst value;
    element<TRest...> next_element;
};

template <typename... TTypes>
struct tuple {
    constexpr static std::size_t size = sizeof...(TTypes);

    constexpr tuple()
        requires(std::default_initializable<TTypes> && ...)
    = default;

#ifdef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
#define MISSING_BRACE_SUPPRESSED
#endif
    // perfect‐forwarding value‐ctor (also covers forward_as_tuple)
    template <typename... Us>
        requires(sizeof...(Us) == size &&
                 (std::constructible_from<TTypes, Us &&> && ...))
    constexpr tuple(Us&&... us) : data{std::forward<Us>(us)...} {}
#ifdef MISSING_BRACE_SUPPRESSED
#pragma GCC diagnostic pop
#undef MISSING_BRACE_SUPPRESSED
#endif

    template <std::size_t TIdx, typename Self>
    constexpr decltype(auto) get(this Self&& self)
        requires(TIdx < size)
    {
        return std::forward_like<Self>(self.data).template get<
            typename detail::type_at_index<TIdx, TTypes...>::type, TIdx>();
    }

    template <typename T, typename Self>
    constexpr decltype(auto) get(this Self&& self) {
        constexpr std::size_t idx{detail::index_of_type<T, TTypes...>()};
        return std::forward_like<Self>(self.data).template get<T, idx>();
    }

    template <std::size_t TIdx, typename Self>
    constexpr decltype(auto) get(this Self&& self, std::integral_constant<std::size_t, TIdx>) {
        return std::forward_like<Self>(self.data).template get<
            typename detail::type_at_index<TIdx, TTypes...>::type, TIdx>();
    }

    template <std::size_t TIdx, typename Self>
    constexpr decltype(auto) operator[](this Self&& self, std::integral_constant<std::size_t, TIdx>) {
        return std::forward_like<Self>(self.data).template get<
            typename detail::type_at_index<TIdx, TTypes...>::type, TIdx>();
    }

    constexpr void swap(tuple& other) noexcept
        requires((... && std::is_nothrow_swappable_v<TTypes>))
    {
        swap_impl(other);
    }

    constexpr auto operator<=>(const tuple&) const = default;

   private:
    template <std::size_t... Is>
    constexpr void swap_impl(tuple& other, std::index_sequence<Is...>) {
        (std::swap(this->get<Is>(), other.get<Is>()), ...);
    }

    constexpr void swap_impl(tuple& other) {
        swap_impl(other, std::make_index_sequence<size>());
    }

    element<TTypes...> data;
};

template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(tuple<Ts...>& t) {
    return t.template get<I>();
}

template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(const tuple<Ts...>& t) {
    return t.template get<I>();
}

template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(tuple<Ts...>&& t) {
    return std::move(t).template get<I>();
}

template <std::size_t I, typename... Ts>
constexpr decltype(auto) get(const tuple<Ts...>&& t) {
    return std::move(t).template get<I>();
}

template <typename T, typename... Ts>
constexpr decltype(auto) get(tuple<Ts...>& t) {
    return t.template get<T>();
}

template <typename T, typename... Ts>
constexpr decltype(auto) get(const tuple<Ts...>& t) {
    return t.template get<T>();
}

template <typename T, typename... Ts>
constexpr decltype(auto) get(tuple<Ts...>&& t) {
    return std::move(t).template get<T>();
}

template <typename T, typename... Ts>
constexpr decltype(auto) get(const tuple<Ts...>&& t) {
    return std::move(t).template get<T>();
}

template <typename... Ts>
constexpr auto make_tuple(Ts&&... args) {
    return tuple<std::decay_t<Ts>...>(std::forward<Ts>(args)...);
}

template <typename... Ts>
constexpr auto tie(Ts&... args) noexcept {
    return tuple<Ts&...>(args...);
}

template <typename... Ts>
constexpr auto forward_as_tuple(Ts&&... args) noexcept {
    return tuple<Ts&&...>(std::forward<Ts>(args)...);
}

namespace detail {

// Helper: actually applies
template <typename F, typename Tuple, std::size_t... Is>
constexpr decltype(auto) apply_impl(F&& f, Tuple&& t,
                                    std::index_sequence<Is...>) {
    return std::invoke(std::forward<F>(f),
                       mguid::get<Is>(std::forward<Tuple>(t))...);
}

}  // namespace detail

template <typename F, typename Tuple>
constexpr decltype(auto) apply(F&& f, Tuple&& t) {
    return detail::apply_impl(
        std::forward<F>(f), std::forward<Tuple>(t),
        std::make_index_sequence<std::remove_reference_t<Tuple>::size>{});
}

namespace detail {

template <typename... Ts>
constexpr auto flatten(Ts&&... tuples) {
    return mguid::apply(
        [](auto&&... args) {
            return mguid::make_tuple(std::forward<decltype(args)>(args)...);
        },
        mguid::make_tuple(std::forward<Ts>(tuples)...));
}

}  // namespace detail

template <typename... Tuples>
constexpr auto tuple_cat(Tuples&&... tuples) {
    return detail::flatten(mguid::apply(
        [](auto&&... elems) {
            return mguid::make_tuple(std::forward<decltype(elems)>(elems)...);
        },
        std::forward<Tuples>(tuples))...);
}

}  // namespace mguid

namespace std {

// tuple_size specialization
template <typename... Ts>
struct tuple_size<mguid::tuple<Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)> {};

// const version
template <typename... Ts>
struct tuple_size<const mguid::tuple<Ts...>> : tuple_size<mguid::tuple<Ts...>> {
};

// tuple_element specialization
template <std::size_t I, typename... Ts>
struct tuple_element<I, mguid::tuple<Ts...>> {
    using type = typename mguid::detail::type_at_index<I, Ts...>::type;
};

// const version
template <std::size_t I, typename... Ts>
struct tuple_element<I, const mguid::tuple<Ts...>> {
    using type =
        std::add_const_t<typename tuple_element<I, mguid::tuple<Ts...>>::type>;
};

}  // namespace std
