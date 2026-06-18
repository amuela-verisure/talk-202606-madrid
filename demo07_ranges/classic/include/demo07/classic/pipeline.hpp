#pragma once
// Classic data pipeline: index loops, intermediate heap buffers, and the
// algorithm's intent buried in bookkeeping. Each stage re-states iteration
// logic that the language already knows how to do.
#include <cstddef>
#include <span>
#include <vector>

namespace demo07::classic {

struct Stats {
    float min;
    float max;
    float mean;
    std::size_t saturated;
};

// valid raw range, calibration and windowing constants shared by both versions
inline constexpr float raw_min = 0.0f;
inline constexpr float raw_max = 100.0f;
inline constexpr float cal_gain = 1.02f;
inline constexpr float cal_offset = 0.5f;
inline constexpr std::size_t window = 3;
inline constexpr float saturation_level = 95.0f;

// filter -> calibrate -> sliding-window average; two intermediate vectors.
[[nodiscard]] std::vector<float> process_readings(std::span<const float> raw);

// min / max / mean / count: four hand-rolled loops.
[[nodiscard]] Stats compute_stats(std::span<const float> values);

} // namespace demo07::classic
