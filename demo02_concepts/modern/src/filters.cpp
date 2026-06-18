#include "demo02/modern/filters.hpp"

namespace demo02::modern {

float run_lowpass(LowPass& filter, std::span<const float> samples) {
    return run_filter(filter, samples);
}

float run_highpass(HighPass& filter, std::span<const float> samples) {
    return run_filter(filter, samples);
}

} // namespace demo02::modern
