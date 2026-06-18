#include "demo07/modern/pipeline.hpp"

#include <array>

#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/algorithm/max_element.hpp>
#include <range/v3/algorithm/min_element.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/sliding.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/filter.hpp>

namespace demo07::modern {

namespace {
constexpr bool in_range(float v) { return v >= raw_min && v <= raw_max; }
constexpr float calibrate(float v) { return v * cal_gain + cal_offset; }
} // namespace

std::size_t process_readings(std::span<const float> raw, std::span<float> out) {
    namespace views = ranges::views;

    // Lazy: filter + calibrate run on demand — no intermediate buffer.
    auto calibrated_view =
        raw | views::filter(in_range) | views::transform(calibrate);

    // Materialize once into a fixed stack buffer (sliding needs forward).
    std::array<float, max_readings> calibrated{};
    std::size_t n = 0;
    for (float v : calibrated_view) {
        if (n == calibrated.size()) {
            break;
        }
        calibrated[n++] = v;
    }

    // Lazy sliding window, written straight into the caller's storage.
    std::size_t count = 0;
    auto averages =
        std::span<const float>(calibrated.data(), n) |
        views::sliding(static_cast<std::ptrdiff_t>(window)) |
        views::transform([](auto&& win) {
            return ranges::accumulate(win, 0.0f) / static_cast<float>(window);
        });
    for (float avg : averages) {
        if (count == out.size()) {
            break;
        }
        out[count++] = avg;
    }
    return count;
}

Stats compute_stats(std::span<const float> values) {
    if (values.empty()) {
        return Stats{0.0f, 0.0f, 0.0f, 0};
    }
    return Stats{
        *ranges::min_element(values),
        *ranges::max_element(values),
        ranges::accumulate(values, 0.0f) / static_cast<float>(values.size()),
        static_cast<std::size_t>(ranges::count_if(
            values, [](float v) { return v >= saturation_level; })),
    };
}

} // namespace demo07::modern
