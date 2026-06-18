#pragma once
// Modern data pipeline: composable lazy views + named algorithms.
// The code says WHAT happens to the data; iteration is the library's job.
// Uses range-v3 today; [C++23] std::ranges + std::views provide the same
// (views::slide, ranges::fold_left) and the dependency gets dropped.
#include <cstddef>
#include <span>

namespace demo07::modern {

struct Stats {
    float min;
    float max;
    float mean;
    std::size_t saturated;
};

inline constexpr float raw_min = 0.0f;
inline constexpr float raw_max = 100.0f;
inline constexpr float cal_gain = 1.02f;
inline constexpr float cal_offset = 0.5f;
inline constexpr std::size_t window = 3;
inline constexpr float saturation_level = 95.0f;

// Capacity budget instead of a growable heap buffer.
inline constexpr std::size_t max_readings = 64;

// filter | transform | sliding — caller provides the output storage,
// nothing touches the heap. Returns the number of averages written.
[[nodiscard]] std::size_t process_readings(std::span<const float> raw,
                                           std::span<float> out);

// min_element / max_element / accumulate / count_if: intent by name.
[[nodiscard]] Stats compute_stats(std::span<const float> values);

} // namespace demo07::modern
