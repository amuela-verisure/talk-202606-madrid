#pragma once
// Classic runtime polymorphism for a signal-filter interface:
// abstract base class + virtual dispatch, even when every call site
// knows the concrete type at compile time.
#include <cstddef>
#include <span>

namespace demo02::classic {

class IFilter {
public:
    virtual ~IFilter() = default;
    virtual void reset() = 0;
    [[nodiscard]] virtual float process(float sample) = 0;
};

class LowPass final : public IFilter {
public:
    explicit LowPass(float alpha) : alpha_{alpha} {}
    void reset() override { state_ = 0.0f; }
    [[nodiscard]] float process(float sample) override;

private:
    float alpha_;
    float state_{0.0f};
};

class HighPass final : public IFilter {
public:
    explicit HighPass(float alpha) : alpha_{alpha} {}
    void reset() override {
        prev_input_ = 0.0f;
        state_ = 0.0f;
    }
    [[nodiscard]] float process(float sample) override;

private:
    float alpha_;
    float prev_input_{0.0f};
    float state_{0.0f};
};

// Generic algorithm forced through the vtable: the compiler cannot inline
// process() here because the concrete type is erased behind IFilter&.
[[nodiscard]] float run_filter(IFilter& filter, std::span<const float> samples);

} // namespace demo02::classic
