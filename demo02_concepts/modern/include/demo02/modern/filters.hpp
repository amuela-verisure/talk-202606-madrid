#pragma once
// Modern static polymorphism: a concept defines the interface, templates
// dispatch at compile time. The filter is a plain value type; process()
// inlines into the caller's loop.
#include <concepts>
#include <cstddef>
#include <span>

namespace demo02::modern {

// The interface as a compile-time contract. A type that misses a member
// (or gets a signature wrong) fails right here, with the requirement named.
template <typename F>
concept Filter = requires(F f, float sample) {
    { f.reset() } -> std::same_as<void>;
    { f.process(sample) } -> std::same_as<float>;
};

struct LowPass {
    float alpha;
    float state{0.0f};

    constexpr void reset() { state = 0.0f; }
    [[nodiscard]] constexpr float process(float sample) {
        state += alpha * (sample - state);
        return state;
    }
};

struct HighPass {
    float alpha;
    float prev_input{0.0f};
    float state{0.0f};

    constexpr void reset() {
        prev_input = 0.0f;
        state = 0.0f;
    }
    [[nodiscard]] constexpr float process(float sample) {
        state = alpha * (state + sample - prev_input);
        prev_input = sample;
        return state;
    }
};

static_assert(Filter<LowPass>);
static_assert(Filter<HighPass>);

// One generic algorithm; each instantiation is fully specialized and the
// process() body is inlined — there is no call at all in the hot loop.
template <Filter F>
[[nodiscard]] constexpr float run_filter(F& filter, std::span<const float> samples) {
    filter.reset();
    float last = 0.0f;
    for (float s : samples) {
        last = filter.process(s);
    }
    return last;
}

// Out-of-line entry points so the generated code can be measured and compared
// against the classic vtable version.
[[nodiscard]] float run_lowpass(LowPass& filter, std::span<const float> samples);
[[nodiscard]] float run_highpass(HighPass& filter, std::span<const float> samples);

} // namespace demo02::modern
