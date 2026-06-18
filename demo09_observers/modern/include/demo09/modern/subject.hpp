#pragma once
// Modern observer: the observer set is a variadic parameter pack, stored by
// value in a std::tuple. notify() is one fold expression — every handler is
// a concrete type, every call can inline, nothing touches the heap.
#include <concepts>
#include <cstdint>
#include <tuple>
#include <utility>

namespace demo09::modern {

struct Event {
    std::uint16_t code;
    std::int32_t value;
};

// The constraint documents (and enforces) what an observer is.
template <typename H>
concept EventHandler = std::invocable<H&, const Event&>;

template <EventHandler... Handlers>
class Subject {
public:
    explicit constexpr Subject(Handlers... handlers)
        : handlers_{std::move(handlers)...} {}

    // Fold expression over the pack: unrolled at compile time, inlined,
    // in declaration order — no loop, no indirection, no erasure.
    constexpr void notify(const Event& event) {
        std::apply([&event](auto&... handler) { (handler(event), ...); },
                   handlers_);
    }

    [[nodiscard]] static constexpr std::size_t observer_count() {
        return sizeof...(Handlers);
    }

private:
    std::tuple<Handlers...> handlers_;
};

// CTAD: Subject s{handlerA, handlerB}; — no template noise at the call site.
template <EventHandler... Handlers>
Subject(Handlers...) -> Subject<Handlers...>;

// TRADE-OFF (honest): the observer set is fixed at compile time. If you need
// runtime subscribe/unsubscribe, this is not your tool — if you don't (most
// firmware wiring is known at build time), you stop paying for it.

} // namespace demo09::modern
