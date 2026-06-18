#include "demo02/classic/filters.hpp"

namespace demo02::classic {

float LowPass::process(float sample) {
    state_ += alpha_ * (sample - state_);
    return state_;
}

float HighPass::process(float sample) {
    state_ = alpha_ * (state_ + sample - prev_input_);
    prev_input_ = sample;
    return state_;
}

float run_filter(IFilter& filter, std::span<const float> samples) {
    filter.reset();
    float last = 0.0f;
    for (float s : samples) {
        last = filter.process(s); // indirect call, every sample
    }
    return last;
}

} // namespace demo02::classic
