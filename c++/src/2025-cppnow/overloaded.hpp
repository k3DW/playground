#pragma once
#include <functional>
#include <type_traits>
#include <utility>

namespace detail::overloaded {

template <class F>
struct wrapper : private F {
    using F::operator();
    constexpr wrapper(F f) : F(std::move(f)) {}
};

template <class F>
requires std::is_final_v<F>
struct wrapper<F> {
    template <class... Args>
    requires std::invocable<F&, Args&&...>
    constexpr decltype(auto) operator()(Args&&... args) & {
        return std::invoke(_f, std::forward<Args>(args)...);
    }
    template <class... Args>
    requires std::invocable<const F&, Args&&...>
    constexpr decltype(auto) operator()(Args&&... args) const& {
        return std::invoke(_f, std::forward<Args>(args)...);
    }
    template <class... Args>
    requires std::invocable<F&&, Args&&...>
    constexpr decltype(auto) operator()(Args&&... args) && {
        return std::invoke(std::move(_f), std::forward<Args>(args)...);
    }
    template <class... Args>
    requires std::invocable<const F&&, Args&&...>
    constexpr decltype(auto) operator()(Args&&... args) const&& {
        return std::invoke(std::move(_f), std::forward<Args>(args)...);
    }
    constexpr wrapper(F f) : _f(std::move(f)) {}
    [[no_unique_address]] F _f;
};

template <class... Fs>
wrapper(Fs...) -> wrapper<Fs...>;

} // namespace detail::overloaded

template <class... Fs>
struct overloaded : private detail::overloaded::wrapper<Fs>... {
    using detail::overloaded::wrapper<Fs>::operator()...;
    constexpr overloaded(Fs... fs)
        : detail::overloaded::wrapper<Fs>(std::move(fs))... {}
};

template <class... Fs>
overloaded(Fs...) -> overloaded<Fs...>;
