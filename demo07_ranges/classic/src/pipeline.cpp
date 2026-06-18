#include "demo07/classic/pipeline.hpp"

namespace demo07::classic {

std::vector<float> process_readings(std::span<const float> raw) {
    // Stage 1: filter — heap buffer #1
    std::vector<float> filtered;
    filtered.reserve(raw.size());
    for (std::size_t i = 0; i < raw.size(); ++i) {
        if (raw[i] >= raw_min && raw[i] <= raw_max) {
            filtered.push_back(raw[i]);
        }
    }

    // Stage 2: calibrate — heap buffer #2
    std::vector<float> calibrated;
    calibrated.reserve(filtered.size());
    for (std::size_t i = 0; i < filtered.size(); ++i) {
        calibrated.push_back(filtered[i] * cal_gain + cal_offset);
    }

    // Stage 3: sliding-window average — heap buffer #3 (the result)
    std::vector<float> output;
    if (calibrated.size() >= window) {
        output.reserve(calibrated.size() - window + 1);
        for (std::size_t i = 0; i + window <= calibrated.size(); ++i) {
            float sum = 0.0f;
            for (std::size_t j = 0; j < window; ++j) {
                sum += calibrated[i + j];
            }
            output.push_back(sum / static_cast<float>(window));
        }
    }
    return output;
}

Stats compute_stats(std::span<const float> values) {
    Stats stats{0.0f, 0.0f, 0.0f, 0};
    if (values.empty()) {
        return stats;
    }
    stats.min = values[0];
    stats.max = values[0];
    float sum = 0.0f;
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (values[i] < stats.min) {
            stats.min = values[i];
        }
        if (values[i] > stats.max) {
            stats.max = values[i];
        }
        sum += values[i];
    }
    stats.mean = sum / static_cast<float>(values.size());
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (values[i] >= saturation_level) {
            ++stats.saturated;
        }
    }
    return stats;
}

} // namespace demo07::classic
