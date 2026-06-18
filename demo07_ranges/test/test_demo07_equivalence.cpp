#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <array>

#include "demo07/classic/pipeline.hpp"
#include "demo07/modern/pipeline.hpp"

namespace {
// Mix of valid readings, spikes (>100) and dropouts (<0).
constexpr std::array<float, 16> raw = {12.5f, 98.0f,  150.0f, 45.0f, -5.0f, 67.2f,
                                       88.8f, 102.0f, 23.4f,  55.5f, 91.0f, -1.0f,
                                       30.0f, 77.7f,  99.9f,  60.1f};
} // namespace

TEST_CASE("demo07: pipelines produce identical averages") {
    const auto classic_out = demo07::classic::process_readings(raw);

    std::array<float, 32> modern_buf{};
    const auto modern_count = demo07::modern::process_readings(raw, modern_buf);

    REQUIRE(classic_out.size() == modern_count);
    for (std::size_t i = 0; i < modern_count; ++i) {
        CAPTURE(i);
        CHECK(classic_out[i] == doctest::Approx(modern_buf[i]).epsilon(1e-6));
    }
}

TEST_CASE("demo07: stats agree between hand-rolled loops and algorithms") {
    const auto classic_stats = demo07::classic::compute_stats(raw);
    const auto modern_stats = demo07::modern::compute_stats(raw);

    CHECK(classic_stats.min == modern_stats.min);
    CHECK(classic_stats.max == modern_stats.max);
    CHECK(classic_stats.mean == doctest::Approx(modern_stats.mean));
    CHECK(classic_stats.saturated == modern_stats.saturated);
}

TEST_CASE("demo07: empty input is handled identically") {
    const std::span<const float> empty;
    std::array<float, 4> buf{};

    CHECK(demo07::classic::process_readings(empty).empty());
    CHECK(demo07::modern::process_readings(empty, buf) == 0);
    CHECK(demo07::classic::compute_stats(empty).saturated ==
          demo07::modern::compute_stats(empty).saturated);
}
